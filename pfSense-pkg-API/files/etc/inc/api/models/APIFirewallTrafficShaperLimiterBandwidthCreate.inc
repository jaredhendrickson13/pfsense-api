<?php
//   Copyright 2021 Jared Hendrickson
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


class APIFirewallTrafficShaperLimiterBandwidthCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-trafficshaper-limiter"];
        $this->change_note = "Added firewall traffic shaper limiter bandwidth via API";
    }

    public function action() {
        $this->__init_config();
        $this->config["dnshaper"]["queue"][$this->id]["bandwidth"]["item"][] = $this->validated_data;
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

    public function validate_payload($nested=false) {
        # Don't require validation of the name on nested API calls
        if (!$nested) {
            $this->__validate_name();
        }
        $this->__validate_bw();
        $this->__validate_bwscale();
        $this->__validate_bwsched();

        # Set static fields
        $this->validated_data["burst"] = "";
    }

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            if ($this->get_limiter_id_by_name($this->initial_data["name"], true)) {
                $this->id = $this->get_limiter_id_by_name($this->initial_data["name"]);
            } else {
                $this->errors[] = APIResponse\get(4209);
            }
        } else {
            $this->errors[] = APIResponse\get(4167);
        }
    }

    private function __validate_bw() {
        # Check for our required `bw` payload value
        if (isset($this->initial_data["bw"])) {
            if (is_numeric($this->initial_data["bw"]) and intval($this->initial_data["bw"] >= 1)) {
                $this->validated_data["bw"] = intval($this->initial_data["bw"]);
            } else {
                $this->errors[] = APIResponse\get(4211);
            }
        } else {
            $this->errors[] = APIResponse\get(4210);
        }
    }

    private function __validate_bwscale() {
        # Check for our required `bwscale` payload value
        if (isset($this->initial_data["bwscale"])) {
            if (in_array($this->initial_data["bwscale"], ["b", "Kb", "Mb"])) {
                $this->validated_data["bwscale"] = $this->initial_data["bwscale"];
            } else {
                $this->errors[] = APIResponse\get(4213);
            }
        } else {
            $this->errors[] = APIResponse\get(4212);
        }
    }

    private function __validate_bwsched() {
        # Check for our required `bwsched` payload value
        if (isset($this->initial_data["bwsched"])) {
            if ($this->is_firewall_schedule($this->initial_data["bwsched"])) {
                $this->validated_data["bwsched"] = $this->initial_data["bwsched"];
            } else {
                $this->errors[] = APIResponse\get(4150);
            }
        } else {
            $this->validated_data["bwsched"] = "none";
        }
    }

    public function get_limiter_id_by_name($name, $as_bool=false) {
        # Loop through each configured limiter and see if it uses this name
        foreach ($this->config["dnshaper"]["queue"] as $id=>$limiter) {
            # Check for matching names
            if ($name === $limiter["name"]) {
                return ($as_bool) ? true : $id;
            }
        }
        return ($as_bool) ? false : null;
    }

    public function is_firewall_schedule($name) {
        # Loop through each configured firewall schedule and check if one exists with a provided name
        foreach ($this->config["schedules"]["schedule"] as $schedule) {
            # Check if the provided name matches this schedule's name
            if ($name === $schedule["name"]) {
                return true;
            }
        }
        return false;
    }

    private function __init_config() {
        # Check if there is no dnshaper array in the config
        if (empty($this->config["dnshaper"]["queue"][$this->id]["queue"])) {
            $this->config["dnshaper"]["queue"][$this->id]["queue"] = [];
        }
        # Check if there is no dnshaper queue array in the config
        if (empty($this->config["dnshaper"]["queue"][$this->id]["queue"]["item"])) {
            $this->config["dnshaper"]["queue"][$this->id]["queue"]["item"] = [];
        }
    }
}