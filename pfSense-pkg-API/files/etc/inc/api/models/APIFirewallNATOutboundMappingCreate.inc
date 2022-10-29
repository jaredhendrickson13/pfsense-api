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

class APIFirewallNATOutboundMappingCreate extends APIModel {
    private $protocols;
    private $pool_options;
    private $created_by_msg;
    private $port_supported;
    private $pool_source_hash_supported;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Added outbound NAT mapping via API";
        $this->privileges = ["page-all", "page-firewall-nat-outbound-edit"];
        $this->protocols = ["any", "tcp", "udp", "tcp/udp", "icmp", "esp", "ah", "gre", "ipv6", "igmp", "pim", "ospf"];
        $this->pool_options = ["round-robin", "round-robin sticky-address", "random", "random sticky-address", "source-hash", "bitmask"];
        $this->port_supported = false;
        $this->pool_source_hash_supported = false;
    }

    public function action() {
        # Add our new configuration
        $this->id = count($this->get_config("nat/outbound/rule"));
        $this->set_config("nat/outbound/rule/{$this->id}", $this->validated_data);
        APITools\sort_nat_rules($this->initial_data["top"], $this->id, "outbound");
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
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Require interface to exist
            if ($interface) {
                $this->validated_data["interface"] = $interface;
            } else {
                $this->errors[] = APIResponse\get(4087);
            }
        } else {
            $this->errors[] = APIResponse\get(4086);
        }
    }

    private function __validate_protocol() {
        # Require client to pass in a protocol
        if (isset($this->initial_data['protocol'])) {
            # Require protocol to be a known/supported protocol
            if (in_array($this->initial_data['protocol'], $this->protocols)) {
                # Only add the protocol if it is not any (XML expects no entry for any)
                if ($this->initial_data["protocol"] !== "any") {
                    $this->validated_data["protocol"] = $this->initial_data['protocol'];
                    # Set our port supported toggle to true if our protocol uses ports
                    if (in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) {
                        $this->port_supported = true;
                    }
                }
            } else {
                $this->errors[] = APIResponse\get(4089);
            }
        } else {
            $this->errors[] = APIResponse\get(4088);
        }
    }

    private function __validate_poolopts() {
        # Optionally allow clients to pass in a poolopts, otherwise assume empty string (default pool option)
        if (isset($this->initial_data['poolopts'])) {
            # Require option to be a known/supported pool option
            if (in_array($this->initial_data['poolopts'], $this->pool_options)) {
                $this->validated_data["poolopts"] = $this->initial_data['poolopts'];
                # Set our hash key supported toggle to true if our pool option is source hash
                if ($this->validated_data["poolopts"] === "source-hash") {
                    $this->pool_source_hash_supported = true;
                }
            } else {
                $this->errors[] = APIResponse\get(4090);
            }
        } else {
            $this->validated_data["poolopts"] = "";
        }
    }

    private function __validate_source_hash_key() {
        # Only allow client to set source hash key if it is the current pool option
        if ($this->pool_source_hash_supported) {
            # Allow client to specify source hash, otherwise generate a random one
            if (isset($this->initial_data["source_hash_key"])) {
                # Set an error if the hash key does not start with 0x
                if (substr($this->initial_data["source_hash_key"],0,2) !== "0x") {
                    $this->errors[] = APIResponse\get(4091);
                }
                # Set an error if the hash key does contain a hexadecimal value after 0x
                elseif (!ctype_xdigit(substr($this->initial_data['source_hash_key'],2))) {
                    $this->errors[] = APIResponse\get(4092);
                }
                # Set an error if the hash key is not exactly 34 characters in length include 0x
                elseif (strlen($this->initial_data['source_hash_key']) !== 34) {
                    $this->errors[] = APIResponse\get(4093);
                }
                # Otherwise, hash key appears to be valid. Save the validated value.
                else {
                    $this->validated_data["source_hash_key"] = $this->initial_data['source_hash_key'];
                }
            } else {
                $this->validated_data["source_hash_key"] = "0x".md5($this->initial_data['source_hash_key']);
            }
        }
    }

    private function __validate_target() {
        # Require client to pass in an external target for the mapping
        if (isset($this->initial_data['target'])) {
            # Require the target to be a valid IPv4 subnet, address, alias or empty string for iface address
            if (APITools\is_ip_subnet_or_alias($this->initial_data['target']) === "ipv4_subnet") {
                $this->validated_data["target"] = "other-subnet";
                $this->validated_data['targetip'] = explode("/", $this->initial_data["target"])[0];
                $this->validated_data['targetip_subnet'] = explode("/", $this->initial_data["target"])[1];
            } elseif (APITools\is_ip_subnet_or_alias($this->initial_data['target']) === "ipv4_addr") {
                $this->validated_data["target"] = "other-subnet";
                $this->validated_data["targetip"] = $this->initial_data['target'];
                $this->validated_data['targetip_subnet'] = 32;
            } elseif (APITools\is_ip_subnet_or_alias($this->initial_data['target']) === "alias") {
                # Only allow an alias if round-robin is chosen as the pool option
                if (in_array($this->validated_data["poolopts"], ["round-robin", "round-robin sticky-address"])) {
                    $this->validated_data["target"] = $this->initial_data['target'];
                } else {
                    $this->errors[] = APIResponse\get(4096);
                }
            } elseif ($this->initial_data["target"] === "") {
                $this->validated_data["target"] = "";
                $this->validated_data['targetip'] = "";
                $this->validated_data['targetip_subnet'] = "";
            } else {
                $this->errors[] = APIResponse\get(4095);
            }
        } else {
            $this->errors[] = APIResponse\get(4094);
        }
    }

    private function __validate_staticnatport() {
        # Only a static NAT port value if the protocol supports a port
        if ($this->port_supported and $this->initial_data["staticnatport"] === true) {
            $this->validated_data["staticnatport"] = "";
        }
    }

    private function __validate_natport() {
        # Only allow a NAT port if the protocol supports a port
        if ($this->port_supported and !$this->validated_data["staticnatport"]) {
            # Optionally allow client to pass in a NAT port
            if (isset($this->initial_data['natport'])) {
                # Require the port to be a valid TCP/UDP port or range
                if (is_port_or_range(strval($this->initial_data['natport']))) {
                    $this->validated_data["natport"] = $this->initial_data['natport'];
                } else {
                    $this->errors[] = APIResponse\get(4097);
                }
            }
        }
    }

    private function __validate_src() {
        # Require to specify a source address
        if (isset($this->initial_data['src'])) {
            if ($this->initial_data['src'] === "any") {
                $this->validated_data['source']["network"] = "any";
            } elseif (APITools\is_ip_subnet_or_alias($this->initial_data["src"])) {
                $this->validated_data['source']["network"] = $this->initial_data["src"];
            } else {
                $this->errors[] = APIResponse\get(4099);
            }
        } else {
            $this->errors[] = APIResponse\get(4098);
        }
    }

    private function __validate_dst() {
        # Require to specify a destination address
        if (isset($this->initial_data['dst'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($rule_check["valid"]) {
                $this->validated_data["destination"] = $rule_check["data"]["destination"];
            } else {
                $this->errors[] = APIResponse\get(4101);
            }
        } else {
            $this->errors[] = APIResponse\get(4100);
        }
    }

    private function __validate_srcport() {
        # Optionally allow a source port if it is supported and a source port was passed in
        if ($this->port_supported and isset($this->initial_data["srcport"])) {
            $this->initial_data['srcport'] = str_replace("-", ":", $this->initial_data['srcport']);
            # Require port to be a valid port or range, or be any
            if (!is_port_or_range($this->initial_data['srcport']) and $this->initial_data['srcport'] !== "any") {
                $this->errors[] = APIResponse\get(4102);
            }
            # If our value is not any, replace the port range character with a - and save the value
            elseif ($this->initial_data['srcport'] !== "any") {
                $this->validated_data["sourceport"] = str_replace(":", "-", $this->initial_data['srcport']);
            }
        }
    }

    private function __validate_dstport() {
        # Optionally allow a dest port if it is supported and a dest port was passed in
        if ($this->port_supported and isset($this->initial_data["dstport"])) {
            $this->initial_data['dstport'] = str_replace("-", ":", $this->initial_data['dstport']);
            # Require port to be a valid port or range, or be any
            if (!is_port_or_range($this->initial_data['dstport']) and $this->initial_data['dstport'] !== "any") {
                $this->errors[] = APIResponse\get(4103);
            }
            # If our value is not any, replace the port range character with a - and save the value
            elseif ($this->initial_data['dstport'] !== "any") {
                $this->validated_data["dstport"] = str_replace(":", "-", $this->initial_data['dstport']);
            }
        }
    }

    private function __validate_disabled() {
        # Optionally allow client to disable port forward upon entry
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        }
    }

    private function __validate_nonat() {
        # Optionally allow client to disable NAT for traffic matching this entry
        if ($this->initial_data['nonat'] === true) {
            $this->validated_data["nonat"] = "";
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

    public function validate_payload() {
        # Run each field validator method
        $this->__validate_interface();
        $this->__validate_protocol();
        $this->__validate_poolopts();
        $this->__validate_source_hash_key();
        $this->__validate_target();
        $this->__validate_staticnatport();
        $this->__validate_natport();
        $this->__validate_src();
        $this->__validate_dst();
        $this->__validate_srcport();
        $this->__validate_dstport();
        $this->__validate_disabled();
        $this->__validate_nonat();
        $this->__validate_nosync();
        $this->__validate_descr();

        # Add static field values
        $this->created_by_msg = $this->client->username."@".$this->client->ip_address." (API)";
        $this->validated_data["created"] = array("time" => time(), "username" => $this->created_by_msg);
        $this->validated_data["updated"] = $this->validated_data["created"];
    }
}
