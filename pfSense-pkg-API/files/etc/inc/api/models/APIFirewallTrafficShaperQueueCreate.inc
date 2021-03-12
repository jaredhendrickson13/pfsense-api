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

class APIFirewallTrafficShaperQueueCreate extends APIModel {
    public $p_sched;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-trafficshaper-queues"];
        $this->change_note = "Added firewall traffic shaper queue via API";
    }

    public function action() {
        # Initialize the traffic shaper configuration and save our shaper, mark the subsystem as un-applied
        $this->__init_config();
        $this->config["shaper"]["queue"][$this->id]["queue"][] = $this->validated_data;
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
        $this->__validate_name();
        $this->__validate_priority();
        $this->__validate_qlimit();
        $this->__validate_description();
        $this->__validate_enabled();
        $this->__validate_default();
        $this->__validate_red();
        $this->__validate_rio();
        $this->__validate_ecn();
        $this->__validate_codel();
        $this->__validate_bandwidthtype();
        $this->__validate_bandwidth();
        $this->__validate_buckets();
        $this->__validate_hogs();
        $this->__validate_borrow();
    }

    private function __validate_interface() {
        # Check for our required `interface` payload value
        if (isset($this->initial_data["interface"])) {
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);

            # Check that the requested interface exists
            if ($this->initial_data["interface"]) {
                # Check that a traffic shaper is configured for this interface
                if ($this->get_shaper_id_by_interface($this->initial_data["interface"], true)) {
                    # Set the interface this queue will be applied to
                    $this->validated_data["interface"] = $this->initial_data["interface"];
                    $this->p_sched = $this->config["shaper"]["queue"][$this->id]["scheduler"];
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

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            # Ensure the name only contains alphanumeric, underscores, and hyphens
            if (preg_match('/^[\w-]+$/', $this->initial_data["name"])) {
                # Ensure the name is greater than 1 character and less than equal to 15 characters
                if (strlen($this->initial_data["name"]) >= 1 and strlen($this->initial_data["name"]) <= 15) {
                    # Ensure the name is not already in use
                    if (!$this->is_queue_name_in_use($this->initial_data["name"], $this->validated_data["interface"])) {
                        $this->validated_data["name"] = $this->initial_data["name"];
                    } else {
                        $this->errors[] = APIResponse\get(4126);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4125);
                }
            } else {
                $this->errors[] = APIResponse\get(4124);
            }
        } else {
            $this->errors[] = APIResponse\get(4123);
        }
    }

    private function __validate_priority() {
        # Only validate this field if the parent shaper's scheduler is set to FAIRQ CBQ or PRIQ
        if (in_array($this->p_sched, ["FAIRQ", "CBQ", "PRIQ"])) {
            # Check for our optional `priority` value
            if (isset($this->initial_data["priority"])) {
                # For FAIRQ and CBQ schedulers, force priority to be between 0 and 7
                if (in_array($this->p_sched, ["FAIRQ", "CBQ"])) {
                    if (intval($this->initial_data["priority"]) < 0 or intval($this->initial_data["priority"]) > 7) {
                        $this->errors[] = APIResponse\get(4127);
                    } else {
                        $this->validated_data = intval($this->initial_data["priority"]);
                    }
                }
                # For PRIQ schedulers, force priority to be between 0 and 15
                elseif (in_array($this->p_sched, ["PRIQ"])) {
                    if (intval($this->initial_data["priority"]) < 0 or intval($this->initial_data["priority"]) > 15) {
                        $this->errors[] = APIResponse\get(4128);
                    } else {
                        $this->validated_data = intval($this->initial_data["priority"]);
                    }
                }
            } else {
                $this->validated_data["priority"] = 1;
            }
        }
    }

    private function __validate_description() {
        # Check for the optional `description` payload value
        if (isset($this->initial_data["description"])) {
            $this->validated_data = strval($this->initial_data["description"]);
        }
    }

    private function __validate_enabled() {
        # Enable this shaper by default if a non-false value was provided
        if ($this->initial_data["enabled"] !== false) {
            $this->validated_data["enabled"] = "on";
        }
    }

    private function __validate_default() {
        # Set this queue as the default if requested
        if ($this->initial_data["default"] === true) {
            $this->validated_data["default"] = "default";
        }
    }

    private function __validate_red() {
        # Set the Random Early Detection option if requested
        if ($this->initial_data["red"] === true) {
            $this->validated_data["red"] = "yes";
        }
    }

    private function __validate_rio() {
        # Set the Random Early Detection In and Out option if requested
        if ($this->initial_data["rio"] === true) {
            $this->validated_data["rio"] = "yes";
        }
    }

    private function __validate_ecn() {
        # Set the Explicit Congestion Notification option if requested
        if ($this->initial_data["ecn"] === true) {
            $this->validated_data["ecn"] = "yes";
        }
    }

    private function __validate_codel() {
        # Set the Codel Active Queue option if requested
        if ($this->initial_data["codel"] === true) {
            $this->validated_data["codel"] = "yes";
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

    private function __validate_bandwidthtype() {
        # Only validate this field if the parent shaper's scheduler is set to FAIRQ or CBQ
        if (in_array($this->p_sched, ["FAIRQ", "CBQ"])) {
            # Check for our optional `bandwidthtype` payload value
            if (isset($this->initial_data["bandwidthtype"])) {
                # Check that the scheduler type is supported
                if (in_array($this->initial_data["bandwidthtype"], ["%", "b", "Kb", "Mb", "Gb"])) {
                    $this->validated_data["bandwidthtype"] = $this->initial_data["bandwidthtype"];
                } else {
                    $this->errors[] = APIResponse\get(4116);
                }
            } else {
                $this->validated_data["bandwidthtype"] = "Mb";
            }
        }
    }

    private function __validate_bandwidth() {
        # Only validate this field if the parent shaper's scheduler is set to FAIRQ or CBQ
        if (in_array($this->p_sched, ["FAIRQ", "CBQ"])) {
            # Check for our optional `bandwidth` payload value
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
                $this->validated_data["bandwidth"] = "";
            }
        }
    }

    private function __validate_buckets() {
        # Only validate this field if the parent shaper's scheduler is set to FAIRQ
        if (in_array($this->p_sched, ["FAIRQ"])) {
            # Check for the optional `buckets` payload value
            if (isset($this->initial_data["buckets"])) {
                $this->validated_data = strval($this->initial_data["buckets"]);
            }
        }
    }

    private function __validate_hogs() {
        # Only validate this field if the parent shaper's scheduler is set to FAIRQ
        if (in_array($this->p_sched, ["FAIRQ"])) {
            # Check for the optional `hogs` payload value
            if (isset($this->initial_data["hogs"])) {
                $this->validated_data = strval($this->initial_data["hogs"]);
            }
        }
    }

    private function __validate_borrow() {
        # Only validate this field if the parent shaper's scheduler is set to FAIRQ
        if (in_array($this->p_sched, ["FAIRQ"])) {
            # Set the borrows options if requested
            if ($this->initial_data["borrow"] === true) {
                $this->validated_data["borrow"] = "yes";
            }
        }
    }

    private function __init_config() {
        # Check if there is no shaper queue array in the config
        if (empty($this->config["shaper"]["queue"][$this->id]["queue"])) {
            $this->config["shaper"]["queue"][$this->id]["queue"] = [];
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

    public function is_queue_name_in_use($name, $interface) {
        # Only proceed if interface has a traffic shaper configured
        if ($this->get_shaper_id_by_interface($interface, true)) {
            # Fetch our shaper config ID
            $id = $this->get_shaper_id_by_interface($interface);

            # Loop through each queue configured for this shaper
            foreach ($this->config["shaper"]["queue"][$id]["queue"] as $queue) {
                # Check if this queue's name matches our requested name
                if ($name === $queue["name"]) {
                    return true;
                }
            }
            return false;
        }
    }
}
