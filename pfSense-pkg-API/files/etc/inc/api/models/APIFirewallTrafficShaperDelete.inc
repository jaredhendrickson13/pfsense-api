<?php
//   Copyright 2022 Jared Hendrickson
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

class APIFirewallTrafficShaperDelete extends APIModel {
    private $type_changed;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-trafficshaper"];
        $this->change_note = "Deleted firewall traffic shaper via API";
    }

    public function action() {
        # Save the updated shaper, mark the subsystem as un-applied
        unset($this->config["shaper"]["queue"][$this->id]);
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
        $this->__validate_interface();
    }

    private function __validate_interface() {
        # Check for our required `interface` payload value
        if (isset($this->initial_data["interface"])) {
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);

            # Check that the requested interface exists
            if ($this->initial_data["interface"]) {
                # Check that a traffic shaper does not already exist for this interface
                if ($this->get_shaper_id_by_interface($this->initial_data["interface"], true)) {
                    # Ensure no child queues are in use before deleting
                    if (!$this->is_child_queue_in_use($this->initial_data["interface"])) {
                        # Fetch the traffic shaper object associated with this interface
                        $this->id = $this->get_shaper_id_by_interface($this->initial_data["interface"]);
                        $this->validated_data = $this->config["shaper"]["queue"][$this->id];
                    } else {
                        $this->errors[] = APIResponse\get(4230);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4122);
                }
            } else {
                $this->errors[] = APIResponse\get(4111);
            }
        } else {
            $this->errors[] = APIResponse\get(4110);
        }
    }

    public function get_shaper_id_by_interface($interface, $as_bool=false) {
        # Loop through each configured shaper
        foreach ($this->config["shaper"]["queue"] as $id=>$shaper) {
            # Check if this is the shaper for our interface, if so return it's ID
            if ($interface === $shaper["interface"]) {
                return ($as_bool) ? true : intval($id);
            }
        }
        return ($as_bool) ? false : null;
    }

    public function is_child_queue_in_use($interface) {
        # Get our traffic shaper ID
        $id = $this->get_shaper_id_by_interface($interface);

        # Loop through each child queue to check if it is in use
        foreach ($this->config["shaper"]["queue"][$id]["queue"] as $child) {
            # Loop through each firewall rule to check if the child queue is configured in the rule
            foreach ($this->config["filter"]["rule"] as $rule) {
                # It is in use if the defaultqueue or ackqueue use this queue
                if ($child["name"] === $rule["defaultqueue"] or $child["name"] === $rule["ackqueue"]) {
                    return true;
                }
            }
        }
        return false;
    }
}
