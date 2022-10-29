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

class APIRoutingGatewayCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-gateways-editgateway"];
        $this->change_note = "Added gateway via API";
    }

    public function action() {
        save_gateway($this->validated_data);
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_interface() {
        # Require clients to pass in an interface
        if (isset($this->initial_data['interface'])) {
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Require interface to exist
            if ($interface) {
                $this->validated_data["interface"] = $interface;
            } else {
                $this->errors[] = APIResponse\get(6008);
            }
        } else {
            $this->errors[] = APIResponse\get(6007);
        }
    }

    private function __validate_ipprotocol() {
        # Require client to specify an IP protocol
        if (isset($this->initial_data["ipprotocol"])) {
            # Require the IP protocol to be a supported protocol
            if (in_array($this->initial_data["ipprotocol"], ["inet", "inet6"])) {
                $this->validated_data["ipprotocol"] = $this->initial_data["ipprotocol"];
            } else {
                $this->errors[] = APIResponse\get(6010);
            }
        } else {
            $this->errors[] = APIResponse\get(6009);
        }
    }

    private function __validate_name() {
        # Require client to pass in a name for this gateway
        if (isset($this->initial_data["name"])) {
            # Require the gateway name to be valid and not in use already
            if (!is_validaliasname($this->initial_data["name"])) {
                $this->errors[] = APIResponse\get(6012);
            } elseif (APITools\is_gateway($this->initial_data["name"])) {
                $this->errors[] = APIResponse\get(6026);
            } else {
                $this->validated_data["name"] = $this->initial_data["name"];
            }
        } else {
            $this->errors[] = APIResponse\get(6011);
        }
    }

    private function __validate_gateway() {
        # Require client to pass in a gateway address
        if (isset($this->initial_data["gateway"])) {
            # Always set non-local gateway to reduce conditions
            $this->validated_data["nonlocalgateway"] = "yes";

            # Gateway can be valid IPv4 address
            if ($this->validated_data["ipprotocol"] === "inet" and is_ipaddrv4($this->initial_data["gateway"])) {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            }
            # Gateway can be valid IPv6 address
            elseif ($this->validated_data["ipprotocol"] === "inet6" and is_ipaddrv6($this->initial_data["gateway"])) {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            }
            # Gateway can be dynamic if one does not already exist and a dynamic gateway is supported
            elseif ($this->initial_data["gateway"] === "dynamic") {
                # Check if this interface and IP protocol combo is eligible for a dynamic interface
                if ($this->allow_dynamic_gw($this->validated_data["interface"], $this->validated_data["ipprotocol"])) {
                    $this->validated_data["gateway"] = $this->initial_data["gateway"];
                } else {
                    $this->errors[] = APIResponse\get(6029);
                }
            }
            # Otherwise this gateway is not valid
            else {
                $this->errors[] = APIResponse\get(6014);
            }
        } else {
            $this->errors[] = APIResponse\get(6013);
        }
    }

    private function __validate_monitor() {
        # Optionally allow client to pass in a alternate gateway monitor address. Default to gateway address.
        if (isset($this->initial_data["monitor"])) {
            # Require gateway address to be valid IPv4 or IPv6 address depending on the protocol
            if ($this->validated_data["ipprotocol"] === "inet" and is_ipaddrv4($this->initial_data["monitor"])) {
                $this->validated_data["monitor"] = $this->initial_data["monitor"];
            } elseif ($this->validated_data["ipprotocol"] === "inet6" and is_ipaddrv6($this->initial_data["monitor"])) {
                $this->validated_data["monitor"] = $this->initial_data["monitor"];
            } else {
                $this->errors[] = APIResponse\get(6025);
            }
        }
        # Automatically assume the gateway address unless it is a dynamic gateway
        elseif($this->validated_data["gateway"] !== "dynamic") {
            $this->validated_data["monitor"] = $this->validated_data["gateway"];
        }
    }

    private function __validate_monitor_disable() {
        # Optionally allow client to disable gateway monitoring
        if ($this->initial_data['monitor_disable'] === true) {
            $this->validated_data["monitor_disable"] = "";
        }
    }

    private function __validate_action_disable() {
        # Optionally allow client to disable gateway actions
        if ($this->initial_data['action_disable'] === true) {
            $this->validated_data["action_disable"] = "";
        }
    }

    private function __validate_force_down() {
        # Optionally allow client to force this gateway to be considered down
        if ($this->initial_data['force_down'] === true) {
            $this->validated_data["force_down"] = "";
        }
    }

    private function __validate_disabled() {
        # Optionally allow client to disable gateway upon entry
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        }
    }

    private function __validate_descr() {
        # Optionally allow client to specify a description for this entry
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = $this->initial_data["descr"];
        }
    }

    private function __validate_weight() {
        # Optionally allow client to specify this gateway's weight when load balancing. Otherwise assume default
        if (isset($this->initial_data["weight"])) {
            # Require weight to be between 1 and 30
            if ($this->initial_data["weight"] >= 1 and 30 >= $this->initial_data["weight"]) {
                $this->validated_data["weight"] = intval($this->initial_data["weight"]);
            } else {
                $this->errors[] = APIResponse\get(6015);
            }
        } else {
            $this->validated_data["weight"] = 1;
        }
    }

    private function __validate_data_payload() {
        # Optionally allow client to specify a custom data payload to send ICMP monitoring packets
        if (isset($this->initial_data["data_payload"])) {
            # Require data payload to be greater than 1
            if ($this->initial_data["data_payload"] >= 1) {
                $this->validated_data["data_payload"] = intval($this->initial_data["data_payload"]);
            } else {
                $this->errors[] = APIResponse\get(6016);
            }
        } else {
            $this->validated_data["data_payload"] = 1;
        }
    }

    private function __validate_latencylow() {
        # Optionally allow client to set the low threshold for latency monitoring
        if (isset($this->initial_data["latencylow"])) {
            # Require latencylow to be greater or equal than 1
            if ($this->initial_data["latencylow"] >= 1) {
                $this->validated_data["latencylow"] = intval($this->initial_data["latencylow"]);
            } else {
                $this->errors[] = APIResponse\get(6017);
            }
        } else {
            $this->validated_data["latencylow"] = 200;
        }
    }

    private function __validate_latencyhigh() {
        # Optionally allow client to set the high threshold for latency monitoring
        if (isset($this->initial_data["latencyhigh"])) {
            # Require latencyhigh to be greater than low latency threshold
            if ($this->initial_data["latencyhigh"] > $this->validated_data["latencylow"]) {
                $this->validated_data["latencyhigh"] = intval($this->initial_data["latencyhigh"]);
            } else {
                $this->errors[] = APIResponse\get(6018);
            }
        } else {
            $this->validated_data["latencyhigh"] = 500;
        }
    }

    private function __validate_losslow() {
        # Optionally allow client to set the low threshold for packet loss monitoring
        if (isset($this->initial_data["losslow"])) {
            # Require losslow to be greater or equal than 1 and less than or equal to 100
            if ($this->initial_data["losslow"] >= 1 and 100 >= $this->initial_data["losslow"]) {
                $this->validated_data["losslow"] = intval($this->initial_data["losslow"]);
            } else {
                $this->errors[] = APIResponse\get(6019);
            }
        } else {
            $this->validated_data["losslow"] = 10;
        }
    }

    private function __validate_losshigh() {
        # Optionally allow client to set the high threshold for packet loss monitoring
        if (isset($this->initial_data["losshigh"])) {
            # Require losshigh to be greater or equal than 1 and less than the low loss threshold
            if ($this->initial_data["losshigh"] >= $this->validated_data["losslow"] and 100 >= $this->initial_data["losshigh"]) {
                $this->validated_data["losshigh"] = intval($this->initial_data["losshigh"]);
            } else {
                $this->errors[] = APIResponse\get(6020);
            }
        } else {
            $this->validated_data["losshigh"] = 20;
        }
    }

    private function __validate_interval() {
        # Optionally allow client to set the milliseconds between ICMP probes
        if (isset($this->initial_data["interval"])) {
            # Require interval to be between 1 and 3600000
            if ($this->initial_data["interval"] >= 1 and 3600000 >= $this->initial_data["interval"]) {
                $this->validated_data["interval"] = intval($this->initial_data["interval"]);
            } else {
                $this->errors[] = APIResponse\get(6021);
            }
        } else {
            $this->validated_data["interval"] = 500;
        }
    }

    private function __validate_loss_interval() {
        # Optionally allow client to set time in milliseconds when packets will be considered lost
        if (isset($this->initial_data["loss_interval"])) {
            # Require loss_interval to be greater than high latency threshold
            if ($this->initial_data["loss_interval"] >= $this->validated_data["latencyhigh"]) {
                $this->validated_data["loss_interval"] = intval($this->initial_data["loss_interval"]);
            } else {
                $this->errors[] = APIResponse\get(6022);
            }
        } else {
            $this->validated_data["loss_interval"] = 2000;
        }
    }

    private function __validate_time_period() {
        # Optionally allow client to set time in milliseconds which results will be averaged
        if (isset($this->initial_data["time_period"])) {
            # Require time_period to be greater than twice the probe interval plus the loss interval
            $min = 2 * intval($this->validated_data["interval"]) + intval($this->validated_data["loss_interval"]);
            if ($this->initial_data["time_period"] > $min) {
                $this->validated_data["time_period"] = intval($this->initial_data["time_period"]);
            } else {
                $this->errors[] = APIResponse\get(6023);
            }
        } else {
            $this->validated_data["time_period"] = 60000;
        }
    }

    private function __validate_alert_interval() {
        # Optionally allow client to set time in milliseconds which alert conditions are checked
        if (isset($this->initial_data["alert_interval"])) {
            # Require alert_interval to be greater than twice the probe interval plus the loss interval
            if ($this->initial_data["alert_interval"] >= $this->validated_data["interval"]) {
                $this->validated_data["alert_interval"] = intval($this->initial_data["alert_interval"]);
            } else {
                $this->errors[] = APIResponse\get(6024);
            }
        } else {
            $this->validated_data["alert_interval"] = 1000;
        }
    }

    public function validate_payload() {
        $this->__validate_disabled();
        $this->__validate_interface();
        $this->__validate_ipprotocol();
        $this->__validate_name();
        $this->__validate_gateway();
        $this->__validate_monitor_disable();
        $this->__validate_action_disable();
        $this->__validate_monitor();
        $this->__validate_force_down();
        $this->__validate_descr();
        $this->__validate_weight();
        $this->__validate_data_payload();
        $this->__validate_latencylow();
        $this->__validate_latencyhigh();
        $this->__validate_losslow();
        $this->__validate_losshigh();
        $this->__validate_interval();
        $this->__validate_loss_interval();
        $this->__validate_time_period();
        $this->__validate_alert_interval();
    }

    # Checks if this interface already has a saved configuration for a dynamic gateway using this IP protocol
    public function allow_dynamic_gw($interface, $ipprotocol) {
        # Local variables
        $gateway_details = return_gateways_array(true, true);
        $interface = APITools\get_pfsense_if_id($interface);
        $dyn_gw = false;
        $dyn_gw_exists = false;

        # Loop through each system gateway and see if this interface is dynamic
        foreach ($gateway_details as $gateway) {
            if ($gateway["friendlyiface"] === $interface and $gateway["ipprotocol"] === $ipprotocol and isset($gateway["dynamic"])) {
                $dyn_gw = true;
            }
        }

        # Loop through each configured gateway and check if this interface already has a dynamic gateway set
        foreach ($this->get_config("gateways/gateway_item", []) as $gateway) {
            if ($gateway["interface"] === $interface and $gateway["ipprotocol"] === $ipprotocol and $gateway["gateway"] === "dynamic") {
                $dyn_gw_exists = true;
            }
        }

        # Check if dynamic gateways should be supported and if one already exists
        if ($dyn_gw and !$dyn_gw_exists) {
            return true;
        } else {
            return false;
        }
    }

    public function apply() {
        # Apply these changes immediately if requested
        if ($this->initial_data["apply"] === true) {
            APIRoutingApplyCreate::apply();
        }
    }
}
