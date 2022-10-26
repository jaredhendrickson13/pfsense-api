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

class APIFirewallNATOutboundMappingUpdate extends APIModel {
    private $protocols;
    private $pool_options;
    private $created_by_msg;
    private $port_supported;
    private $pool_source_hash_supported;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified outbound NAT mapping via API";
        $this->privileges = ["page-all", "page-firewall-nat-outbound-edit"];
        $this->protocols = ["any", "tcp", "udp", "tcp/udp", "icmp", "esp", "ah", "gre", "ipv6", "igmp", "pim", "ospf"];
        $this->pool_options = ["", "round-robin", "round-robin sticky-address", "random", "random sticky-address", "source-hash", "bitmask"];
        $this->port_supported = false;
        $this->pool_source_hash_supported = false;
        $this->init_config('nat', 'outbound', 'rule');
    }

    public function action() {
        # Add our new configuration
        $this->config["nat"]["outbound"]["rule"][$this->id] = $this->validated_data;
        APITools\sort_nat_rules($this->initial_data["top"], $this->id, "outbound");
        $this->write_config();
        mark_subsystem_dirty('natconf');

        # Only reload the firewall filter if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            APIFirewallApplyCreate::apply();
        }
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Require clients to pass in a mapping ID to locate the mapping to update
        if (isset($this->initial_data['id'])) {
            // Check that our rule ID exists
            if (array_key_exists($this->initial_data['id'], $this->config["nat"]["outbound"]["rule"])) {
                $this->id = $this->initial_data['id'];
                $this->validated_data = $this->config["nat"]["outbound"]["rule"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(4105);
            }
        } else {
            $this->errors[] = APIResponse\get(4104);
        }
    }

    private function __validate_interface() {
        # Optionally allow client to update the interface
        if (isset($this->initial_data['interface'])) {
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Require interface to exist
            if ($interface) {
                $this->validated_data["interface"] = $interface;
            } else {
                $this->errors[] = APIResponse\get(4087);
            }
        }
    }

    private function __validate_protocol() {
        # Optionally allow client to update the protocol
        if (isset($this->initial_data['protocol'])) {
            # Require protocol to be a known/supported protocol
            if (in_array($this->initial_data['protocol'], $this->protocols)) {
                # Unset the protocol value if it is any (XML expects no entry for any). Otherwise update value.
                if ($this->initial_data["protocol"] === "any") {
                    unset($this->validated_data["protocol"]);
                } else {
                    $this->validated_data["protocol"] = $this->initial_data['protocol'];
                }
            } else {
                $this->errors[] = APIResponse\get(4089);
            }
        }
        # Set our port supported toggle to true if our protocol supports a port
        if (in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) {
            $this->port_supported = true;
        }
        # If our protocol is not a port-based protocol, remove any existing port values from the entry
        else {
            unset($this->validated_data["sourceport"]);
            unset($this->validated_data["dstport"]);
            unset($this->validated_data["natport"]);
        }
    }

    private function __validate_poolopts() {
        # Optionally allow clients to update poolopts
        if (isset($this->initial_data['poolopts'])) {
            # Require option to be a known/supported pool option
            if (in_array($this->initial_data['poolopts'], $this->pool_options)) {
                $this->validated_data["poolopts"] = $this->initial_data['poolopts'];
            } else {
                $this->errors[] = APIResponse\get(4090);
            }
        }
        # Set our hash key supported toggle to true if our pool option is source hash
        if ($this->validated_data["poolopts"] === "source-hash") {
            $this->pool_source_hash_supported = true;
        }
    }

    private function __validate_source_hash_key() {
        # Only allow client to update source hash key if it is the current pool option
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
            } elseif(empty($this->validated_data["source_hash_key"])) {
                $this->validated_data["source_hash_key"] = "0x".md5($this->initial_data['source_hash_key']);
            }
        }
    }

    private function __validate_target() {
        # Optionally allow client to update external target for the mapping
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
                # Only allow an alias if default or round-robin is chosen as the pool option
                if (in_array($this->validated_data["poolopts"], ["", "round-robin", "round-robin sticky-address"])) {
                    # Remove existing target IPs/subnets and add our alias as the target
                    unset($this->validated_data["targetip"]);
                    unset($this->validated_data["targetip_subnet"]);
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
        }
    }

    private function __validate_staticnatport() {
        # Optionally allow client to enable or disable static NAT port
        if ($this->port_supported and $this->initial_data["staticnatport"] === true) {
            # Set our NAT port toggle and remove any existing NAT port
            $this->validated_data["staticnatport"] = "";
            unset($this->validated_data["natport"]);
        } elseif($this->port_supported and $this->initial_data["staticnatport"] === false) {
            unset($this->validated_data["staticnatport"]);
        }
    }

    private function __validate_natport() {
        # Only allow updating a NAT port if the protocol supports a port
        if ($this->port_supported and !$this->validated_data["staticnatport"]) {
            # Optionally allow client to update the NAT port
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
        # Optionally allow client to update the mapping source
        if (isset($this->initial_data['src'])) {
            if ($this->initial_data['src'] === "any") {
                $this->validated_data['source']["network"] = "any";
            } elseif (APITools\is_ip_subnet_or_alias($this->initial_data["src"])) {
                $this->validated_data['source']["network"] = $this->initial_data["src"];
            } else {
                $this->errors[] = APIResponse\get(4099);
            }
        }
    }

    private function __validate_dst() {
        # Optionally allow client to update the mapping destination
        if (isset($this->initial_data['dst'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($rule_check["valid"]) {
                $this->validated_data["destination"] = $rule_check["data"]["destination"];
            } else {
                $this->errors[] = APIResponse\get(4101);
            }
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
        # Optionally allow client to enable or disable mapping upon entry
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        } elseif ($this->initial_data['disabled'] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    private function __validate_nonat() {
        # Optionally allow client to enable or disable NAT for traffic matching this entry
        if ($this->initial_data['nonat'] === true) {
            $this->validated_data["nonat"] = "";
        } elseif ($this->initial_data['nonat'] === false) {
            unset($this->validated_data["nonat"]);
        }
    }
    private function __validate_nosync() {
        # Optionally allow client to enable or disable pfSync for this entry
        if ($this->initial_data['nosync'] === true) {
            $this->validated_data["nosync"] = "";
        } elseif ($this->initial_data['nosync'] === false) {
            unset($this->validated_data["nosync"]);
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
        $this->__validate_id();
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
        $this->validated_data["updated"] = array("time" => time(), "username" => $this->created_by_msg);
    }
}
