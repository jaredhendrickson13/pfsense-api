<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIInterfacesAdd extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-interfaces-assignnetworkports"];
        $this->change_note = "Added interface via API";

    }

    public function action() {
        $this->config["interfaces"] = array_merge($this->config["interfaces"], $this->validated_data);
        $this->write_config();
        $apply_if = APITools\apply_interface_config($this->validated_data);
        if ($apply_if) {
            return APIResponse\get(0, $this->validated_data);
        } else {
            return APIResponse\get(1);
        }
    }

    public function validate_payload() {
        $if_list = APITools\get_all_avail_interfaces();    // Save all our available interfaces
        $allowed_ip4_types = array("staticv4", "dhcp");    // List of allowed IPv4 if types
        $allowed_ip6_types = array("staticv6", "dhcp6", "slaac", "6rd", "6to4", "track6");    // List of allowed IPv6 if types
        $next_if = APITools\get_next_pfsense_if_id();
        $this->validated_data = array($next_if => []);
        if (isset($this->initial_data["if"])) {
            $interface = trim($this->initial_data["if"]);
            // Check that our interface exists and is not in use
            if (!array_key_exists($interface, $if_list)) {
                $this->errors[] = APIResponse\get(3000);
            } elseif (isset($if_list[$interface]["in_use"])) {
                $this->errors[] = APIResponse\get(3001);
            }
            $this->validated_data[$next_if]["if"] = $interface;
        } else {
            $this->errors[] = APIResponse\get(3002);
        }
        // Check for our enable value
        if (isset($this->initial_data["enable"])) {
            $enable = true;
            $this->validated_data[$next_if]["enable"] = "";
        }
        // Check for our MAC address value
        if (isset($this->initial_data["spoofmac"])) {
            $mac_addr = $this->initial_data["spoofmac"];
            // Check if mac addr is valid
            if (is_macaddr($mac_addr)) {
                $this->validated_data[$next_if]["spoofmac"] = $mac_addr;
            } else {
                $this->errors[] = APIResponse\get(3003);
            }
        }
        // Check for our MTU value
        if (isset($this->initial_data["mtu"])) {
            $mtu = intval($this->initial_data["mtu"]);
            // Check if MTU is within range
            if (1280 > $mtu or $mtu > 8192) {
                $this->errors[] = APIResponse\get(3004);
            } elseif ($if_list[$interface]["is_vlan"]) {
                // Check if interface is VLAN and that it's MTU is lower than it's parent interface
                $parent_if = $if_list[$interface]["if"];
                if ($mtu > $parent_if["mtu"]) {
                    $this->errors[] = APIResponse\get(3006);
                }
            } else {
                $this->validated_data[$next_if]["mtu"] = $mtu;
            }
        }
        // Check for our MSS value
        if (isset($this->initial_data["mss"])) {
            $mss = intval($this->initial_data["mss"]);
            // Check if MSS is within range
            if (576 > $mss or $mss > 65535) {
                $this->errors[] = APIResponse\get(3005);

            } else {
                $this->validated_data[$next_if]["mss"] = $mss;
            }
        }
        // Check for our SPEED/DUPLEX value
        if (isset($this->initial_data["media"])) {
            $media = $this->initial_data["media"];
            $avail_media = APITools\get_if_media_options($interface, true);
            // Loop each of our media options and see if our input matches
            foreach ($avail_media as $mopt) {
                if ($media === $mopt) {
                    $media_found = true;
                    $mopt_list = explode(" ", $mopt);
                    $this->validated_data[$next_if]["media"] = $mopt_list[0];
                    $this->validated_data[$next_if]["mediaopt"] = $mopt_list[1];
                    break;
                }
            }
            // If we did not find a match return error
            if (!$media_found) {
                $this->errors[] = APIResponse\get(3007);
            }
        }
        // Check for our description value
        if (isset($this->initial_data["descr"])) {
            $descr = APITools\sanitize_str($this->initial_data["descr"]);
            // Check that is interface descriptive name does not alrady exist
            if (!APITools\get_pfsense_if_id($descr)) {
                $this->validated_data[$next_if]["descr"] = $descr;
            } else {
                $this->errors[] = APIResponse\get(3008);
            }
        } else {
            $descr = strtoupper($next_if);
            $this->validated_data[$next_if]["descr"] = $descr;
        }
        // Check for our block private IP value
        if (isset($this->initial_data["blockpriv"])) {
            $block_priv = true;
            $this->validated_data[$next_if]["blockpriv"] = "";
        }
        // Check for our block private IP value
        if (isset($this->initial_data["blockbogons"])) {
            $block_bogons = true;
            $this->validated_data[$next_if]["blockbogons"] = "";
        }
        // Check if we have an IPv4 configuration
        if (isset($this->initial_data["type"])) {
            $type = $this->initial_data["type"];
            // Check if our IPv4 config type is allowed
            if (in_array($type, $allowed_ip4_types)) {
                // Gather input for our various IPv4 interface configuration types
                // IPv4 STATICV4 TYPE
                if ($type === "staticv4") {
                    // Check if our IP is set
                    if (isset($this->initial_data["ipaddr"])) {
                        $ipaddr = $this->initial_data["ipaddr"];
                        // Check if IP address is valid
                        if (!is_ipaddrv4($ipaddr)) {
                            $this->errors[] = APIResponse\get(3010);
                        } elseif (APITools\is_ip_in_use($ipaddr)) {
                            $this->errors[] = APIResponse\get(3009);
                        } else {
                            $this->validated_data[$next_if]["ipaddr"] = $ipaddr;
                        }
                    } else {
                        $this->errors[] = APIResponse\get(3011);
                    }
                    // Check if our subnet is valid
                    if (isset($this->initial_data["subnet"])) {
                        $subnet = strval($this->initial_data["subnet"]);
                        // Check if our subnet is within range
                        if (!is_subnet($ipaddr."/".$subnet)) {
                            $this->errors[] = APIResponse\get(3012);
                        } else {
                            $this->validated_data[$next_if]["subnet"] = $subnet;
                        }
                    } else {
                        // Update our message if we did not already encounter an error
                        $this->errors[] = APIResponse\get(3013);

                    }
                    // Check if user specified a network gateway, if so check if it's valid
                    if (isset($this->initial_data["gateway"])) {
                        $gateway = $this->initial_data["gateway"];
                        // Check if this gateway exists
                        if (!APITools\is_gateway($gateway)) {
                            $this->errors[] = APIResponse\get(3014);
                        } else {
                            $this->validated_data[$next_if]["gateway"] = $gateway;
                        }
                    }
                    // IPv4 DHCP TYPE
                } elseif ($type === "dhcp") {
                    $this->validated_data[$next_if]["ipaddr"] = $type;    // Set our ipaddr value to dhcp
                    // Check if we have a dhcphostname value
                    if (isset($this->initial_data["dhcphostname"])) {
                        $this->validated_data[$next_if]["dhcphostname"] = strval($this->initial_data["dhcphostname"]);
                    }
                    // Check if we have a alias-address value
                    if (isset($this->initial_data["alias-address"])) {
                        if (is_ipaddrv4($this->initial_data["alias-address"])) {
                            $this->validated_data[$next_if]["alias-address"] = strval($this->initial_data["alias-address"]);
                            if (isset($this->initial_data["alias-subnet"])) {
                                $dhcpaliasnet = str($this->initial_data["alias-subnet"]);
                                if (is_subnet($this->validated_data[$next_if]["alias-address"]."/".$dhcpaliasnet)) {
                                    $this->validated_data[$next_if]["alias-subnet"] = $dhcpaliasnet;
                                }
                            } else {
                                $this->validated_data[$next_if]["alias-subnet"] = 32;
                            }
                        } else {
                            $this->errors[] = APIResponse\get(3015);
                        }
                    }
                    // Check if we have a dhcprejectfrom value
                    if (isset($this->initial_data["dhcprejectfrom"])) {
                        $dhcpreject = $this->initial_data["dhcprejectfrom"];
                        // Check what data type was passed in
                        if (is_string($dhcpreject)) {
                            $dhcprejectlist = explode(",", $dhcpreject);
                            // Loop through our reject list and ensure values are valid
                            foreach ($dhcprejectlist as $ra) {
                                if (!is_ipaddrv4($ra)) {
                                    $bad_reject = true;
                                    break;
                                }
                            }
                        } elseif (is_array($dhcpreject)) {
                            // Loop through our reject list and ensure values are valid
                            foreach ($dhcpreject as $ra) {
                                if (!is_ipaddrv4($ra)) {
                                    $bad_reject = true;
                                    break;
                                }
                            }
                            // Convert our list to comma separated string
                            $dhcpreject = implode(",", $dhcpreject);
                        }
                        // Check for bad IPs
                        if ($bad_reject) {
                            $this->errors[] = APIResponse\get(3016);
                        } else {
                            $this->validated_data[$next_if]["dhcprejectfrom"] = $dhcpreject;
                        }
                    }
                    // Check for our DHCP protocol timing
                    $timing_protocols = array(
                        "adv_dhcp_pt_timeout" => ["keyword" => "timeout", "return" => 134, "min" => 1],
                        "adv_dhcp_pt_retry" => ["keyword" => "retry", "return" => 135, "min" => 1],
                        "adv_dhcp_pt_select_timeout" => ["keyword" => "select timeout", "return" => 136, "min" => 0],
                        "adv_dhcp_pt_reboot" => ["keyword" => "reboot", "return" => 137, "min" => 1],
                        "adv_dhcp_pt_backoff_cutoff" => ["keyword" => "backoff cutoff", "return" => 138, "min" => 1],
                        "adv_dhcp_pt_initial_interval" => ["keyword" => "initial interval", "return" => 139, "min" => 1],
                    );
                    // Loop through each timing attribute and see if it's valid
                    foreach ($timing_protocols as $tp => $data) {
                        if (isset($this->initial_data[$tp])) {
                            // Check that value is in range
                            $dhcp_attr = intval($this->initial_data[$tp]);
                            if ($dhcp_attr >= $data["min"]) {
                                $this->validated_data[$next_if][$tp] = $dhcp_attr;
                                $this->validated_data[$next_if]["adv_dhcp_pt_values"] = "SavedCfg";
                            } else {
                                if ($data["keyword"] === "timeout") {
                                    $this->errors[] = APIResponse\get(3017);
                                } elseif ($data["keyword"] === "retry") {
                                    $this->errors[] = APIResponse\get(3018);
                                } elseif ($data["keyword"] === "select timeout") {
                                    $this->errors[] = APIResponse\get(3019);
                                } elseif ($data["keyword"] === "reboot") {
                                    $this->errors[] = APIResponse\get(3020);
                                } elseif ($data["keyword"] === "backoff cutoff") {
                                    $this->errors[] = APIResponse\get(3021);
                                } elseif ($data["keyword"] === "initial interval") {
                                    $this->errors[] = APIResponse\get(3022);
                                }
                            }
                        }
                    }
                    // Check for advance DHCP config
                    if (isset($this->initial_data["adv_dhcp_config_advanced"])) {
                        $this->validated_data[$next_if]["adv_dhcp_config_advanced"] = "yes";
                        // Check for our DHCP options
                        $dhcp_opts = array(
                            "adv_dhcp_send_options",
                            "adv_dhcp_request_options",
                            "adv_dhcp_required_options",
                            "adv_dhcp_option_modifiers"
                        );
                        foreach ($dhcp_opts as $do) {
                            // Check if option exists
                            if (isset($this->initial_data[$do])) {
                                $this->validated_data[$next_if][$do] = strval($this->initial_data[$do]);
                            }
                        }
                    }
                    // Check for DHCP configuration file override option
                    if (isset($this->initial_data["adv_dhcp_config_file_override"])) {
                        $this->validated_data[$next_if]["adv_dhcp_config_file_override"] = "";
                        // Check if a file path was given
                        if (isset($this->initial_data["adv_dhcp_config_file_override_path"])) {
                            $dhcp_conf_file = $this->initial_data["adv_dhcp_config_file_override_path"];
                            // Check that our conf file exists
                            if (is_file($dhcp_conf_file)) {
                                $this->validated_data[$next_if]["adv_dhcp_config_file_override"] = $dhcp_conf_file;
                            } else {
                                $this->errors[] = APIResponse\get(3023);
                            }
                        }
                    }
                    // Check for DHCP VLAN priority
                    $dhcp_vlan_prios = array(
                        0 => "bk",
                        1 => "be",
                        2 => "ee",
                        3 => "ca",
                        4 => "vi",
                        5 => "vo",
                        6 => "ic",
                        7 => "nc"
                    );
                    if (isset($this->initial_data["dhcpvlanenable"])) {
                        $this->validated_data[$next_if]["dhcpvlanenable"] = "";
                        if (isset($this->initial_data["dhcpcvpt"])) {
                            $vlan_prio = strtolower($this->initial_data["dhcpcvpt"]);
                            // Check if VLAN priority was provided as number
                            if (is_numeric($vlan_prio) and array_key_exists(intval($vlan_prio), $dhcp_vlan_prios)) {
                                $this->validated_data[$next_if]["dhcpcvpt"] = $dhcp_vlan_prios[intval($vlan_prio)];
                            } else {
                                // Loop through our priorities and see if value matches
                                foreach ($dhcp_vlan_prios as $dvp => $dvpval) {
                                    if ($vlan_prio === $dvpval) {
                                        $vlan_prio_found = true;
                                        $this->validated_data[$next_if]["dhcpcvpt"] = $dvpval;
                                        break;
                                    }
                                }
                                // Check that we found a value in our loop
                                if (!$vlan_prio_found) {
                                    $this->errors[] = APIResponse\get(3024);
                                }
                            }
                        }
                    }
                }
            } else {
                $this->errors[] = APIResponse\get(3025);
            }
        }
        // Check if we have an IPv6 configuration
        if (isset($this->initial_data["type6"])) {
            $type6 = $this->initial_data["type6"];
            // Check if our IPv6 config type is allowed
            if (in_array($type6, $allowed_ip6_types)) {
                // Gather input for our various IPv6 interface configuration types
                // IPv6 STATICV6 TYPE
                if ($type6 === "staticv6") {
                    // Check if our IP is set
                    if (isset($this->initial_data["ipaddrv6"])) {
                        $ipaddrv6 = $this->initial_data["ipaddrv6"];
                        // Check if IP address is valid
                        if (!is_ipaddrv6($ipaddrv6)) {
                            $this->errors[] = APIResponse\get(3026);
                        } elseif (APITools\is_ip_in_use($ipaddrv6)) {
                            $this->errors[] = APIResponse\get(3027);
                        } else {
                            $this->validated_data[$next_if]["ipaddrv6"] = $ipaddrv6;
                        }
                    } else {
                        $this->errors[] = APIResponse\get(3028);
                    }
                    // Check if our subnet is valid
                    if (isset($this->initial_data["subnetv6"])) {
                        $subnetv6 = strval($this->initial_data["subnetv6"]);
                        // Check if our subnet is within range
                        if (!is_subnet($ipaddrv6 . "/" . $subnetv6)) {
                            $this->errors[] = APIResponse\get(3029);
                        } else {
                            $this->validated_data[$next_if]["subnetv6"] = $subnetv6;
                        }
                    } else {
                        $this->errors[] = APIResponse\get(3030);
                    }
                    // Check if user specified a network gateway, if so check if it's valid
                    if (isset($this->initial_data["gatewayv6"])) {
                        $gatewayv6 = $this->initial_data["gatewayv6"];
                        // Check if this gateway exists
                        if (!APITools\is_gateway($gatewayv6)) {
                            $this->errors[] = APIResponse\get(3031);
                        } else {
                            $this->validated_data[$next_if]["gatewayv6"] = $gatewayv6;
                        }
                    }
                    // Check if user set ipv6usev4iface value
                    if (isset($this->initial_data["ipv6usev4iface"])) {
                        $this->validated_data[$next_if]["ipv6usev4iface"] = "";
                    }
                    // IPv6 DHCP6 TYPE
                } elseif ($type6 === "dhcp6") {
                    $this->validated_data[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to dhcp6
                    // Check if user set ipv6usev4iface value
                    if (isset($this->initial_data["ipv6usev4iface"])) {
                        $this->validated_data[$next_if]["ipv6usev4iface"] = "";
                    }
                    // Check if user set dhcp6prefixonly value
                    if (isset($this->initial_data["dhcp6prefixonly"])) {
                        $this->validated_data[$next_if]["dhcp6prefixonly"] = "";
                    }
                    // Check if user set dhcp6-ia-pd-send-hint value
                    if (isset($this->initial_data["dhcp6-ia-pd-send-hint"])) {
                        $this->validated_data[$next_if]["dhcp6-ia-pd-send-hint"] = "";
                    }
                    // Check if user set dhcp6debug value
                    if (isset($this->initial_data["dhcp6debug"])) {
                        $this->validated_data[$next_if]["dhcp6debug"] = "";
                    }
                    // Check if user set dhcp6withoutra value
                    if (isset($this->initial_data["dhcp6withoutra"])) {
                        $this->validated_data[$next_if]["dhcp6withoutra"] = "";
                    }
                    // Check if user set dhcp6norelease value
                    if (isset($this->initial_data["dhcp6norelease"])) {
                        $this->validated_data[$next_if]["dhcp6norelease"] = "";
                    }
                    // Check if user set dhcp6vlanenable value
                    if (isset($this->initial_data["dhcp6vlanenable"])) {
                        $this->validated_data[$next_if]["dhcp6vlanenable"] = "";
                    }
                    // Check if user set dhcp6-ia-pd-len value
                    if (isset($this->initial_data["dhcp6-ia-pd-len"])) {
                        // Set array of allowed prefix delegation sizes and their config translation
                        $dhcp6_del_size = intval($this->initial_data["dhcp6-ia-pd-len"]);
                        $allowed_size = array(
                            64 => 0,
                            63 => 1,
                            62 => 2,
                            61 => 3,
                            60 => 4,
                            59 => 5,
                            56 => 8,
                            52 => 12,
                            48 => 16
                        );
                        if (array_key_exists($dhcp6_del_size, $allowed_size)) {
                            $this->validated_data[$next_if]["dhcp6-ia-pd-len"] = $allowed_size[$dhcp6_del_size];
                        } else {
                            $this->errors[] = APIResponse\get(3032);
                        }
                    }
                    // Check for DHCP VLAN priority
                    $dhcp_vlan_prios = array(
                        0 => "bk",
                        1 => "be",
                        2 => "ee",
                        3 => "ca",
                        4 => "vi",
                        5 => "vo",
                        6 => "ic",
                        7 => "nc"
                    );
                    if (isset($this->initial_data["dhcp6vlanenable"])) {
                        $this->validated_data[$next_if]["dhcp6vlanenable"] = "";
                        if (isset($this->initial_data["dhcp6cvpt"])) {
                            $vlan_prio = strtolower($this->initial_data["dhcp6cvpt"]);
                            // Check if VLAN priority was provided as number
                            if (is_numeric($vlan_prio) and array_key_exists(intval($vlan_prio), $dhcp_vlan_prios)) {
                                $this->validated_data[$next_if]["dhcp6cvpt"] = $dhcp_vlan_prios[intval($vlan_prio)];
                            } else {
                                // Loop through our priorities and see if value matches
                                foreach ($dhcp_vlan_prios as $dvp => $dvpval) {
                                    if ($vlan_prio === $dvpval) {
                                        $vlan_prio_found = true;
                                        $this->validated_data[$next_if]["dhcp6cvpt"] = $dvpval;
                                        break;
                                    }
                                }
                                // Check that we found a value in our loop
                                if (!$vlan_prio_found) {
                                    $this->errors[] = APIResponse\get(3033);
                                }
                            }
                        }
                    }
                    // Check for DHCP configuration file override option
                    if (isset($this->initial_data["adv_dhcp6_config_file_override"])) {
                        $this->validated_data[$next_if]["adv_dhcp6_config_file_override"] = "";
                        // Check if a file path was given
                        if (isset($this->initial_data["adv_dhcp6_config_file_override_path"])) {
                            $dhcp_conf_file = $this->initial_data["adv_dhcp6_config_file_override_path"];
                            // Check that our conf file exists
                            if (is_file($dhcp_conf_file)) {
                                $this->validated_data[$next_if]["adv_dhcp6_config_file_override_path"] = $dhcp_conf_file;
                            } else {
                                $this->errors[] = APIResponse\get(3034);
                            }
                        }
                    }
                    // IPv6 SLAAC TYPE
                } elseif ($type6 === "slaac") {
                    $this->validated_data[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to slaac
                    // IPv6 6RD TYPE
                } elseif ($type6 === "6rd") {
                    $this->validated_data[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to 6rd
                    $this->validated_data[$next_if]["prefix-6rd-v4plen"] = $this->initial_data["prefix-6rd-v4plen"];    // Default prefix len
                    // Check for a 6RD border relay
                    if (isset($this->initial_data["gateway-6rd"])) {
                        $gw6rd = $this->initial_data["gateway-6rd"];
                        // Check that our gateway is a valid IPv4 address
                        if (is_ipaddrv4($gw6rd)) {
                            $this->validated_data[$next_if]["gateway-6rd"] = $this->initial_data["gateway-6rd"];
                        } else {
                            $this->errors[] = APIResponse\get(3035);
                        }
                    } else {
                        $this->errors[] = APIResponse\get(3036);
                    }
                    // Check for a 6RD prefix
                    if (isset($this->initial_data["prefix-6rd"])) {
                        $this->validated_data[$next_if]["prefix-6rd"] = $this->initial_data["prefix-6rd"];
                    }
                    // Check for a 6RD prefix length
                    if (isset($this->initial_data["prefix-6rd-v4plen"])) {
                        $prefix_len = $this->initial_data["prefix-6rd-v4plen"];
                        // Check if our prefix length is within range
                        if (is_numeric($prefix_len) and (0 <= intval($prefix_len) and intval($prefix_len) <= 32)) {
                            $this->validated_data[$next_if]["prefix-6rd-v4plen"] = $this->initial_data["prefix-6rd-v4plen"];
                        } else {
                            $this->errors[] = APIResponse\get(3037);
                        }
                    }
                    // IPv6 TRACK TYPE
                } elseif ($type6 === "track6") {
                    $this->validated_data[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to track6
                    // Check for track 6 interface
                    if (isset($this->initial_data["track6-interface"])) {
                        $track_if = $this->initial_data["track6-interface"];
                        $track_if = APITools\get_pfsense_if_id($track_if);
                        // Check that our gateway is a valid IPv4 address
                        if (array_key_exists($track_if, APITools\get_ipv6_if_list())) {
                            $this->validated_data[$next_if]["track6-interface"] = $this->initial_data["track6-interface"];
                        } else {
                            $this->errors[] = APIResponse\get(3038);
                        }
                    } else {
                        $this->errors[] = APIResponse\get(3039);
                    }
                    // Check for track 6 prefix ID
                    $track_prefix = 0;    // Default our prefix value
                    if (isset($this->initial_data["track6-prefix-id-hex"])) {
                        $track_prefix = $this->initial_data["track6-prefix-id-hex"];
                        // Check that our gateway is a valid IPv4 address
                        if (is_numeric($track_prefix) and ctype_xdigit(strval($track_prefix))) {
                            $this->validated_data[$next_if]["track6-prefix-id--hex"] = intval($track_prefix);
                        } else {
                            $this->errors[] = APIResponse\get(3040);
                        }
                    }
                    // IPv6 6-to-4 TYPE
                } elseif ($type6 === "6to4") {
                    $this->validated_data[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to 6to4
                }
            } else {
                $this->errors[] = APIResponse\get(3041);

            }
        }

    }

}