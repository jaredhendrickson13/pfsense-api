<?php
//   Copyright 2023 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallTrafficShaperLimiterQueueDelete extends APIModel {
    private $parent_id;

    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Deleted firewall traffic shaper limiter queue via API";
    }

    public function action() {
        $this->del_config("dnshaper/queue/{$this->parent_id}/queue/{$this->id}");
        $this->write_config();
        mark_subsystem_dirty('shaper');

        # Only reload the filter immediately if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            # Reload the filter, reset RRD logs and mark the subsystem as applied
            filter_configure();
            system("rm -f /var/db/rrd/*queuedrops.rrd");
            system("rm -f /var/db/rrd/*queues.rrd");
            enable_rrd_graphing();
            clear_subsystem_dirty('shaper');
        }
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_limiter();
        $this->__validate_name();
    }

    private function __validate_limiter() {
        # Check for our required `limiter` payload value
        if (isset($this->initial_data["limiter"])) {
            if ($this->get_limiter_id_by_name($this->initial_data["limiter"], true)) {
                $this->parent_id = $this->get_limiter_id_by_name($this->initial_data["limiter"]);
                $this->validated_data["number"] = $this->get_config("dnshaper/queue/{$this->id}/number");
            } else {
                $this->errors[] = APIResponse\get(4219);
            }
        } else {
            $this->errors[] = APIResponse\get(4218);
        }
    }

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            if ($this->get_limiter_queue_id_by_name($this->initial_data["name"], $this->parent_id, true)) {
                # Ensure limiter queue is not in use
                if (!$this->is_limiter_queue_in_use($this->initial_data["name"])) {
                    $this->id = $this->get_limiter_queue_id_by_name($this->initial_data["name"], $this->parent_id);
                    $this->validated_data = $this->get_config("dnshaper/queue/{$this->parent_id}/queue/{$this->id}");
                } else {
                    $this->errors[] = APIResponse\get(4214);
                }
            } else {
                $this->errors[] = APIResponse\get(4221);
            }
        } else {
            $this->errors[] = APIResponse\get(4220);
        }
    }

    public function get_limiter_id_by_name($name, $as_bool=false) {
        # Loop through each configured limiter and see if it uses this name
        foreach ($this->get_config("dnshaper/queue", []) as $id=>$limiter) {
            # Check for matching names
            if ($name === $limiter["name"]) {
                return ($as_bool) ? true : $id;
            }
        }
        return ($as_bool) ? false : null;
    }

    public function get_limiter_queue_id_by_name($name, $limiter_id, $as_bool=false) {
        # Loop through each configured limiter queue and see if it uses this name
        foreach ($this->get_config("dnshaper/queue/{$limiter_id}/queue", []) as $id=>$queue) {
            # Check for matching names
            if ($name === $queue["name"]) {
                return ($as_bool) ? true : $id;
            }
        }
        return ($as_bool) ? false : null;
    }

    public function is_limiter_queue_in_use($name) {
        # Loop through each configured firewall rule and check if it's using this limiter
        foreach ($this->get_config("filter/rule", []) as $rule) {
            # Check for matches
            if ($rule["dnpipe"] === $name or $rule["pdnpipe"] === $name) {
                return true;
            };
        }
        return false;
    }

}
