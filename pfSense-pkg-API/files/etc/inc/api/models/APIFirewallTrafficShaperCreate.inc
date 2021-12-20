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

class APIFirewallTrafficShaperCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-trafficshaper"];
        $this->change_note = "Added firewall traffic shaper via API";
    }

    public function action() {
        # Initialize the traffic shaper configuration and save our shaper, mark the subsystem as un-applied
        $this->__init_config();
        $this->config["shaper"]["queue"][] = $this->validated_data;
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
        $this->__validate_scheduler();
        $this->__validate_bandwidthtype();    // Must run before __validate_banddwith()
        $this->__validate_bandwidth();
        $this->__validate_enabled();
        $this->__validate_qlimit();
        $this->__validate_tbrconfig();
    }

    private function __validate_interface() {
        # Check for our required `interface` payload value
        if (isset($this->initial_data["interface"])) {
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);

            # Check that the requested interface exists
            if ($this->initial_data["interface"]) {
                # Check that a traffic shaper does not already exist for this interface
                if (!$this->get_shaper_id_by_interface($this->initial_data["interface"], true)) {
                    # Set the interface and name of this shaper
                    $this->validated_data["interface"] = $this->initial_data["interface"];
                    $this->validated_data["name"] = $this->initial_data["interface"];
                } else {
                    $this->errors[] = APIResponse\get(4112);
                }
            } else {
                $this->errors[] = APIResponse\get(4111);
            }
        } else {
            $this->errors[] = APIResponse\get(4110);
        }
    }

    private function __validate_scheduler() {
        # Check for our required `scheduler` payload value
        if (isset($this->initial_data["scheduler"])) {
            # Check that the scheduler type is supported
            if (in_array($this->initial_data["scheduler"], ["HFSC", "CBQ", "FAIRQ", "CODELQ", "PRIQ"])) {
                $this->validated_data["scheduler"] = $this->initial_data["scheduler"];
            } else {
                $this->errors[] = APIResponse\get(4114);
            }
        } else {
            $this->errors[] = APIResponse\get(4113);
        }
    }

    private function __validate_bandwidthtype() {
        # Check for our required `bandwidthtype` payload value
        if (isset($this->initial_data["bandwidthtype"])) {
            # Check that the scheduler type is supported
            if (in_array($this->initial_data["bandwidthtype"], ["%", "b", "Kb", "Mb", "Gb"])) {
                $this->validated_data["bandwidthtype"] = $this->initial_data["bandwidthtype"];
            } else {
                $this->errors[] = APIResponse\get(4116);
            }
        } else {
            $this->errors[] = APIResponse\get(4115);
        }
    }

    private function __validate_bandwidth() {
        # Check for our required `bandwidth` payload value
        if (isset($this->initial_data["bandwidth"])) {
            # Check that the bandwidth is 1 or greater
            if (intval($this->initial_data["bandwidth"]) >= 1) {
                # If bandwidth type % is used, enforce value to be less than 100
                if ($this->validated_data["bandwidthtype"] === "%" and intval($this->initial_data["bandwidth"]) > 100) {
                    $this->errors[] = APIResponse\get(4119);
                } else {
                    $this->validated_data["bandwidth"] = intval($this->initial_data["bandwidth"]);
                }
            } else {
                $this->errors[] = APIResponse\get(4118);
            }
        } else {
            $this->errors[] = APIResponse\get(4117);
        }
    }

    private function __validate_enabled() {
        # Enable this shaper by default if a non-false value was provided
        if ($this->initial_data["enabled"] !== false) {
            $this->validated_data["enabled"] = "on";
        }
    }

    private function __validate_qlimit() {
        # Check for our optional `qlimit` payload value
        if (isset($this->initial_data["qlimit"])) {
            # Ensure the qlimit is 1 or greater
            if (is_numeric($this->initial_data["qlimit"]) and intval($this->initial_data["qlimit"]) >= 1) {
                $this->validated_data["qlimit"] = intval($this->initial_data["qlimit"]);
            } else {
                $this->errors[] = APIResponse\get(4120);
            }
        }
    }

    private function __validate_tbrconfig() {
        # Check for our optional `tbrconfig` payload value
        if (isset($this->initial_data["tbrconfig"])) {
            # Ensure the tbrconfig is 1 or greater
            if (is_numeric($this->initial_data["tbrconfig"]) and intval($this->initial_data["tbrconfig"]) >= 1) {
                $this->validated_data["tbrconfig"] = intval($this->initial_data["tbrconfig"]);
            } else {
                $this->errors[] = APIResponse\get(4121);
            }
        }
    }

    private function __init_config() {
        # Check if there is no shaper array in the config
        if (empty($this->config["shaper"])) {
            $this->config["shaper"] = [];
        }
        # Check if there is no shaper queue array in the config
        if (empty($this->config["shaper"]["queue"])) {
            $this->config["shaper"]["queue"] = [];
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
}
