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

class APIRoutingGatewayUpdate extends APIModel {
    private $ip_protocol_changed;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-gateways-editgateway"];
        $this->change_note = "Modified gateway via API";
        $this->ip_protocol_changed = false;
    }

    public function action() {
        $this->config["gateways"]["gateway_item"][$this->id] = $this->validated_data;
        $this->write_config();
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Require clients to pass in a gateway ID to locate the gateway to update
        if (isset($this->initial_data['id'])) {
            # Check that our ID exists
            if (array_key_exists($this->initial_data['id'], $this->config["gateways"]["gateway_item"])) {
                $this->id = $this->initial_data['id'];
                $this->validated_data = $this->config["gateways"]["gateway_item"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(6028);
            }
        } else {
            $this->errors[] = APIResponse\get(6027);
        }
    }

    private function __validate_interface() {
        # Optionally allow clients to update the gateway interface
        if (isset($this->initial_data['interface'])) {
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Require interface to exist
            if ($interface) {
                $this->validated_data["interface"] = $interface;
            } else {
                $this->errors[] = APIResponse\get(6008);
            }
        }
    }

    private function __validate_ipprotocol() {
        # Optionally allow clients to update the IP protocol
        if (isset($this->initial_data["ipprotocol"])) {
            # Require the IP protocol to be a supported protocol
            if (in_array($this->initial_data["ipprotocol"], ["inet", "inet6"])) {
                $this->validated_data["ipprotocol"] = $this->initial_data["ipprotocol"];
                $this->ip_protocol_changed = true;
            } else {
                $this->errors[] = APIResponse\get(6010);
            }
        }
    }

    private function __validate_name() {
        # Optionally allow clients to update the name of this gateway
        if (isset($this->initial_data["name"])) {
            # Require the gateway name to be valid and not in use by another gateway
            if (!is_validaliasname($this->initial_data["name"])) {
                $this->errors[] = APIResponse\get(6012);
            } elseif (APITools\is_gateway($this->initial_data["name"]) and $this->initial_data["name"] !== $this->validated_data["name"]) {
                # Only throw an error if the updated name doesn't match the existing name
                $this->errors[] = APIResponse\get(6026);
            } else {
                $this->validated_data["name"] = $this->initial_data["name"];
            }
        }
    }

    private function __validate_gateway() {
        # Optionally allow client to update the gateway address. Require this if the IP protocol has been updated
        if (isset($this->initial_data["gateway"]) or $this->ip_protocol_changed) {
            # Revalidate the existing gateway value in case the IP protocol was changed but no value was passed in
            if (!isset($this->initial_data["gateway"])) {
                $this->initial_data["gateway"] = $this->validated_data["gateway"];
            }

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
        }
    }

    private function __validate_monitor() {
        # Optionally allow client to update the gateway monitor address.
        if (isset($this->initial_data["monitor"]) or $this->ip_protocol_changed) {
            # Revalidate the existing monitor value in case the IP protocol was changed but no value was passed in
            if (!isset($this->initial_data["monitor"])) {
                $this->initial_data["monitor"] = $this->validated_data["monitor"];
            }

            # If our monitor value is blank, use the gateway address as default
            if (empty($this->initial_data["monitor"])) {
                $this->initial_data["monitor"] = $this->validated_data["gateway"];
            }

            # Require monitor address to be valid IPv4 or IPv6 address depending on the protocol
            if ($this->validated_data["ipprotocol"] === "inet" and is_ipaddrv4($this->initial_data["monitor"])) {
                $this->validated_data["monitor"] = $this->initial_data["monitor"];
            } elseif ($this->validated_data["ipprotocol"] === "inet6" and is_ipaddrv6($this->initial_data["monitor"])) {
                $this->validated_data["monitor"] = $this->initial_data["monitor"];
            } else {
                $this->errors[] = APIResponse\get(6025);
            }
        }
    }

    private function __validate_monitor_disable() {
        # Optionally allow client to enable or disable gateway monitoring
        if ($this->initial_data['monitor_disable'] === true) {
            $this->validated_data["monitor_disable"] = "";
        } elseif ($this->initial_data['monitor_disable'] === false) {
            unset($this->validated_data["monitor_disable"]);
        }
    }

    private function __validate_action_disable() {
        # Optionally allow client to disable gateway actions
        if ($this->initial_data['action_disable'] === true) {
            $this->validated_data["action_disable"] = "";
        } elseif ($this->initial_data['action_disable'] === false) {
            unset($this->validated_data["action_disable"]);
        }
    }

    private function __validate_force_down() {
        # Optionally allow client to enable or disable forcing this gateway to be considered down
        if ($this->initial_data['force_down'] === true) {
            $this->validated_data["force_down"] = "";
        } elseif ($this->initial_data['force_down'] === false) {
            unset($this->validated_data["force_down"]);
        }
    }

    private function __validate_disabled() {
        # Optionally allow client to enable or disable gateway
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        } elseif ($this->initial_data['disabled'] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    private function __validate_descr() {
        # Optionally allow client to update the description for this entry
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
        } elseif (empty($this->validated_data["weight"])) {
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
        } elseif (empty($this->validated_data["data_payload"])) {
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
        } elseif (empty($this->validated_data["latencylow"])) {
            $this->validated_data["latencylow"] = 200;
        }
    }

    private function __validate_latencyhigh() {
        # Optionally allow client to set the high threshold for latency monitoring
        if (isset($this->initial_data["latencyhigh"])) {
            # Require latencyhigh to be greater than low latency threshold
            if ($this->initial_data["latencyhigh"] > intval($this->validated_data["latencylow"])) {
                $this->validated_data["latencyhigh"] = intval($this->initial_data["latencyhigh"]);
            } else {
                $this->errors[] = APIResponse\get(6018);
            }
        } elseif (empty($this->validated_data["latencyhigh"])) {
            $this->validated_data["latencyhigh"] = 500;
        }
    }

    private function __validate_losslow() {
        # Before validating, set the fields default value if it is empty
        if (empty($this->validated_data["losslow"])) {
            $this->validated_data["losslow"] = 10;
        }

        # Optionally allow client to set the low threshold for packet loss monitoring
        if (isset($this->initial_data["losslow"])) {
            # Require losslow to be greater or equal than 1 and less than or equal to 100
            if ($this->initial_data["losslow"] >= 1 and 100 >= $this->initial_data["losslow"]) {
                $this->validated_data["losslow"] = intval($this->initial_data["losslow"]);
            } else {
                $this->errors[] = APIResponse\get(6019);
            }
        }
    }

    private function __validate_losshigh() {
        # Before validating, set the fields default value if it is empty
        if (empty($this->validated_data["losshigh"])) {
            $this->validated_data["losshigh"] = 20;
        }

        # Optionally allow client to set the high threshold for packet loss monitoring
        if (isset($this->initial_data["losshigh"])) {
            # Require losshigh to be greater or equal than 1 and less than the low loss threshold
            if ($this->initial_data["losshigh"] >= intval($this->validated_data["losslow"]) and 100 >= $this->initial_data["losshigh"]) {
                $this->validated_data["losshigh"] = intval($this->initial_data["losshigh"]);
            } else {
                $this->errors[] = APIResponse\get(6020);
            }
        }
    }

    private function __validate_interval() {
        # Before validating, set the fields default value if it is empty
        if (empty($this->validated_data["interval"])) {
            $this->validated_data["interval"] = 500;
        }

        # Optionally allow client to set the milliseconds between ICMP probes
        if (isset($this->initial_data["interval"])) {
            # Require interval to be between 1 and 3600000
            if ($this->initial_data["interval"] >= 1 and 3600000 >= $this->initial_data["interval"]) {
                $this->validated_data["interval"] = intval($this->initial_data["interval"]);
            } else {
                $this->errors[] = APIResponse\get(6021);
            }
        }
    }

    private function __validate_loss_interval() {
        # Before validating, set the fields default value if it is empty
        if (empty($this->validated_data["time_period"])) {
            $this->validated_data["time_period"] = 60000;
        }

        # Optionally allow client to set time in milliseconds when packets will be considered lost
        if (isset($this->initial_data["loss_interval"])) {
            # Require loss_interval to be greater than high latency threshold
            if ($this->initial_data["loss_interval"] >= intval($this->validated_data["latencyhigh"])) {
                $this->validated_data["loss_interval"] = intval($this->initial_data["loss_interval"]);
            } else {
                $this->errors[] = APIResponse\get(6022);
            }
        } elseif (empty($this->validated_data["loss_interval"])) {
            $this->validated_data["loss_interval"] = 2000;
        }
    }

    private function __validate_time_period() {
        # Before validating, set the fields default value if it is empty
        if (empty($this->validated_data["time_period"])) {
            $this->validated_data["time_period"] = 60000;
        }

        # Optionally allow client to set time in milliseconds which results will be averaged
        if (isset($this->initial_data["time_period"])) {
            # Require time_period to be greater than twice the probe interval plus the loss interval
            $min = 2 * intval($this->validated_data["interval"]) + intval($this->validated_data["loss_interval"]);
            if ($this->initial_data["time_period"] > $min) {
                $this->validated_data["time_period"] = intval($this->initial_data["time_period"]);
            } else {
                $this->errors[] = APIResponse\get(6023);
            }
        }
    }

    private function __validate_alert_interval() {
        # Before validating, set the fields default value if it is empty
        if (empty($this->validated_data["alert_interval"])) {
            $this->validated_data["alert_interval"] = 1000;
        }

        # Optionally allow client to set time in milliseconds which alert conditions are checked
        if (isset($this->initial_data["alert_interval"])) {
            # Require alert_interval to be greater than twice the probe interval plus the loss interval
            if ($this->initial_data["alert_interval"] >= intval($this->validated_data["interval"])) {
                $this->validated_data["alert_interval"] = intval($this->initial_data["alert_interval"]);
            } else {
                $this->errors[] = APIResponse\get(6024);
            }
        }
    }

    public function validate_payload() {
        $this->__validate_id();
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
            if ($gateway["friendlyiface"] === $interface and $gateway["ipprotocol"] === $ipprotocol and $gateway["dynamic"]) {
                $dyn_gw = true;
            }
        }

        # Loop through each configured gateway and check if this interface already has a dynamic gateway set
        foreach ($this->config["gateways"]["gateway_item"] as $id=>$gateway) {
            if ($gateway["interface"] === $interface and $gateway["ipprotocol"] === $ipprotocol and $gateway["gateway"] === "dynamic") {
                # Allow updating this entry if the ID matches
                if ($this->id !== $id) {
                    $dyn_gw_exists = true;
                }
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
        # Mark the routing subsystem as changed, clear if applied
        mark_subsystem_dirty("staticroutes");

        # Optionally allow clients to apply this gateway immediately if they passed in a true apply value
        if ($this->initial_data["apply"] === true) {
            system_routing_configure();
            system_resolvconf_generate();
            filter_configure();
            setup_gateways_monitor();
            send_event("service reload dyndnsall");
            clear_subsystem_dirty("staticroutes");
        }
    }
}
