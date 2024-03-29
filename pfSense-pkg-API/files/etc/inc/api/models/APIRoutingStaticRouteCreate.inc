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

class APIRoutingStaticRouteCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-staticroutes-editroute"];
        $this->change_note = "Added static route via API";
    }

    public function action() {
        # Add this static route to config
        $this->id = $this->get_next_id('staticroutes/route');
        $this->set_config("staticroutes/route/{$this->id}", $this->validated_data);
        $this->write_config();

        # Mark the routing subsystem as changed
        mark_subsystem_dirty("staticroutes");
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_network() {
        # Require clients to specify a network value in their payload
        if (isset($this->initial_data['network'])) {
            $subnet = intval(explode("/", $this->initial_data["network"])[1]);
            $ip = explode("/", $this->initial_data["network"])[0];
            # Check if address is IPv4, IPv6 or alias. Otherwise return an error
            if (is_ipaddrv4($ip)) {
                # Ensure the subnet bit mask is within range for IPv4 addresses
                if (is_numeric($subnet) and ($subnet >= 1 and $subnet <= 32)) {
                    $this->validated_data["network"] = $this->initial_data["network"];
                } else {
                    $this->errors[] = APIResponse\get(6002);
                }

            }
            elseif (is_ipaddrv6($ip)) {
                # Ensure the subnet bit mask is within range for IPv6 addresses
                if (is_numeric($subnet) and ($subnet >= 1 and $subnet <= 128)) {
                    $this->validated_data["network"] = $this->initial_data["network"];
                } else {
                    $this->errors[] = APIResponse\get(6002);
                }
            } elseif (is_alias($ip)) {
                $this->validated_data["network"] = $this->initial_data["network"];
            } else {
                $this->errors[] = APIResponse\get(6001);
            }
        } else {
            $this->errors[] = APIResponse\get(6000);
        }
    }

    private function __validate_gateway() {
        # Require clients to specify a 'gateway' payload value
        if (isset($this->initial_data["gateway"])) {
            # Ensure this is a valid gateway
            if (APITools\is_gateway($this->initial_data["gateway"])) {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            } else {
                $this->errors[] = APIResponse\get(6004);
            }
        } else {
            $this->errors[] = APIResponse\get(6003);
        }
    }

    private function __validate_desr() {
        # Optionally allow clients to pass in a 'descr' payload value
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = strval($this->initial_data["descr"]);
        } else {
            $this->validated_data["descr"] = "";
        }
    }

    private function __validate_disabled() {
        # Optionally allow clients to pass in a 'disabled' payload value
        if ($this->initial_data["disabled"] === true ) {
            $this->validated_data["disabled"] = "";
        }
    }

    public function validate_payload() {
        $this->__validate_network();
        $this->__validate_gateway();
        $this->__validate_desr();
        $this->__validate_disabled();
    }

    public function apply() {
        # Apply these changes immediately if requested
        if ($this->initial_data["apply"] === true) {
            APIRoutingApplyCreate::apply();
        }
    }
}
