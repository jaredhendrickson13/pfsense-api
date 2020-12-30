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

class APIFirewallRuleCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules-edit"];
        $this->change_note = "Added firewall rule via API";
    }

    public function action() {
        $next_rule_id = count($this->config["filter"]["rule"]);    // Save our next rule ID
        $this->config["filter"]["rule"][] = $this->validated_data;    // Write to our master config
        APITools\sort_firewall_rules($this->initial_data["top"], $next_rule_id);    // Sort our firewall rules
        $this->write_config();    // Apply our configuration change
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_type() {
        # Check for our required 'type' payload value
        if (isset($this->initial_data["type"])) {
            $type_options = array("pass", "block", "reject");
            # Ensure our type is a valid option
            if (in_array($this->initial_data["type"], $type_options)) {
                $this->validated_data["type"] = $this->initial_data["type"];
            } else {
                $this->errors[] = APIResponse\get(4039);
            }
        } else {
            $this->errors[] = APIResponse\get(4033);
        }
    }

    private function __validate_interface() {
        # Check for our required 'interface' payload value
        if (isset($this->initial_data['interface'])) {
            $this->initial_data['interface'] = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Check that we found the request pfSense interface ID
            if (is_string($this->initial_data["interface"])) {
                $this->validated_data['interface'] = $this->initial_data['interface'];
            } else {
                $this->errors[] = APIResponse\get(4034);
            }
        } else {
            $this->errors[] = APIResponse\get(4034);
        }
    }

    private function __validate_ipprotocol() {
        # Check for our required 'ipprotocol' payload value
        if (isset($this->initial_data['ipprotocol'])) {
            $ipprotocol_options = array("inet", "inet6", "inet46");
            # Check that our ipprotocol value is a support option
            if (in_array($this->initial_data["ipprotocol"], $ipprotocol_options)) {
                $this->validated_data["ipprotocol"] = $this->initial_data['ipprotocol'];
            } else {
                $this->errors[] = APIResponse\get(4041);
            }
        } else {
            $this->errors[] = APIResponse\get(4035);
        }
    }

    private function __validate_protocol() {
        # Check for our required 'protocol' payload value
        if (isset($this->initial_data['protocol'])) {
            $protocol_options = [
                "any", "tcp", "udp", "tcp/udp", "icmp", "esp", "ah",
                "gre", "ipv6", "igmp", "pim", "ospf", "carp", "pfsync"
            ];
            # Check that our protocol value is a support option
            if (in_array($this->initial_data["protocol"], $protocol_options)) {
                # Don't add a specific protocol if any
                if ($this->initial_data["protocol"] !== "any") {
                    $this->validated_data["protocol"] = $this->initial_data['protocol'];
                }
            } else {
                $this->errors[] = APIResponse\get(4042);
            }
        } else {
            $this->errors[] = APIResponse\get(4036);
        }
    }

    private function __validate_icmptype() {
        # Check for our optional 'icmpsubtype' payload value when our protocol is set to ICMP
        if (isset($this->initial_data["icmptype"]) and $this->validated_data["protocol"] === "icmp") {
            $icmptype_options = [
                "althost", "dataconv", "echorep", "echoreq", "inforep", "inforeq", "ipv6-here", "ipv6-where", "maskrep",
                "maskreq", "mobredir", "mobregrep", "mobregreq", "paramprob", "photuris", "redir", "routeradv",
                "routersol", "skip", "squench", "timerep", "timereq", "timex", "trace", "unreach"
            ];

            # Ensure icmptype value is an array
            if (!is_array($this->initial_data["icmptype"])) {
                $this->initial_data["icmptype"] = array($this->initial_data["icmptype"]);
            }

            // Loop through each of our subtypes
            foreach ($this->initial_data["icmptype"] as $ict) {
                if (!in_array($ict, $icmptype_options)) {
                    $this->errors[] = APIResponse\get(4046);
                }
                // Write our ICMP subtype config
                $this->validated_data["icmptype"] = implode(",", $this->initial_data["icmptype"]);
            }
        }
    }

    private function __validate_src() {
        # Check for our required 'src' payload value
        if (isset($this->initial_data['src'])) {
            // Check if our source and destination values are valid
            $dir_check = APITools\is_valid_rule_addr($this->initial_data['src'], "source");
            if ($dir_check["valid"] === true) {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            } else {
                $this->errors[] = APIResponse\get(4044);
            }
        } else {
            $this->errors[] = APIResponse\get(4037);
        }
    }

    private function __validate_dst() {
        # Check for our required 'src' payload value
        if (isset($this->initial_data['dst'])) {
            // Check if our source and destination values are valid
            $dir_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($dir_check["valid"] === true) {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            } else {
                $this->errors[] = APIResponse\get(4045);
            }
        } else {
            $this->errors[] = APIResponse\get(4038);
        }
    }

    private function __validate_srcport() {
        # Check for our required 'srcport' payload values if protocol is TCP, UDP or TCP/UDP
        if (in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) {
            if (isset($this->initial_data['srcport'])) {
                $val = str_replace("-", ":", $this->initial_data['srcport']);
                if (!is_port_or_range_or_alias($val) and $val !== "any") {
                    $this->errors[] = APIResponse\get(4048);
                } elseif ($val !== "any") {
                    $this->validated_data["source"]["port"] = str_replace(":", "-", $val);;
                }
            } else {
                $this->errors[] = APIResponse\get(4047);
            }
        }
    }

    private function __validate_dstport() {
        # Check for our required 'dstport' payload values if protocol is TCP, UDP or TCP/UDP
        if (in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) {
            if (isset($this->initial_data['dstport'])) {
                $val = str_replace("-", ":", $this->initial_data['dstport']);
                if (!is_port_or_range_or_alias($val) and $val !== "any") {
                    $this->errors[] = APIResponse\get(4049);
                } elseif ($val !== "any") {
                    $this->validated_data["destination"]["port"] = str_replace(":", "-", $val);;
                }
            } else {
                $this->errors[] = APIResponse\get(4047);
            }
        }
    }

    private function __validate_gateway() {
        # Check for our optional 'gateway' payload value
        if (isset($this->initial_data['gateway'])) {
            $gw_protocol = APITools\is_gateway($this->initial_data["gateway"], true);
            # Check if we were able to locate the gateway
            if ($gw_protocol === false) {
                $this->errors[] = APIResponse\get(4043);
            }
            # Check if the gateway IP protocol matches our rule's IP protocol
            elseif ($gw_protocol !== "inet46" and $gw_protocol !== $this->validated_data["ipprotocol"]) {
                $this->errors[] = APIResponse\get(4109);
            }
            # Otherwise, the gateway is valid
            else {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            }
        }
    }

    private function __validate_disabled() {
        # Check for our optional 'disabled' payload value
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        }
    }

    private function __validate_descr() {
        # Check for our optional 'descr' payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = strval($this->initial_data['descr']);
        }
    }

    private function __validate_log() {
        # Check for our optional 'log' payload value
        if ($this->initial_data['log'] === true) {
            $this->validated_data["log"] = "";
        }
    }

    private function __validate_top() {
        # Check for our optional 'top' payload value
        if ($this->initial_data['top'] === true) {
            $this->initial_data['top'] = "top";
        }
    }

    public function validate_payload() {
        $this->__validate_type();
        $this->__validate_interface();
        $this->__validate_ipprotocol();
        $this->__validate_protocol();
        $this->__validate_icmptype();
        $this->__validate_src();
        $this->__validate_dst();
        $this->__validate_srcport();
        $this->__validate_dstport();
        $this->__validate_gateway();
        $this->__validate_disabled();
        $this->__validate_descr();
        $this->__validate_log();
        $this->__validate_top();

        # Add our static 'tracker', 'created' and 'updated' values
        $this->validated_data["tracker"] = (int)microtime(true);
        $this->validated_data["created"] = [
            "time" => time(),
            "username" => $this->client->username."@".$this->client->ip_address." (API)"
        ];
        $this->validated_data["updated"] = $this->validated_data["created"];
    }

    public function apply() {
        # Mark the firewall subsystem as changed, this will be cleared if applied
        mark_subsystem_dirty('filter');

        # Optionally allow clients to apply this rule immediately if they passed in a true apply value
        if ($this->initial_data["apply"] === true) {
            filter_configure();
            clear_subsystem_dirty('filter');
        }
    }
}