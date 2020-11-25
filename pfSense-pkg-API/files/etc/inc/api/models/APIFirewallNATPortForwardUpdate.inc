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

class APIFirewallNATPortForwardUpdate extends APIModel {
    private $protocols;
    private $nat_reflection;
    private $updated_by_msg;
    private $port_required;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified NAT rule via API";
        $this->privileges = ["page-all", "page-firewall-nat-portforward-edit"];
        $this->protocols = ["tcp", "udp", "tcp/udp", "icmp", "esp", "ah", "gre", "ipv6", "igmp", "pim", "ospf"];
        $this->nat_reflection = ["enable", "disable", "purenat"];
        $this->port_required = false;
    }

    public function action() {
        # Update our new configuration
        $this->config["nat"]["rule"][$this->id] = $this->validated_data;
        APITools\sort_nat_rules($this->initial_data["top"], $this->id);
        $this->write_config();
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Require clients to specify a port forward ID to modify
        if (isset($this->initial_data["id"])) {
            # Require the port forward ID specified to exist
            if (array_key_exists($this->initial_data["id"], $this->config["nat"]["rule"])) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->config["nat"]["rule"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(4016);
            }
        } else {
            $this->errors[] = APIResponse\get(4015);
        }
    }

    private function __validate_interface() {
        # Optionally allow clients to update the associated interface
        if (isset($this->initial_data['interface'])) {
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Require interface to exist
            if ($interface) {
                $this->validated_data["interface"] = $interface;
            } else {
                $this->errors[] = APIResponse\get(4006);
            }
        }
    }

    private function __validate_protocol() {
        # Optionally allow client to update the protocol
        if (isset($this->initial_data['protocol'])) {
            # Require protocol to be a known/supported protocol
            if (in_array($this->initial_data['protocol'], $this->protocols)) {
                # Only require ports if updating to port protocol from non-port protocol
                if (in_array($this->initial_data["protocol"], ["tcp", "udp", "tcp/udp"])) {
                    if (!in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) {
                        $this->port_required = true;
                    }
                }
                $this->validated_data["protocol"] = $this->initial_data['protocol'];
            } else {
                $this->errors[] = APIResponse\get(4007);
            }
        }
    }

    private function __validate_target() {
        # Optionally allow client to update internal target for the port forward
        if (isset($this->initial_data['target'])) {
            # Require the target to either be a valid IPv4 or IPv6 address
            if (is_ipaddrv4($this->initial_data['target'])) {
                $this->validated_data["target"] = $this->initial_data['target'];
            } elseif (alias_in_use($this->initial_data['target'])) {
                $this->validated_data["target"] = $this->initial_data['target'];
            } else {
                $this->errors[] = APIResponse\get(4009);
            }
        }
    }

    private function __validate_local_port() {
        # Only require a local port if the protocol requires a port
        if ($this->port_required) {
            # Require client to pass in a local port to forward to the target
            if (isset($this->initial_data['local-port'])) {
                # Require the port to be a valid TCP/UDP port or range
                if (is_port_or_range(strval($this->initial_data['local-port']))) {
                    $this->validated_data["local-port"] = $this->initial_data['local-port'];
                } else {
                    $this->errors[] = APIResponse\get(4010);
                }
            } else {
                $this->errors[] = APIResponse\get(4003);
            }
        }
    }

    private function __validate_src() {
        # Optionally allow client to update source address
        if (isset($this->initial_data['src'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['src'], "source");
            if ($rule_check["valid"]) {
                $this->validated_data["source"] = $rule_check["data"]["source"];
            } else {
                $this->errors[] = APIResponse\get(4011);
            }
        }
    }

    private function __validate_dst() {
        # Optionally allow client to update destination address
        if (isset($this->initial_data['dst'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($rule_check["valid"]) {
                $this->validated_data["destination"] = $rule_check["data"]["destination"];
            } else {
                $this->errors[] = APIResponse\get(4012);
            }
        }
    }

    private function __validate_srcport() {
        # Only require a source port value if our protocol requires ports
        if ($this->port_required) {
            $this->initial_data['srcport'] = str_replace("-", ":", $this->initial_data['srcport']);
            # Require port to be a valid port or range, or be any
            if (!is_port_or_range($this->initial_data['srcport']) and $this->initial_data['srcport'] !== "any") {
                    $this->errors[] = APIResponse\get(4013);
            }
            # If our value is not any, replace the port range character with a - and save the value
            elseif ($this->initial_data['srcport'] !== "any") {
                $this->validated_data["source"]["port"] = str_replace(":", "-", $this->initial_data['srcport']);
            }
        }
    }

    private function __validate_dstport() {
        # Only require a destination port value if our protocol requires ports
        if ($this->port_required) {
            $this->initial_data['dstport'] = str_replace("-", ":", $this->initial_data['dstport']);
            # Require port to be a valid port or range, or be any
            if (!is_port_or_range($this->initial_data['dstport']) and $this->initial_data['dstport'] !== "any") {
                $this->errors[] = APIResponse\get(4014);
            }
            # If our value is not any, replace the port range character with a - and save the value
            elseif ($this->initial_data['dstport'] !== "any") {
                $this->validated_data["destination"]["port"] = str_replace(":", "-", $this->initial_data['dstport']);
            }
        }
    }

    private function __validate_disabled() {
        # Optionally allow client to enable or disable port forward upon update
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        }
        elseif($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    private function __validate_nordr() {
        # Optionally allow client to enable or disable RDR upon update
        if ($this->initial_data['nordr'] === true) {
            $this->validated_data["nordr"] = "";
        }
        elseif($this->initial_data["nordr"] === false) {
            unset($this->validated_data["nordr"]);
        }
    }
    private function __validate_nosync() {
        # Optionally allow client to enable or disable pfSync upon update
        if ($this->initial_data['nosync'] === true) {
            $this->validated_data["nosync"] = "";
        }
        elseif($this->initial_data["nosync"] === false) {
            unset($this->validated_data["nosync"]);
        }
    }

    private function __validate_descr() {
        # Optionally allow client to specify a description for this entry
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = $this->initial_data["descr"];
        }
    }

    private function __validate_natreflection() {
        # Optionally allow client to specify the NAT reflection mode
        if (isset($this->initial_data['natreflection'])) {
            # Ensure client specified a support NAT reflection mode
            if (in_array($this->initial_data['natreflection'], $this->nat_reflection)) {
                $this->validated_data["natreflection"] = $this->initial_data['natreflection'];
            } else {
                $this->errors[] = APIResponse\get(4008);
            }
        }
    }

    public function validate_payload() {
        # Run each field validator method
        $this->__validate_id();
        $this->__validate_interface();
        $this->__validate_protocol();
        $this->__validate_target();
        $this->__validate_local_port();
        $this->__validate_src();
        $this->__validate_dst();
        $this->__validate_srcport();
        $this->__validate_dstport();
        $this->__validate_disabled();
        $this->__validate_nordr();
        $this->__validate_nosync();
        $this->__validate_descr();
        $this->__validate_natreflection();

        # Add static field values
        $this->updated_by_msg = $this->client->username."@".$this->client->ip_address." (API)";
        $this->validated_data["associated-rule-id"] = "pass";
        $this->validated_data["updated"] = array("time" => time(), "username" => $this->updated_by_msg);
    }

    public function apply() {
        # Mark the NAT subsystem as changed, this will be cleared if applied
        mark_subsystem_dirty('natconf');

        # Allow clients to apply this rule immediately if they passed in an apply value
        if ($this->initial_data["apply"] === true) {
            filter_configure();
            clear_subsystem_dirty('natconf');
            clear_subsystem_dirty('filter');
        }
    }
}