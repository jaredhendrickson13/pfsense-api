<?php
//   Copyright 2020 Jared Hendrickson
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


class APIRoutingStaticRouteUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-staticroutes-editroute"];
        $this->change_note = "Modified static route via API";
    }

    public function action() {
        APITools\delete_static_route($this->id);
        $this->config["staticroutes"]["route"][$this->id] = $this->validated_data;
        $this->write_config();
        $this->validated_data["id"] = count($this->config["staticroutes"]["route"]) - 1;
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Require clients to pass in an 'id' payload value
        if (isset($this->initial_data["id"])) {
            if (array_key_exists($this->initial_data["id"], $this->config["staticroutes"]["route"])) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->config["staticroutes"]["route"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(6006);
            }
        } else {
            $this->errors[] = APIResponse\get(6005);
        }
    }

    private function __validate_network() {
        # Optionally allow clients to update the 'network' value
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

            } elseif (is_ipaddrv6($ip)) {
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
        }
    }

    private function __validate_gateway() {
        # Optionally allow clients to update the 'gateway' value
        if (isset($this->initial_data["gateway"])) {
            # Ensure this is a valid gateway
            if (APITools\is_gateway($this->initial_data["gateway"])) {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            } else {
                $this->errors[] = APIResponse\get(6004);
            }
        }
    }

    private function __validate_descr() {
        # Optionally allow clients to update the 'descr' value
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = strval($this->initial_data["descr"]);
        } elseif(empty($this->validated_data["descr"])) {
            $this->validated_data["descr"] = "";
        }
    }

    private function __validate_disabled() {
        # Optionally allow clients to update the 'disabled' value
        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = "";
        } elseif($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
        $this->__validate_network();
        $this->__validate_gateway();
        $this->__validate_descr();
        $this->__validate_disabled();
    }

    public function apply() {
        # Mark the routing subsystem as changed, clear if applied
        mark_subsystem_dirty("staticroutes");

        # Optionally allow clients to apply this route immediately if they passed in a true apply value
        # Note: this is a one-off case where this was better to default to true instead of false.
        if (empty($this->initial_data["apply"]) or $this->initial_data["apply"] === true) {
            system_routing_configure();
            system_resolvconf_generate();
            filter_configure();
            setup_gateways_monitor();
            send_event("service reload dyndnsall");
            clear_subsystem_dirty("staticroutes");
        }
    }
}