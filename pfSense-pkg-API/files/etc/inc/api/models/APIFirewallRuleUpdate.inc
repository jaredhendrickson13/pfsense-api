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

class APIFirewallRuleUpdate extends APIModel {
    private $requires_port;
    private $missing_port;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules-edit"];
        $this->change_note = "Modified firewall rule via API";
        $this->requires_port = false;
        $this->missing_port = false;
    }

    public function action() {
        # Update our config item, sort rule filters and apply.
        $this->config["filter"]["rule"][$this->id] = $this->validated_data;
        APITools\sort_firewall_rules($this->initial_data["top"], $this->id);
        $this->write_config();
        mark_subsystem_dirty('filter');

        # Only reload the firewall filter if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            APIFirewallApplyCreate::apply();
        }
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_tracker() {
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
    }

    private function __validate_type() {
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
    }

    private function __validate_interface() {
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
    }

    private function __validate_ipprotocol() {
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
    }

    private function __validate_protocol() {
        # Check for our optional 'protocol' payload value
        if (isset($this->initial_data['protocol'])) {
            $this->missing_port = (!in_array($this->validated_data["protocol"], ["tcp", "udp", "tcp/udp"])) ? true : false;
            $this->requires_port = (in_array($this->initial_data["protocol"], ["tcp", "udp", "tcp/udp"])) ? true : false;
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

            # Loop through each of our subtypes
            foreach ($this->initial_data["icmptype"] as $ict) {
                if (!in_array($ict, $icmptype_options)) {
                    $this->errors[] = APIResponse\get(4046);
                }
                # Write our ICMP subtype config
                $this->validated_data["icmptype"] = implode(",", $this->initial_data["icmptype"]);
            }
        }
    }

    private function __validate_src() {
        # Check for our optional 'src' payload value
        if (isset($this->initial_data['src'])) {
            # Check if our source and destination values are valid
            $dir_check = APITools\is_valid_rule_addr($this->initial_data['src'], "source");
            if ($dir_check["valid"] === true) {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            } else {
                $this->errors[] = APIResponse\get(4044);
            }
        }
    }

    private function __validate_dst() {
        # Check for our optional 'dst' payload value
        if (isset($this->initial_data['dst'])) {
            # Check if our source and destination values are valid
            $dir_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($dir_check["valid"] === true) {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            } else {
                $this->errors[] = APIResponse\get(4045);
            }
        }
    }

    private function __validate_srcport() {
        # Check for our optional 'srcport' payload values if protocol is TCP, UDP or TCP/UDP
        if ($this->requires_port) {
            if (isset($this->initial_data['srcport'])) {
                $val = str_replace("-", ":", $this->initial_data['srcport']);
                if (!is_port_or_range_or_alias($val) and $val !== "any") {
                    $this->errors[] = APIResponse\get(4048);
                } elseif ($val !== "any") {
                    $this->validated_data["source"]["port"] = str_replace(":", "-", $val);;
                }
            } elseif ($this->missing_port) {
                $this->errors[] = APIResponse\get(4047);
            }
        }
    }

    private function __validate_dstport() {
        # Check for our optional 'dstport' payload values if protocol is TCP, UDP or TCP/UDP
        if ($this->requires_port) {
            if (isset($this->initial_data['dstport'])) {
                $val = str_replace("-", ":", $this->initial_data['dstport']);
                if (!is_port_or_range_or_alias($val) and $val !== "any") {
                    $this->errors[] = APIResponse\get(4049);
                } elseif ($val !== "any") {
                    $this->validated_data["destination"]["port"] = str_replace(":", "-", $val);;
                }
            } elseif ($this->missing_port) {
                $this->errors[] = APIResponse\get(4047);
            }
        }
    }

    private function __validate_gateway() {
        # Check for our optional 'gateway' payload value. If already set, revalidate.
        if (isset($this->initial_data['gateway']) or isset($this->validated_data['gateway'])) {
            # Revalidate the existing value if a new one was not specified
            if (!isset($this->initial_data["gateway"])) {
                $this->initial_data["gateway"] = $this->validated_data["gateway"];
            }
            $gw_protocol = APITools\is_gateway($this->initial_data["gateway"], true);
            # Check if we were able to locate the gateway
            if ($gw_protocol === false) {
                $this->errors[] = APIResponse\get(4043);
            }
            # Check if the gateway IP protocol matches our rule's IP protocol
            elseif ($gw_protocol !== $this->validated_data["ipprotocol"]) {
                $this->errors[] = APIResponse\get(4109);
            }
            # Otherwise, the gateway is valid
            else {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            }
        }
    }

    private function __validate_sched() {
        # Check for our optional 'sched' payload value
        if (isset($this->initial_data['sched'])) {
            # Only try to set the schedule if the value is not empty
            if (!empty($this->initial_data["sched"])) {
                # Loop through each configured schedule and check if it exists
                foreach ($this->config["schedules"]["schedule"] as $schedule) {
                    # Check if the names match
                    if ($this->initial_data["sched"] === $schedule["name"]) {
                        $this->validated_data["sched"] = $this->initial_data["sched"];
                    }
                }

                # Set error if no match was found
                if (!isset($this->validated_data["sched"])) {
                    $this->errors[] = APIResponse\get(4150);
                }
            }
            # Unset the schedule from the rule if an empty value was provided
            else {
                unset($this->validated_data["sched"]);
            }
        }
    }

    private function __validate_dnpipe() {
        # Check for our optional 'dnpipe' payload value
        if (isset($this->initial_data["dnpipe"])) {
            # Only proceed if value is not empty
            if (!empty($this->initial_data["dnpipe"])) {
                # Load our dummynet config and fetch configured queues
                read_dummynet_config();
                $dnqueues = get_unique_dnqueue_list();

                # Ensure this is a valid queue
                if (array_key_exists($this->initial_data["dnpipe"], $dnqueues)) {
                    $this->validated_data["dnpipe"] = $this->initial_data["dnpipe"];
                } else {
                    $this->errors[] = APIResponse\get(4222);
                }
            } else {
                unset($this->validated_data["dnpipe"]);
                unset($this->validated_data["pdnpipe"]);
            }
        }
    }

    private function __validate_pdnpipe() {
        # Check for our optional 'pdnpipe' payload value
        if (isset($this->initial_data["pdnpipe"])) {
            # Only proceed if value is not empty
            if (!empty($this->initial_data["pdnpipe"])) {
                # Load our dummynet config and fetch configured queues
                read_dummynet_config();
                $dnqueues = get_unique_dnqueue_list();

                # Only allow this setting if a dnpipe was set
                if (isset($this->validated_data["dnpipe"])) {
                    # Ensure this is a valid queue
                    if (array_key_exists($this->initial_data["pdnpipe"], $dnqueues)) {
                        # Ensure the dnpipe does not match the pdn pipe
                        if ($this->initial_data["pdnpipe"] !== $this->validated_data["dnpipe"]) {
                            # Determine pipe types
                            $dnpipe_type = $dnqueues[$this->validated_data['dnpipe']][0];
                            $pdnpipe_type = $dnqueues[$this->initial_data['pdnpipe']][0];

                            # Ensure the dnpipe and pdnpipe types match
                            if (($dnpipe_type === "?" and $pdnpipe_type !== "?") or ($pdnpipe_type === "?" and $dnpipe_type !== "?")) {
                                $this->errors[] = APIResponse\get(4225);
                            } else {
                                $this->validated_data["pdnpipe"] = $this->initial_data["pdnpipe"];
                            }
                        } else {
                            $this->errors[] = APIResponse\get(4224);
                        }
                    } else {
                        $this->errors[] = APIResponse\get(4222);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4223);
                }
            } else {
                unset($this->validated_data["pdnpipe"]);
            }
        }
    }

    private function __validate_defaultqueue() {
        # Validate our optional 'defaultqueue' payload value
        if (isset($this->initial_data["defaultqueue"])) {
            # Only proceed if value is not empty
            if (!empty($this->initial_data["defaultqueue"])) {
                # Require the value to be a known altq (traffic shaper) queue
                if ($this->is_altq_queue($this->initial_data["defaultqueue"])) {
                    $this->validated_data["defaultqueue"] = $this->initial_data["defaultqueue"];
                } else {
                    $this->errors[] = APIResponse\get(4226);
                }
            } else {
                unset($this->validated_data["defaultqueue"]);
                unset($this->validated_data["ackqueue"]);
            }
        }
    }

    private function __validate_ackqueue() {
        # Validate our optional 'ackqueue' payload value
        if (isset($this->initial_data["ackqueue"])) {
            # Only proceed if value is not empty
            if (!empty($this->initial_data["ackqueue"])) {
                # Only allow this field if a default queue was provided
                if (isset($this->validated_data["defaultqueue"])) {
                    # Require the value to be a known altq (traffic shaper) queue
                    if ($this->is_altq_queue($this->initial_data["ackqueue"])) {
                        # Ackqueue cannot match default queue
                        if ($this->initial_data["ackqueue"] !== $this->validated_data["defaultqueue"]) {
                            $this->validated_data["ackqueue"] = $this->initial_data["ackqueue"];
                        } else {
                            $this->errors[] = APIResponse\get(4229);
                        }
                    } else {
                        $this->errors[] = APIResponse\get(4228);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4227);
                }
            } else {
                unset($this->validated_data["ackqueue"]);
            }
        }
    }

    private function __validate_disabled() {
        # Check for our optional 'disabled' payload value
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        } elseif ($this->initial_data['disabled'] === false) {
            unset($this->validated_data["disabled"]);
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
        } elseif ($this->initial_data['log'] === false) {
            unset($this->validated_data["log"]);
        }
    }

    private function __validate_top() {
        # Check for our optional 'top' payload value
        if ($this->initial_data['top'] === true) {
            $this->initial_data['top'] = "top";
        }
    }

    private function __validate_floating() {
        # Check for floating rule
        if ($this->initial_data["floating"] === true) {
            $this->validated_data["floating"] = "yes";
        } elseif ($this->initial_data["floating"] === false) {
            unset($this->validated_data["floating"]);
        }
    }

    private function __validate_direction() {
        # Validate our optional 'floating' payload value
        #if (empty($this->initial_data["floating"])) {
        #   if (isset($this->initial_data["direction"])) {
        #        if (in_array($this->initial_data["direction"], ["in", "out", "any"])) {
        #            $this->validated_data["direction"] = $this->initial_data["direction"];
        #        } else {
        #            $this->errors[] = APIResponse\get(6030);
        #        }
        #    }
        #}
        if ($this->initial_data["floating"] === true) {
            if (isset($this->initial_data["direction"])) {
                if (in_array($this->initial_data["direction"], ["in", "out", "any"])) {
                    $this->validated_data["direction"] = $this->initial_data["direction"];
                } else {
                    $this->errors[] = APIResponse\get(6030);
                }
            }
        } else {
            unset($this->validated_data["direction"]);
        }
    }

    public function validate_payload() {
        $this->__validate_tracker();
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
        $this->__validate_sched();
        $this->__validate_dnpipe();
        $this->__validate_pdnpipe();
        $this->__validate_defaultqueue();
        $this->__validate_ackqueue();
        $this->__validate_disabled();
        $this->__validate_descr();
        $this->__validate_log();
        $this->__validate_top();
        $this->__validate_floating();
        $this->__validate_direction();

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

    public function is_altq_queue($name) {
        # Local variables
        read_altq_config();
        $qlist = get_unique_queue_list();
        $iflist = get_configured_interface_with_descr();
        $list = [];

        # Format interface queues
        foreach ($qlist as $q => $qkey) {
            if (isset($ifdisp[$q])) {
                $list[$q] = $iflist[$q];
            } else {
                $list[$q] = $q;
            }
        }
        return array_key_exists($name, $list);
    }
}
