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

class APIFirewallRuleUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules-edit"];
        $this->change_note = "Modified firewall rule via API";
    }

    public function action() {
        # Update our config item, sort rule filters and apply.
        $this->config["filter"]["rule"][$this->id] = $this->validated_data;
        APITools\sort_firewall_rules($this->initial_data["top"], $this->id);
        $this->write_config();
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Check for our required 'tracker' payload value
        if (isset($this->initial_data['tracker'])) {
            # Loop through each rule and check if our tracker matches
            foreach ($this->config["filter"]["rule"] as $id=>$rule) {
                if (intval($rule["tracker"]) === intval($this->initial_data['tracker'])) {
                    $this->validated_data = $rule;
                    $this->id = $id;
                    break;
                }
            }
            # Return not found error if no match was found
            if (!is_int($this->id)) {
                $this->errors[] = APIResponse\get(4032);
            }
        } else {
            $this->errors[] = APIResponse\get(4031);
        }

        # Check for our optional 'type' payload value
        if (isset($this->initial_data["type"])) {
            $type_options = array("pass", "block", "reject");
            # Ensure our type is a valid option
            if (in_array($this->initial_data["type"], $type_options)) {
                $this->validated_data["type"] = $this->initial_data["type"];
            } else {
                $this->errors[] = APIResponse\get(4039);
            }
        }

        # Check for our optional 'interface' payload value
        if (isset($this->initial_data['interface'])) {
            $this->initial_data['interface'] = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Check that we found the request pfSense interface ID
            if (is_string($this->initial_data["interface"])) {
                $this->validated_data['interface'] = $this->initial_data['interface'];
            } else {
                $this->errors[] = APIResponse\get(4034);
            }
        }

        # Check for our optional 'ipprotocol' payload value
        if (isset($this->initial_data['ipprotocol'])) {
            $ipprotocol_options = array("inet", "inet6", "inet46");
            # Check that our ipprotocol value is a support option
            if (in_array($this->initial_data["ipprotocol"], $ipprotocol_options)) {
                $this->validated_data["ipprotocol"] = $this->initial_data['ipprotocol'];
            } else {
                $this->errors[] = APIResponse\get(4041);
            }
        }

        # Check for our optional 'protocol' payload value
        if (isset($this->initial_data['protocol'])) {
            $missing_port = (!in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) ? true : false;
            $requires_port = (in_array($this->initial_data["protocol"], ["tcp", "udp", "tcp/udp"])) ? true : false;
            $this->unset_ports();
            $protocol_options = [
                "any", "tcp", "udp", "tcp/udp", "icmp", "esp", "ah",
                "gre", "ipv6", "igmp", "pim", "ospf", "carp", "pfsync"
            ];

            # Check that our protocol value is a support option
            if (in_array($this->initial_data["protocol"], $protocol_options)) {
                # Don't add a specific protocol if any
                if ($this->initial_data["protocol"] !== "any") {
                    $this->validated_data["protocol"] = $this->initial_data['protocol'];
                } else {
                    unset($this->validated_data["protocol"]);
                }
            } else {
                $this->errors[] = APIResponse\get(4042);
            }
        } else {
            # If a new protocol was not selected don't validate ports
            $requires_port = false;
            $missing_port = false;
        }

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

        # Check for our optional 'src' payload value
        if (isset($this->initial_data['src'])) {
            // Check if our source and destination values are valid
            $dir_check = APITools\is_valid_rule_addr($this->initial_data['src'], "source");
            if ($dir_check["valid"] === true) {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            } else {
                $this->errors[] = APIResponse\get(4044);
            }
        }

        # Check for our optional 'src' payload value
        if (isset($this->initial_data['dst'])) {
            // Check if our source and destination values are valid
            $dir_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($dir_check["valid"] === true) {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            } else {
                $this->errors[] = APIResponse\get(4049);
            }
        }

        # Check for our optional 'srcport' and 'dstport' payload values if protocol is TCP, UDP or TCP/UDP
        if ($requires_port) {
            if (isset($this->initial_data['srcport'])) {
                $val = str_replace("-", ":", $this->initial_data['srcport']);
                if (!is_port_or_range_or_alias($val) and $val !== "any") {
                    $this->errors[] = APIResponse\get(4048);
                } elseif ($val !== "any") {
                    $this->validated_data["source"]["port"] = str_replace(":", "-", $val);;
                }
            } elseif ($missing_port) {
                $this->errors[] = APIResponse\get(4047);
            }

            if (isset($this->initial_data['dstport'])) {
                $val = str_replace("-", ":", $this->initial_data['dstport']);
                if (!is_port_or_range_or_alias($val) and $val !== "any") {
                    $this->errors[] = APIResponse\get(4048);
                } elseif ($val !== "any") {
                    $this->validated_data["destination"]["port"] = str_replace(":", "-", $val);;
                }
            } elseif ($missing_port) {
                $this->errors[] = APIResponse\get(4047);
            }
        }

        # Check for our optional 'gateway' payload value
        if (isset($this->initial_data['gateway'])) {
            # Check that the specified gateway exists
            if (APITools\is_gateway($this->initial_data["gateway"])) {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            } else {
                $this->errors[] = APIResponse\get(4043);
            }
        }

        # Check for our optional 'disabled' payload value
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        } elseif ($this->initial_data['disabled'] === false) {
            unset($this->validated_data["disabled"]);
        }

        # Check for our optional 'descr' payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = strval($this->initial_data['descr']);
        }

        # Check for our optional 'log' payload value
        if ($this->initial_data['log'] === true) {
            $this->validated_data["log"] = "";
        }

        # Check for our optional 'top' payload value
        if ($this->initial_data['top'] === true) {
            $this->initial_data['top'] = "top";
        }

        # Update our 'updated' value
        $this->validated_data["updated"] = [
            "time" => time(),
            "username" => $this->client->username."@".$this->client->ip_address." (API)"
        ];
    }

    # Removes port specifications when updating to a protocol that is not port based
    private function unset_ports() {
        # If a new protocol was chosen that doesn't require a port, remove existing ports from the rule
        if ((!in_array($this->initial_data["protocol"], ["tcp", "udp", "tcp/udp"]))) {
            unset($this->validated_data["source"]["port"]);
            unset($this->validated_data["destination"]["port"]);
        }
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