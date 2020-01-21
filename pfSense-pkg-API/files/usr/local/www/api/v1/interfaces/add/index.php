<?php

// IMPORTS--------------------------------------------------------------------------------------------------------------
require_once("config.inc");
require_once("auth.inc");
require_once("functions.inc");
require_once("interfaces.inc");
require_once("api.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
api_runtime_allowed();    // Check that our configuration allows this API call to run first
header('Content-Type: application/json');
header("Referer: no-referrer");
session_start();    // Start our session. This is only used for tracking user name

// VARIABLES------------------------------------------------------------------------------------------------------------
global $g, $config, $api_resp, $client_id, $client_params;
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-interfaces");    // Array of privileges allowing this action
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$if_list = get_all_avail_interfaces();    // Save all our available interfaces
$allowed_ip4_types = array("staticv4", "dhcp", "ppp", "pppoe", "pptp", "lt2p");    // List of allowed IPv4 if types
$allowed_ip6_types = array("staticv6", "dhcp6", "slaac", "6rd", "6to4", "track6");    // List of allowed IPv6 if types
$next_if = get_next_pfsense_if_id();
$if_ent = array($next_if => []);

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (CREATE)
    if ($http_method === 'POST') {
        $err_found = false;
        $api_resp = array("status" => "bad request", "code" => 400);
        // Get our requested physical interface
        if (isset($client_params["if"])) {
            $interface = trim($client_params["if"]);
            // Check that our interface exists and is not in use
            if (!array_key_exists($interface, $if_list)) {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 120, "message" => "interface does not exist");
            } elseif (isset($if_list[$interface]["in_use"])) {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 121, "message" => "interface already in use");
            }
            $if_ent[$next_if]["if"] = $interface;
        } else {
            $err_found = true;
            $api_resp = $api_resp + array("return" => 11, "message" => "interface id required");
        }
        // Check for our enable value
        if (!$err_found and isset($client_params["enable"])) {
            $enable = true;
            $if_ent[$next_if]["enable"] = "";
        }
        // Check for our MAC address value
        if (!$err_found and isset($client_params["spoofmac"])) {
            $mac_addr = $client_params["spoofmac"];
            // Check if mac addr is valid
            if (is_macaddr($mac_addr)) {
                $if_ent[$next_if]["spoofmac"] = $mac_addr;
            } else {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 16, "message" => "invalid mac address");
            }
        }
        // Check for our MTU value
        if (!$err_found and isset($client_params["mtu"])) {
            $mtu = intval($client_params["mtu"]);
            // Check if MTU is within range
            if (1280 > $mtu or $mtu > 8192) {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 122, "message" => "mtu out of range");
            } elseif ($if_list[$interface]["is_vlan"]) {
                // Check if interface is VLAN and that it's MTU is lower than it's parent interface
                $parent_if = $if_list[$interface]["if"];
                if ($mtu > $parent_if["mtu"]) {
                    $err_found = true;
                    $api_resp = $api_resp + array("return" => 123, "message" => "mtu greater than parent interface");
                }
            } else {
                $if_ent[$next_if]["mtu"] = $mtu;
            }
        }
        // Check for our MSS value
        if (!$err_found and isset($client_params["mss"])) {
            $mss = intval($client_params["mss"]);
            // Check if MSS is within range
            if (576 > $mss or $mss > 65535) {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 124, "message" => "mss out of range");
            } else {
                $if_ent[$next_if]["mss"] = $mss;
            }
        }
        // Check for our SPEED/DUPLEX value
        if (!$err_found and isset($client_params["media"])) {
            $media = $client_params["media"];
            $avail_media = get_if_media_options($interface, true);
            // Loop each of our media options and see if our input matches
            foreach ($avail_media as $mopt) {
                if ($media === $mopt) {
                    $media_found = true;
                    $mopt_list = explode(" ", $mopt);
                    $if_ent[$next_if]["media"] = $mopt_list[0];
                    $if_ent[$next_if]["mediaopt"] = $mopt_list[1];
                    break;
                }
            }
            // If we did not find a match return error
            if (!$media_found) {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 126, "message" => "invalid speed/duplex");
            }
        }
        // Check for our description value
        if (!$err_found and isset($client_params["descr"])) {
            $descr = sanitize_str($client_params["descr"]);
            // Check that is interface descriptive name does not alrady exist
            if (!get_pfsense_if_id($descr)) {
                $if_ent[$next_if]["descr"] = $descr;
            } else {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 125, "message" => "interface descriptive name already exists");
            }
        } else {
            $descr = strtoupper($next_if);
            $if_ent[$next_if]["descr"] = $descr;
        }
        // Check for our block private IP value
        if (!$err_found and isset($client_params["blockpriv"])) {
            $block_priv = true;
            $if_ent[$next_if]["blockpriv"] = "";
        }
        // Check for our block private IP value
        if (!$err_found and isset($client_params["blockbogons"])) {
            $block_bogons = true;
            $if_ent[$next_if]["blockbogons"] = "";
        }
        // Check if we have an IPv4 configuration
        if (!$err_found and isset($client_params["type"])) {
            $type = $client_params["type"];
            // Check if our IPv4 config type is allowed
            if (in_array($type, $allowed_ip4_types)) {
                $err_found = false;
                // Gather input for our various IPv4 interface configuration types
                // IPv4 STATICV4 TYPE
                if ($type === "staticv4") {
                    // Check if our IP is set
                    if (!$err_found and isset($client_params["ipaddr"])) {
                        $ipaddr = $client_params["ipaddr"];
                        // Check if IP address is valid
                        if (!is_ipaddrv4($ipaddr)) {
                            $err_found = true;
                            $api_resp["message"] = "invalid ip address";
                            $api_resp["return"] = 7;
                        } elseif (is_ip_in_use($ipaddr)) {
                            $err_found = true;
                            $api_resp["message"] = "interface ip address already in use";
                            $api_resp["return"] = 130;
                        } else {
                            $if_ent[$next_if]["ipaddr"] = $ipaddr;
                        }
                    } else {
                        // Update our message if we did not already encounter an error
                        if (!$err_found) {
                            $err_found = true;
                            $api_resp["message"] = "interface type static4 requires ipv4 address";
                            $api_resp["return"] = 129;
                        }
                    }
                    // Check if our subnet is valid
                    if (!$err_found and isset($client_params["subnet"])) {
                        $subnet = strval($client_params["subnet"]);
                        // Check if our subnet is within range
                        if (!is_subnet($ipaddr."/".$subnet)) {
                            $err_found = true;
                            $api_resp["message"] = "invalid subnet";
                            $api_resp["return"] = 9;
                        } else {
                            $if_ent[$next_if]["subnet"] = $subnet;
                        }
                    } else {
                        // Update our message if we did not already encounter an error
                        if (!$err_found) {
                            $err_found = true;
                            $api_resp["message"] = "interface type static4 requires subnet bitmask";
                            $api_resp["return"] = 131;
                        }
                    }
                    // Check if user specified a network gateway, if so check if it's valid
                    if (!$err_found and isset($client_params["gateway"])) {
                        $gateway = $client_params["gateway"];
                        // Check if this gateway exists
                        if (!is_gateway($gateway)) {
                            $err_found = true;
                            $api_resp["message"] = "invalid gateway";
                            $api_resp["return"] = 12;
                        } else {
                            $if_ent[$next_if]["gateway"] = $gateway;
                        }
                    }
                // IPv4 DHCP TYPE
                } elseif ($type === "dhcp") {
                    $if_ent[$next_if]["ipaddr"] = $type;    // Set our ipaddr value to dhcp
                    // Check if we have a dhcphostname value
                    if (!$err_found and isset($client_params["dhcphostname"])) {
                        $if_ent[$next_if]["dhcphostname"] = strval($client_params["dhcphostname"]);
                    }
                    // Check if we have a alias-address value
                    if (!$err_found and isset($client_params["alias-address"])) {
                        if (is_ipaddrv4($client_params["alias-address"])) {
                            $if_ent[$next_if]["alias-address"] = strval($client_params["alias-address"]);
                            if (isset($client_params["alias-subnet"])) {
                                $dhcpaliasnet = str($client_params["alias-subnet"]);
                                if (is_subnet($if_ent[$next_if]["alias-address"]."/".$dhcpaliasnet)) {
                                    $if_ent[$next_if]["alias-subnet"] = $dhcpaliasnet;
                                }
                            } else {
                                $if_ent[$next_if]["alias-subnet"] = 32;
                            }
                        } else {
                            $err_found = true;
                            $api_resp["message"] = "invalid dhcp alias address";
                            $api_resp["return"] = 132;
                        }
                    }
                    // Check if we have a dhcprejectfrom value
                    if (!$err_found and isset($client_params["dhcprejectfrom"])) {
                        $dhcpreject = $client_params["dhcprejectfrom"];
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
                            $err_found = true;
                            $api_resp["message"] = "invalid dhcp reject from address";
                            $api_resp["return"] = 133;
                        } else {
                            $if_ent[$next_if]["dhcprejectfrom"] = $dhcpreject;
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
                        if (!$err_found and isset($client_params[$tp])) {
                            // Check that value is in range
                            $dhcp_attr = intval($client_params[$tp]);
                            if ($dhcp_attr >= $data["min"]) {
                                $if_ent[$next_if][$tp] = $dhcp_attr;
                                $if_ent[$next_if]["adv_dhcp_pt_values"] = "SavedCfg";
                            } else {
                                $err_found = true;
                                $api_resp["message"] = "invalid ".$data["keyword"]." value";
                                $api_resp["return"] = $data["return"];
                                break;
                            }
                        }
                    }
                    // Check for advance DHCP config
                    if (!$err_found and isset($client_params["adv_dhcp_config_advanced"])) {
                        $if_ent[$next_if]["adv_dhcp_config_advanced"] = "yes";
                        // Check for our DHCP options
                        $dhcp_opts = array(
                            "adv_dhcp_send_options",
                            "adv_dhcp_request_options",
                            "adv_dhcp_required_options",
                            "adv_dhcp_option_modifiers"
                        );
                        foreach ($dhcp_opts as $do) {
                            // Check if option exists
                            if (!$err_found and isset($client_params[$do])) {
                                $if_ent[$next_if][$do] = strval($client_params[$do]);
                            }
                        }
                    }
                    // Check for DHCP configuration file override option
                    if (!$err_found and isset($client_params["adv_dhcp_config_file_override"])) {
                        $if_ent[$next_if]["adv_dhcp_config_file_override"] = "";
                        // Check if a file path was given
                        if (isset($client_params["adv_dhcp_config_file_override_path"])) {
                            $dhcp_conf_file = $client_params["adv_dhcp_config_file_override_path"];
                            // Check that our conf file exists
                            if (is_file($dhcp_conf_file)) {
                                $if_ent[$next_if]["adv_dhcp_config_file_override"] = $dhcp_conf_file;
                            } else {
                                $err_found = true;
                                $api_resp["message"] = "invalid dhcp configuration file";
                                $api_resp["return"] = 141;
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
                    if (!$err_found and isset($client_params["dhcpvlanenable"])) {
                        $if_ent[$next_if]["dhcpvlanenable"] = "";
                        if (isset($client_params["dhcpcvpt"])) {
                            $vlan_prio = strtolower($client_params["dhcpcvpt"]);
                            echo $vlan_prio;
                            // Check if VLAN priority was provided as number
                            if (is_numeric($vlan_prio) and array_key_exists(intval($vlan_prio), $dhcp_vlan_prios)) {
                                $if_ent[$next_if]["dhcpcvpt"] = $dhcp_vlan_prios[intval($vlan_prio)];
                            } else {
                                // Loop through our priorities and see if value matches
                                foreach ($dhcp_vlan_prios as $dvp => $dvpval) {
                                    if ($vlan_prio === $dvpval) {
                                        $vlan_prio_found = true;
                                        $if_ent[$next_if]["dhcpcvpt"] = $dvpval;
                                        break;
                                    }
                                }
                                // Check that we found a value in our loop
                                if (!$vlan_prio_found) {
                                    $err_found = true;
                                    $api_resp["message"] = "invalid dhcp vlan priority value";
                                    $api_resp["return"] = 140;
                                }
                            }
                        }
                    }
                }
            } else {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 126, "message" => "invalid ipv4 configuration type");
            }
        }
        // Check if we have an IPv6 configuration
        if (!$err_found and isset($client_params["type6"])) {
            $type6 = $client_params["type6"];
            // Check if our IPv6 config type is allowed
            if (in_array($type6, $allowed_ip6_types)) {
                // Gather input for our various IPv6 interface configuration types
                // IPv6 STATICV6 TYPE
                if ($type6 === "staticv6") {
                    // Check if our IP is set
                    if (!$err_found and isset($client_params["ipaddrv6"])) {
                        $ipaddrv6 = $client_params["ipaddrv6"];
                        // Check if IP address is valid
                        if (!is_ipaddrv6($ipaddrv6)) {
                            $err_found = true;
                            $api_resp["message"] = "invalid ip address";
                            $api_resp["return"] = 7;
                        } elseif (is_ip_in_use($ipaddrv6)) {
                            $err_found = true;
                            $api_resp["message"] = "interface ip address already in use";
                            $api_resp["return"] = 130;
                        } else {
                            $if_ent[$next_if]["ipaddrv6"] = $ipaddrv6;
                        }
                    } else {
                        // Update our message if we did not already encounter an error
                        if (!$err_found) {
                            $err_found = true;
                            $api_resp["message"] = "interface type staticv6 requires ipv6 address";
                            $api_resp["return"] = 142;
                        }
                    }
                    // Check if our subnet is valid
                    if (!$err_found and isset($client_params["subnetv6"])) {
                        $subnetv6 = strval($client_params["subnetv6"]);
                        // Check if our subnet is within range
                        if (!is_subnet($ipaddrv6 . "/" . $subnetv6)) {
                            $err_found = true;
                            $api_resp["message"] = "invalid subnet";
                            $api_resp["return"] = 9;
                        } else {
                            $if_ent[$next_if]["subnetv6"] = $subnetv6;
                        }
                    } else {
                        // Update our message if we did not already encounter an error
                        if (!$err_found) {
                            $err_found = true;
                            $api_resp["message"] = "interface type staticv6 requires subnet bitmask";
                            $api_resp["return"] = 143;
                        }
                    }
                    // Check if user specified a network gateway, if so check if it's valid
                    if (!$err_found and isset($client_params["gatewayv6"])) {
                        $gatewayv6 = $client_params["gatewayv6"];
                        // Check if this gateway exists
                        if (!is_gateway($gatewayv6)) {
                            $err_found = true;
                            $api_resp["message"] = "invalid gateway";
                            $api_resp["return"] = 12;
                        } else {
                            $if_ent[$next_if]["gatewayv6"] = $gatewayv6;
                        }
                    }
                    // Check if user set ipv6usev4iface value
                    if (!$err_found and isset($client_params["ipv6usev4iface"])) {
                        $if_ent[$next_if]["ipv6usev4iface"] = "";
                    }
                    // IPv6 DHCP6 TYPE
                } elseif ($type6 === "dhcp6") {
                    $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to dhcp6
                    // Check if user set ipv6usev4iface value
                    if (!$err_found and isset($client_params["ipv6usev4iface"])) {
                        $if_ent[$next_if]["ipv6usev4iface"] = "";
                    }
                    // Check if user set dhcp6prefixonly value
                    if (!$err_found and isset($client_params["dhcp6prefixonly"])) {
                        $if_ent[$next_if]["dhcp6prefixonly"] = "";
                    }
                    // Check if user set dhcp6-ia-pd-send-hint value
                    if (!$err_found and isset($client_params["dhcp6-ia-pd-send-hint"])) {
                        $if_ent[$next_if]["dhcp6-ia-pd-send-hint"] = "";
                    }
                    // Check if user set dhcp6debug value
                    if (!$err_found and isset($client_params["dhcp6debug"])) {
                        $if_ent[$next_if]["dhcp6debug"] = "";
                    }
                    // Check if user set dhcp6withoutra value
                    if (!$err_found and isset($client_params["dhcp6withoutra"])) {
                        $if_ent[$next_if]["dhcp6withoutra"] = "";
                    }
                    // Check if user set dhcp6norelease value
                    if (!$err_found and isset($client_params["dhcp6norelease"])) {
                        $if_ent[$next_if]["dhcp6norelease"] = "";
                    }
                    // Check if user set dhcp6vlanenable value
                    if (!$err_found and isset($client_params["dhcp6vlanenable"])) {
                        $if_ent[$next_if]["dhcp6vlanenable"] = "";
                    }
                    // Check if user set dhcp6-ia-pd-len value
                    if (!$err_found and isset($client_params["dhcp6-ia-pd-len"])) {
                        // Set array of allowed prefix delegation sizes and their config translation
                        $dhcp6_del_size = intval($client_params["dhcp6-ia-pd-len"]);
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
                            $if_ent[$next_if]["dhcp6-ia-pd-len"] = $allowed_size[$dhcp6_del_size];
                        } else {
                            $err_found = true;
                            $api_resp["message"] = "invalid dhcp6 prefix delegation size";
                            $api_resp["return"] = 145;
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
                    if (!$err_found and isset($client_params["dhcp6vlanenable"])) {
                        $if_ent[$next_if]["dhcp6vlanenable"] = "";
                        if (isset($client_params["dhcp6cvpt"])) {
                            $vlan_prio = strtolower($client_params["dhcp6cvpt"]);
                            echo $vlan_prio;
                            // Check if VLAN priority was provided as number
                            if (is_numeric($vlan_prio) and array_key_exists(intval($vlan_prio), $dhcp_vlan_prios)) {
                                $if_ent[$next_if]["dhcp6cvpt"] = $dhcp_vlan_prios[intval($vlan_prio)];
                            } else {
                                // Loop through our priorities and see if value matches
                                foreach ($dhcp_vlan_prios as $dvp => $dvpval) {
                                    if ($vlan_prio === $dvpval) {
                                        $vlan_prio_found = true;
                                        $if_ent[$next_if]["dhcp6cvpt"] = $dvpval;
                                        break;
                                    }
                                }
                                // Check that we found a value in our loop
                                if (!$vlan_prio_found) {
                                    $err_found = true;
                                    $api_resp["message"] = "invalid dhcp6 vlan priority value";
                                    $api_resp["return"] = 144;
                                }
                            }
                        }
                    }
                    // Check for DHCP configuration file override option
                    if (!$err_found and isset($client_params["adv_dhcp6_config_file_override"])) {
                        $if_ent[$next_if]["adv_dhcp6_config_file_override"] = "";
                        // Check if a file path was given
                        if (isset($client_params["adv_dhcp6_config_file_override_path"])) {
                            $dhcp_conf_file = $client_params["adv_dhcp6_config_file_override_path"];
                            // Check that our conf file exists
                            if (is_file($dhcp_conf_file)) {
                                $if_ent[$next_if]["adv_dhcp6_config_file_override_path"] = $dhcp_conf_file;
                            } else {
                                $err_found = true;
                                $api_resp["message"] = "invalid dhcp6 configuration file";
                                $api_resp["return"] = 146;
                            }
                        }
                    }
                    // IPv6 SLAAC TYPE
                } elseif ($type6 === "slaac") {
                    $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to slaac
                    // IPv6 6-to-4 TYPE
                } elseif ($type6 === "6to4") {
                    $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to 6to4
                }
            } else {
                $err_found = true;
                $api_resp = $api_resp + array("return" => 127, "message" => "invalid ipv6 configuration type");
            }
        }

        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "PHYSICAL INTERFACE:" . PHP_EOL;
            echo var_dump($interface) . PHP_EOL;
            echo "pfSENSE INTERFACE ID:" . PHP_EOL;
            echo var_dump($next_if) . PHP_EOL;
            echo "ENABLE:" . PHP_EOL;
            echo var_dump($enable) . PHP_EOL;
            echo "CUSTOM MAC ADDRESS:".PHP_EOL;
            echo var_dump($mac_addr).PHP_EOL;
            echo "MTU:" . PHP_EOL;
            echo var_dump($mtu) . PHP_EOL;
            echo "MSS:" . PHP_EOL;
            echo var_dump($mss) . PHP_EOL;
            echo "DESCRIPTIVE NAME:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;
            echo "IPv4 CONFIGURATION TYPE".PHP_EOL;
            echo var_dump($type).PHP_EOL;
            echo "IPv4 CONFIGURATION TYPE".PHP_EOL;
            echo var_dump($type6).PHP_EOL;
            echo "IP CONFIGURATION".PHP_EOL;
            echo var_dump($if_ent).PHP_EOL;
        }

        // Check if we encountered an error
        if ($err_found === true) {
            http_response_code($api_resp["code"]);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        // Apply our configuration
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Added interface via API";    // Add a change note
        $config["interfaces"] = $config["interfaces"] + $if_ent;    // Add our new interface config
        write_config(sprintf(gettext($change_note)));    // Write our configuration change
        $apply_if = apply_interface_config($if_ent);
        if ($apply_if) {
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "interface added");
            $api_resp["data"] = $if_ent;
            http_response_code(200);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        } else {
            // Return error response if our loop did not find a match
            $api_resp = array("status" => "server error", "code" => 500, "return" => 4, "message" => "process fail");
            http_response_code(500);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
    } else {
        $api_resp = array("status" => "bad request", "code" => 400, "return" => 2, "message" => "invalid http method");
        http_response_code(400);
        echo json_encode($api_resp) . PHP_EOL;
        die();
    }
} else {
    http_response_code(401);
    echo json_encode($api_resp) . PHP_EOL;
    die();
}

