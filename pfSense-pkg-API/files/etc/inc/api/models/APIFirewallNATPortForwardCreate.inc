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

class APIFirewallNATPortForwardCreate extends APIModel {
    private $protocols;
    private $nat_reflection;
    private $created_by_msg;
    private $port_required;

    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Added NAT rule via API";
        $this->protocols = ["tcp", "udp", "tcp/udp", "icmp", "esp", "ah", "gre", "ipv6", "igmp", "pim", "ospf"];
        $this->nat_reflection = ["enable", "disable", "purenat"];
        $this->port_required = false;
    }

    public function action() {
        # Add our new configuration
        $this->id = $this->get_next_id("nat/rule");
        $this->set_config("nat/rule/{$this->id}", $this->validated_data);
        APITools\sort_nat_rules($this->initial_data["top"], $this->id);
        $this->write_config();
        mark_subsystem_dirty('natconf');

        # Only reload the firewall filter if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            APIFirewallApplyCreate::apply();
        }
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_interface() {
        # Require clients to pass in an interface
        if (isset($this->initial_data['interface'])) {
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface'], false, true, true, true);
            # Require interface to exist
            if ($interface) {
                $this->validated_data["interface"] = $interface;
            } else {
                $this->errors[] = APIResponse\get(4006);
            }
        } else {
            $this->errors[] = APIResponse\get(4000);
        }
    }

    private function __validate_protocol() {
        # Require client to pass in a protocol
        if (isset($this->initial_data['protocol'])) {
            # Require protocol to be a known/supported protocol
            if (in_array($this->initial_data['protocol'], $this->protocols)) {
                $this->validated_data["protocol"] = $this->initial_data['protocol'];
                # Set our port required toggle to true if our protocol depends on a port
                if (in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) {
                    $this->port_required = true;
                }
            } else {
                $this->errors[] = APIResponse\get(4007);
            }
        } else {
            $this->errors[] = APIResponse\get(4001);
        }
    }

    private function __validate_target() {
        # Require client to pass in an internal target for the port forward
        if (isset($this->initial_data['target'])) {
            # Require the target to either be a valid IPv4 or alias
            if (in_array(is_ipaddroralias($this->initial_data['target']), [4, true])) {
                $this->validated_data["target"] = $this->initial_data['target'];
            } else {
                $this->errors[] = APIResponse\get(4009);
            }
        } else {
            $this->errors[] = APIResponse\get(4002);
        }
    }

    private function __validate_local_port() {
        # Only require a local port if the protocol requires a port
        if ($this->port_required) {
            # Require client to pass in a local port to forward to the target
            if (isset($this->initial_data['local-port'])) {
                # Require the port to be a valid TCP/UDP port or range
                if (is_port_or_alias(strval($this->initial_data['local-port']))) {
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
        # Require to specify a source address
        if (isset($this->initial_data['src'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['src'], "source");
            if ($rule_check["valid"]) {
                $this->validated_data["source"] = $rule_check["data"]["source"];
            } else {
                $this->errors[] = APIResponse\get(4011);
            }
        } else {
            $this->errors[] = APIResponse\get(4004);
        }
    }

    private function __validate_dst() {
        # Require to specify a destination address
        if (isset($this->initial_data['dst'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($rule_check["valid"]) {
                $this->validated_data["destination"] = $rule_check["data"]["destination"];
            } else {
                $this->errors[] = APIResponse\get(4012);
            }
        } else {
            $this->errors[] = APIResponse\get(4005);
        }
    }

    private function __validate_srcport() {
        # Only require a source port value if our protocol requires ports
        if ($this->port_required) {
            $this->initial_data['srcport'] = str_replace("-", ":", $this->initial_data['srcport']);
            # Require port to be a valid port or range, or be any
            if (!is_port_or_range_or_alias($this->initial_data['srcport']) and $this->initial_data['srcport'] !== "any") {
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
            if (!is_port_or_range_or_alias($this->initial_data['dstport']) and $this->initial_data['dstport'] !== "any") {
                $this->errors[] = APIResponse\get(4014);
            }
            # If our value is not any, replace the port range character with a - and save the value
            elseif ($this->initial_data['dstport'] !== "any") {
                $this->validated_data["destination"]["port"] = str_replace(":", "-", $this->initial_data['dstport']);
            }
        }
    }

    private function __validate_disabled() {
        # Optionally allow client to disable port forward upon entry
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        }
    }

    private function __validate_nordr() {
        # Optionally allow client to disable RDR for this entry
        if ($this->initial_data['nordr'] === true) {
            $this->validated_data["nordr"] = "";
        }
    }
    private function __validate_nosync() {
        # Optionally allow client to disable pfSync for this entry
        if ($this->initial_data['nosync'] === true) {
            $this->validated_data["nosync"] = "";
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
        $this->created_by_msg = $this->client->username."@".$this->client->ip_address." (API)";
        $this->validated_data["associated-rule-id"] = "pass";
        $this->validated_data["created"] = array("time" => time(), "username" => $this->created_by_msg);
        $this->validated_data["updated"] = $this->validated_data["created"];
    }
}
