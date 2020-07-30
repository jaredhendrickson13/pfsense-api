<?php

// IMPORTS--------------------------------------------------------------------------------------------------------------
require_once("config.inc");
require_once("auth.inc");
require_once("functions.inc");
require_once("api.inc");
require_once("filter.inc");
require_once("priv.defs.inc");
require_once("services.inc");
require_once("system.inc");
require_once("util.inc");
require_once("interfaces.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
header('Content-Type: application/json');
header("Referer: no-referrer");
api_runtime_allowed();    // Check that our configuration allows this API call to run first
session_start();    // Start our session. This is only used for tracking user name
$pf_ver_num = get_pfsense_version()["program"];    // Pull our full pfSense version

<<<<<<< Updated upstream
=======

function api_system_tables() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-diagnostics-tables");    // Array of page permissions allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method

    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            $table_status = [];
            $table_status["tables"] = get_table_status();

            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $table_status = api_extended_search($table_status, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $table_status);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

>>>>>>> Stashed changes
function api_status_carp() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-carp");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method

    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            $carp_status = [];
            $carp_status["enable"] = is_carp_enabled();
            $carp_status["maintenance_mode"] = isset($config["virtualip_carp_maintenancemode"]);
            $carp_status["interfaces"] = get_carp_if_status();
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $carp_status = api_extended_search($carp_status, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $carp_status);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_status_carp_modify() {
    # VARIABLES
    global $err_lib, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-carp");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $carp_stat_arr = [];    // Init array to track what changes were made
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            // Check if user specified enable value
            if (isset($client_params['enable'])) {
                // Check if value is true or false
                if (boolval($client_params['enable'])) {
                    $enable = true;
                } else {
                    $enable = false;
                }
                $carp_stat_arr["enable"] = $enable;
            }
            // Check if user specified maintenance mode value
            if (isset($client_params['maintenance_mode'])) {
                // Check if value is true or false
                if (boolval($client_params['maintenance_mode'])) {
                    $mm_enable = true;
                } else {
                    $mm_enable = false;
                }
                $carp_stat_arr["maintenance_mode"] = $mm_enable;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "CARP ENABLED:" . PHP_EOL;
                echo var_dump($enable) . PHP_EOL;
                echo "CARP MAINTENANCE MODE ENABLED:" . PHP_EOL;
                echo var_dump($mm_enable) . PHP_EOL;
            }
            // Add our CARP settings
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            interfaces_carp_set_maintenancemode($mm_enable);    // Set our maintenance mode value
            enable_carp($enable);    // Set our CARP enable value
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $carp_stat_arr);
            $api_resp["message"] = "Successfully modified CARP settings";
            $api_resp["data"] = $carp_stat_arr;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_nat() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-virtualipaddresses");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $nat_array = array();    // Init our array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a virtual IP configuration
            if (!empty($config["nat"])) {
                $nat_array = $config["nat"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $nat_array = api_extended_search($nat_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $nat_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_nat_portforwards() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-nat-portforward");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $nat_array = array();    // Init our array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a virtual IP configuration
            if (!empty($config["nat"]["rule"])) {
                $nat_array = $config["nat"]["rule"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $nat_array = api_extended_search($nat_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $nat_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_nat_portforwards_add() {
    # VARIABLES
    global $err_lib, $g, $config, $tracker, $api_resp, $client_id, $client_params;
    $user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-nat-portforward-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $next_rule_id = count($config["nat"]["rule"]);    // Save our next rule ID
    $allowed_nat_ref = array("enable", "disable", "purenat");    // Save our allow NAT reflection types
    // Array of supported protocols
    $allowed_prot = array(
        "tcp",
        "udp",
        "tcp/udp",
        "icmp",
        "esp",
        "ah",
        "gre",
        "ipv6",
        "igmp",
        "pim",
        "ospf"
    );
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['interface'])) {
                $interface = $client_params['interface'];
                $interface = get_pfsense_if_id($interface);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['protocol'])) {
                $protocol = $client_params['protocol'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4001);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['target'])) {
                $localip = $client_params['target'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4002);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['local-port'])) {
                $localport = $client_params['local-port'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4003);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['src'])) {
                $src = $client_params['src'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4004);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['srcport'])) {
                $srcport = $client_params['srcport'];
            }
            if (isset($client_params['dst'])) {
                $dst = $client_params['dst'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4005);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['dstport'])) {
                $dstport = $client_params['dstport'];
            }
            if (isset($client_params['disabled'])) {
                if ($client_params['disabled']) {
                    $disabled = true;
                }
            }
            if (isset($client_params['nordr'])) {
                if ($client_params['nordr']) {
                    $nordr = true;
                }
            }
            if (isset($client_params['nosync'])) {
                if ($client_params['nosync'] === true) {
                    $nosync = true;
                }
            }
            if (isset($client_params['top'])) {
                if ($client_params['top']) {
                    $top = "top";
                }
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            }
            if (isset($client_params['natreflection'])) {
                $natreflection = $client_params['natreflection'];
            }

            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "INTERFACE:" . PHP_EOL;
                echo var_dump($interface) . PHP_EOL;
                echo "PROTOCOL:" . PHP_EOL;
                echo var_dump($protocol) . PHP_EOL;
                echo "REDIRECT IP:" . PHP_EOL;
                echo var_dump($localip) . PHP_EOL;
                echo "REDIRECT PORT:" . PHP_EOL;
                echo var_dump($localport) . PHP_EOL;
                echo "SOURCE:" . PHP_EOL;
                echo var_dump($src) . PHP_EOL;
                echo "SOURCE PORT:" . PHP_EOL;
                echo var_dump($srcport) . PHP_EOL;
                echo "DESTINATION:" . PHP_EOL;
                echo var_dump($dst) . PHP_EOL;
                echo "DESTINATION PORT:" . PHP_EOL;
                echo var_dump($dstport) . PHP_EOL;
                echo "NO RDR:" . PHP_EOL;
                echo var_dump($nordr) . PHP_EOL;
                echo "DISABLED:" . PHP_EOL;
                echo var_dump($disabled) . PHP_EOL;
                echo "NO SYNC:" . PHP_EOL;
                echo var_dump($nosync) . PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "NAT REFLECTION:".PHP_EOL;
                echo var_dump($natreflection).PHP_EOL;
                echo "ADD TO TOP:" . PHP_EOL;
                echo var_dump($top) . PHP_EOL;
                echo "NEXT RULE ID:" . PHP_EOL;
                echo var_dump($next_rule_id) . PHP_EOL;
            }
            // INPUT VALIDATION/FORMATTING
            $api_resp = array("status" => "bad request", "code" => 400);
            // Check that our required array/interface values are valid
            if (!is_string($interface)) {
                $api_resp["return"] = 4006;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!in_array($protocol, $allowed_prot)) {
                $api_resp["return"] = 4007;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (isset($natreflection) and !in_array($natreflection, $allowed_nat_ref)) {
                $api_resp["return"] = 4008;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!is_ipaddrv4($localip) and !alias_in_use($localip)) {
                $api_resp["return"] = 4009;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!is_port_or_range($localport)) {
                $api_resp["return"] = 4010;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            $rule_ent = array();
            // Check if rule is disabled
            if ($disabled) {
                $rule_ent["disabled"] = "";
            }
            // Check if pfsync is disabled
            if ($nosync) {
                $rule_ent["nosync"] = "";
            }
            // Check if RDR is disabled is disabled
            if ($nordr) {
                $rule_ent["nordr"] = "";
            }
            // Check if user specified NAT reflection
            if ($natreflection) {
                $rule_ent["natreflection"] = $natreflection;
            }
            $rule_ent["interface"] = $interface;
            $rule_ent["protocol"] = $protocol;
            $rule_ent["source"] = array();
            $rule_ent["destination"] = array();
            $rule_ent["target"] = $localip;
            $rule_ent["local-port"] = $localport;
            $rule_ent["descr"] = $descr;
            $rule_ent["associated-rule-id"] = "pass";
            $rule_ent["created"] = array("time" => time(), "username" => $user_created_msg);
            $rule_ent["updated"] = $rule_ent["created"];
            // Check if our source and destination values are valid
            foreach (array("source" => $src, "destination" => $dst) as $dir => $val) {
                $dir_check = is_valid_rule_addr($val, $dir);
                if (!$dir_check["valid"]) {
                    if ($dir === "source") {
                        $api_resp["return"] = 4011;
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    } else {
                        $api_resp["return"] = 4012;
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                } else {
                    $rule_ent = array_merge($rule_ent, $dir_check["data"]);
                }
            }
            // Check if protocol calls for additional specifications
            if (in_array($protocol, array("tcp", "udp", "tcp/udp"))) {
                $port_req = true;
            }
            // Check our src and dst port values if ports are required
            if ($port_req) {
                foreach (array("source" => $srcport, "destination" => $dstport) as $dir => $val) {
                    $val = str_replace("-", ":", $val);
                    if (!is_port_or_range($val) and $val !== "any") {
                        if ($dir === "source") {
                            $api_resp["return"] = 4013;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        } else {
                            $api_resp["return"] = 4014;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    } elseif ($val !== "any") {
                        $rule_ent[$dir]["port"] = str_replace(":", "-", $val);
                    }
                }
            }
            // Add our port forward
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added NAT rule via API";    // Add a change note
            $config["nat"]["rule"][] = $rule_ent;    // Write to our master config
            sort_nat_rules($top, $next_rule_id);    // Sort our nat rules
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            filter_configure();    // Ensure our firewall filter is reloaded
            // Loop through each alias and see if our alias was added successfully
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully added port forward";
            $api_resp["data"] = $rule_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_nat_portforwards_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-nat-portforward-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['id'])) {
                $rule_id = $client_params['id'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4015);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "RULE ID TO DELETE:" . PHP_EOL;
                echo var_dump($rule_id) . PHP_EOL;
            }
            // Check that our rule ID exists
            if (array_key_exists($rule_id, $config["nat"]["rule"])) {
                $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                $change_note = " Deleted NAT rule via API";    // Add a change note
                $del_rule = $config["nat"]["rule"][$rule_id];    // Save the rule we are deleting
                unset($config["nat"]["rule"][$rule_id]);    // Remove rule from our config
                sort_nat_rules();    // Sort our NAT rules
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
                filter_configure();    // Ensure our firewall filter is reloaded
                // Loop through each alias and see if our alias was added successfully
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully deleted port forward";
                $api_resp["data"] = $del_rule;
                return $api_resp;
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4016);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_virtualips() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-virtualipaddresses");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $vip_array = array();    // Init our virtual IP array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a virtual IP configuration
            if (!empty($config["virtualip"]["vip"])) {
                $vip_array = $config["virtualip"]["vip"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $vip_array = api_extended_search($vip_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $vip_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_virtualips_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-virtualipaddress-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['id'])) {
                $vip_id = $client_params['id'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4017);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "VIRTUAL IP TO DELETE:" . PHP_EOL;
                echo var_dump($vip_id) . PHP_EOL;
            }
            // Check that our rule ID exists
            if (array_key_exists($vip_id, $config["virtualip"]["vip"])) {
                $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                $change_note = " Deleted virtual IP via API";    // Add a change note
                $del_vip = $config["virtualip"]["vip"][$vip_id];    // Save the virtual IP we are deleting
                bring_down_virtual_ip($del_vip, $vip_id);    // Bring down our virtual IP and delete it from config
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
                send_event("filter reload");    // Ensure our firewall filter is reloaded
                // Loop through each alias and see if our alias was added successfully
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully deleted virtual IP";
                $api_resp["data"] = $del_vip;
                return $api_resp;
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4018);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_virtualips_add() {
    # VARIABLES
    global $err_lib, $g, $config, $tracker, $api_resp, $client_id, $client_params;
    $user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-virtualipaddress-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $allowed_vip_modes = array("ipalias", "carp", "proxyarp", "other");    // Save our allowed vip modes in array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        $api_resp = array("status" => "bad request", "code" => 400);
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['mode'])) {
                $mode = strtolower($client_params['mode']);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4019);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['interface'])) {
                $interface = $client_params['interface'];
                $interface = get_pfsense_if_id($interface);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4020);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['subnet'])) {
                $subnet = $client_params['subnet'];
                // If a single IPv4 or IPv6, append the subnet mask for one address
                if (is_ipaddrv4($subnet)) {
                    $subnet = $subnet."/32";
                } elseif (is_ipaddrv6($subnet)) {
                    $subnet = $subnet."/128";
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4021);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            } else {
                $descr = "";
            }
            if (isset($client_params['noexpand'])) {
                $noexpand = true;
            }
            if (!in_array($mode, $allowed_vip_modes)) {
                $api_resp["return"] = 4023;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // CARP required attributes
            if ($mode === "carp" and isset($client_params['vhid'])) {
                $vhid = $client_params['vhid'];
            }
            if ($mode === "carp" and isset($client_params['advskew'])) {
                $advskew = intval($client_params['advskew']);
            } else {
                $advskew = 0;    // Default skew to 0
            }
            if ($mode === "carp" and isset($client_params['advbase'])) {
                $advbase = intval($client_params['advbase']);
            } else {
                $advbase = 1;    // Default base to 0
            }
            if ($mode === "carp" and isset($client_params['password'])) {
                $password = $client_params['password'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4022);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "MODE:" . PHP_EOL;
                echo var_dump($mode) . PHP_EOL;
                echo "INTERFACE:" . PHP_EOL;
                echo var_dump($interface) . PHP_EOL;
                echo "SUBNET:" . PHP_EOL;
                echo var_dump($subnet) . PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "DISABLE IP EXPANSION:" . PHP_EOL;
                echo var_dump($noexpand) . PHP_EOL;
                echo "CARP VHID" . PHP_EOL;
                echo var_dump($vhid) . PHP_EOL;
                echo "CARP ADVERTISEMENT BASE" . PHP_EOL;
                echo var_dump($advbase) . PHP_EOL;
                echo "CARP ADVERTISEMENT SKEW" . PHP_EOL;
                echo var_dump($advskew) . PHP_EOL;
                echo "CARP PASSWORD" . PHP_EOL;
                echo var_dump(isset($password)) . PHP_EOL;
            }
            // INPUT VALIDATION/FORMATTING
            $api_resp = array("status" => "bad request", "code" => 400);
            // Check that our required array/interface values are valid
            if (!is_string($interface)) {
                $api_resp["return"] = 4024;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!is_subnet($subnet)) {
                $api_resp["return"] = 4025;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Split our subnet into an address and subnet mask
            $subnet_split = explode("/", $subnet);
            $subnet = $subnet_split[0];
            $subnet_bits = $subnet_split[1];
            // Check that our subnet is not used elsewhere
            if (is_ip_in_use($subnet)) {
                $api_resp["return"] = 4026;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check if VHID was specified
            if (isset($vhid)) {
                if (vhid_exists($vhid)) {
                    $api_resp["return"] = 4027;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                } elseif (1 > $vhid or $vhid > 255) {
                    $api_resp["return"] = 4028;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } elseif ($mode === "carp" and !isset($vhid)) {
                $vhid = next_vhid();    // Pull our next available VHID
            }
            // Check if advertisement base was specified
            if (isset($advbase)) {
                if (1 > $advbase or $advbase > 254) {
                    $api_resp["return"] = 4029;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            // Check if advertisement skew was specified
            if (isset($advskew)) {
                if (0 > $advskew or $advskew > 254) {
                    $api_resp["return"] = 4030;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            // Initialize our virtual IP configuration array
            if (!is_array($config["virtualip"]["vip"])) {
                $config["virtualip"] = array("vip" => []);
            }
            // Populate our new virtual IP entry
            $vip_ent = array();
            $vip_ent["mode"] = $mode;
            $vip_ent["interface"] = $interface;
            $vip_ent["type"] = "network";
            $vip_ent["subnet"] = $subnet;
            $vip_ent["subnet_bits"] = $subnet_bits;
            $vip_ent["descr"] = $descr;
            // Values specific to CARP
            if ($mode === "carp") {
                $vip_ent["vhid"] = $vhid;
                $vip_ent["advbase"] = $advbase;
                $vip_ent["advskew"] = $advskew;
                $vip_ent["password"] = $password;
                $vip_ent["uniqid"] = uniqid();
            }
            // Values specific to Proxy ARP and other
            if (in_array($mode, array("proxyarp", "other")) and $noexpand) {
                $vip_ent["noexpand"] = "";
            }
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added virtual IP via API";    // Add a change note
            $config["virtualip"]["vip"][] = $vip_ent;    // Write to our master config
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            apply_virtual_ip($vip_ent);    // Apply our backend changes with our new configuration
            // Loop through each alias and see if our alias was added successfully
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully added virtual IP";
            $api_resp["data"] = $vip_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_rules() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-rules");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $rule_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["filter"]["rule"])) {
                $rule_array = $config["filter"]["rule"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $rule_array = api_extended_search($rule_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $rule_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_rules_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-rules-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['id'])) {
                $rule_id = $client_params['id'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4031);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "RULE ID TO DELETE:" . PHP_EOL;
                echo var_dump($rule_id) . PHP_EOL;
            }
            // Check that our rule ID exists
            if (array_key_exists($rule_id, $config["filter"]["rule"])) {
                $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                $change_note = " Deleted firewall rule via API";    // Add a change note
                $del_rule = $config["filter"]["rule"][$rule_id];    // Save the rule we are deleting
                unset($config["filter"]["rule"][$rule_id]);    // Remove rule from our config
                sort_firewall_rules();    // Sort our firewall rules
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
                send_event("filter reload");    // Ensure our firewall filter is reloaded
                // Loop through each alias and see if our alias was added successfully
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully deleted firewall rule";
                $api_resp["data"] = $del_rule;
                return $api_resp;
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4032);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_rules_add() {
    # VARIABLES
    global $err_lib, $g, $config, $tracker, $api_resp, $client_id, $client_params;
    $user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-rules-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $allowed_rule_types = array("pass", "block", "reject");    // Array of allowed rule types
    $allowed_ip_prot = array("inet", "inet6", "inet46");    // Array of allowed IP protocols
    $next_rule_id = count($config["filter"]["rule"]);    // Save our next rule ID
    // Array of supported protocols
    $allowed_prot = array(
        "any",
        "tcp",
        "udp",
        "tcp/udp",
        "icmp",
        "esp",
        "ah",
        "gre",
        "ipv6",
        "igmp",
        "pim",
        "ospf",
        "carp",
        "pfsync"
    );
    // Array of allowed ICMP subtypes
    $icmp_subtypes = array(
        "althost",
        "dataconv",
        "echorep",
        "echoreq",
        "inforep",
        "inforeq",
        "ipv6-here",
        "ipv6-where",
        "maskrep",
        "maskreq",
        "mobredir",
        "mobregrep",
        "mobregreq",
        "paramprob",
        "photuris",
        "redir",
        "routeradv",
        "routersol",
        "skip",
        "squench",
        "timerep",
        "timereq",
        "timex",
        "trace",
        "unreach"
    );
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['type'])) {
                $type = $client_params['type'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4033);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['interface'])) {
                $interface = $client_params['interface'];
                $interface = get_pfsense_if_id($interface);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4034);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ipprotocol'])) {
                $ipprotocol = $client_params['ipprotocol'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4035);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['protocol'])) {
                $protocol = $client_params['protocol'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4036);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;

            }
            if (isset($client_params['src'])) {
                $src = $client_params['src'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4037);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['srcport'])) {
                $srcport = $client_params['srcport'];
            }
            if (isset($client_params['dst'])) {
                $dst = $client_params['dst'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4038);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['dstport'])) {
                $dstport = $client_params['dstport'];
            }
            if (isset($client_params['icmptype'])) {
                $icmp_type = $client_params['icmptype'];
                if (!is_array($icmp_type)) {
                    $icmp_type = array($icmp_type);
                }
            }
            if (isset($client_params['gateway'])) {
                $gateway = $client_params['gateway'];
            }
            if (isset($client_params['disabled'])) {
                $disabled = true;
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            }
            if (isset($client_params['log'])) {
                $log = true;
            }
            if (isset($client_params['top'])) {
                $top = "top";
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "TYPE:" . PHP_EOL;
                echo var_dump($type) . PHP_EOL;
                echo "INTERFACE:" . PHP_EOL;
                echo var_dump($interface) . PHP_EOL;
                echo "IP PROTOCOL:" . PHP_EOL;
                echo var_dump($ipprotocol) . PHP_EOL;
                echo "PROTOCOL:" . PHP_EOL;
                echo var_dump($protocol) . PHP_EOL;
                echo "ICMP SUBTYPES:" . PHP_EOL;
                echo var_dump($icmp_type) . PHP_EOL;
                echo "SOURCE:" . PHP_EOL;
                echo var_dump($src) . PHP_EOL;
                echo "SOURCE PORT:" . PHP_EOL;
                echo var_dump($srcport) . PHP_EOL;
                echo "DESTINATION:" . PHP_EOL;
                echo var_dump($dst) . PHP_EOL;
                echo "DESTINATION PORT:" . PHP_EOL;
                echo var_dump($dstport) . PHP_EOL;
                echo "GATEWAY:" . PHP_EOL;
                echo var_dump($gateway) . PHP_EOL;
                echo "DISABLED:" . PHP_EOL;
                echo var_dump($disabled) . PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "LOG MATCHES:" . PHP_EOL;
                echo var_dump($log) . PHP_EOL;
                echo "ADD TO TOP:" . PHP_EOL;
                echo var_dump($top) . PHP_EOL;
                echo "NEXT RULE ID:" . PHP_EOL;
                echo var_dump($next_rule_id) . PHP_EOL;
            }
            // INPUT VALIDATION/FORMATTING
            $api_resp = array("status" => "bad request", "code" => 400);
            // Check that our required array/interface values are valid
            if (!in_array($type, $allowed_rule_types)) {
                $input_err = true;
                $api_resp["return"] = 4039;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            } elseif (!is_string($interface)) {
                $input_err = true;
                $api_resp["return"] = 4040;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            } elseif (!in_array($ipprotocol, $allowed_ip_prot)) {
                $input_err = true;
                $api_resp["return"] = 4041;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            } elseif (!in_array($protocol, $allowed_prot)) {
                $input_err = true;
                $api_resp["return"] = 4042;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            } elseif (isset($gateway) and !is_gateway($gateway)) {
                $input_err = true;
                $api_resp["return"] = 4043;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            }
            $rule_ent = array();
            // Check if rule is not disabled
            if (!$disabled) {
                $rule_ent["id"] = "";
            }
            // Check if logging is enabled
            if ($log) {
                $rule_ent["log"] = "";
            }
            // Check if gateway was specified
            if (isset($gateway)) {
                $rule_ent["gateway"] = $gateway;
            }
            $rule_ent["type"] = $type;
            $rule_ent["interface"] = $interface;
            $rule_ent["ipprotocol"] = $ipprotocol;
            $rule_ent["source"] = array();
            $rule_ent["destination"] = array();
            $rule_ent["descr"] = $descr;
            $rule_ent["created"] = array("time" => time(), "username" => $user_created_msg);
            $rule_ent["updated"] = $rule_ent["created"];
            // Save our protocol if it is not 'any'
            if ($protocol !== "any") {
                $rule_ent["protocol"] = $protocol;
            }
            // Add logging to config if enabled
            if ($log) {
                $rule_ent["log"] = "";
            }
            // Check if our source and destination values are valid
            foreach (array("source" => $src, "destination" => $dst) as $dir => $val) {
                $dir_check = is_valid_rule_addr($val, $dir);
                if (!$dir_check["valid"] === true) {
                    $input_err = true;
                    if ($dir === "source") {
                        $api_resp["return"] = 4044;
                    } else {
                        $api_resp["return"] = 4045;
                    }
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                } else {
                    $rule_ent = array_merge($rule_ent, $dir_check["data"]);
                }
            }
            // Check if protocol calls for additional specifications
            if (in_array($protocol, array("tcp", "udp", "tcp/udp"))) {
                $port_req = true;
            } elseif ($protocol === "icmp") {
                // Check if user specified ICMP subtypes
                if (!$input_err and is_array($icmp_type)) {
                    // Loop through each of our subtypes
                    foreach ($icmp_type as $ict) {
                        if (!in_array($ict, $icmp_subtypes)) {
                            $input_err = true;
                            $api_resp["return"] = 4046;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                        }
                    }
                    // Write our ICMP subtype config
                    $rule_ent["icmptype"] = implode(",", $icmp_type);
                }
            }
            // Check our src and dst port values if ports are required
            if (!$input_err and $port_req) {
                if (!isset($srcport) or !isset($dstport)) {
                    $input_err = true;
                    $api_resp["return"] = 4047;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                }
                foreach (array("source" => $srcport, "destination" => $dstport) as $dir => $val) {
                    $val = str_replace("-", ":", $val);
                    if (!is_port_or_range($val) and $val !== "any") {
                        $input_err = true;
                        if ($dir === "source") {
                            $api_resp["return"] = 4048;
                        } else {
                            $api_resp["return"] = 4049;
                        }
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                    } elseif ($val !== "any") {
                        $rule_ent[$dir]["port"] = str_replace(":", "-", $val);;
                    }
                }
            }
            // Return error if an error was found during input validation
            if ($input_err) {
                return $api_resp;
            }
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added firewall rule via API";    // Add a change note
            $config["filter"]["rule"][] = $rule_ent;    // Write to our master config
            sort_firewall_rules($top, $next_rule_id);    // Sort our firewall rules
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            send_event("filter reload");    // Ensure our firewall filter is reloaded
            // Loop through each alias and see if our alias was added successfully
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully added firewall rule";
            $api_resp["data"] = $rule_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_aliases() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-aliases");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $alias_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["aliases"]["alias"])) {
                $alias_array = $config["aliases"]["alias"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $alias_array = api_extended_search($alias_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $alias_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_aliases_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-aliases-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
                $name = sanitize_str($name);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4050);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "NAME:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
            }
            // Check that alias is not in use in our configuration
            if (!alias_in_use($name)) {
                // Loop through our current config and find the index ID for our alias to delete
                $c_count = 0;    // Init loop counter
                foreach ($config["aliases"]["alias"] as $ce) {
                    // Check if this entry matches our requested value
                    if ($ce["name"] === $name) {
                        $del_index = $c_count;
                        break;
                    }
                    $c_count++;    // Increase our counter
                }
                // Delete our alias if we found a match,
                $del_conf = [];
                if (is_numeric($del_index)) {
                    $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                    $change_note = " Deleted firewall alias via API";    // Add a change note
                    $del_conf = $config["aliases"]["alias"][$del_index];    // Save our alias config before deleting
                    unset($config["aliases"]["alias"][$del_index]);    // Remove this alias from our configuration
                    $config["aliases"]["alias"] = array_values($config["aliases"]["alias"]);    // Reindex array
                    write_config(sprintf(gettext($change_note)));    // Apply our configuration change
                    send_event("filter reload");    // Ensure our firewall filter is reloaded
                }
                // Check if our alias is still in the configuration, if so return error response
                foreach ($config["aliases"]["alias"] as $se) {
                    if ($se["name"] === $name) {
                        $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                }
                // Return success if we did not find our deleted alias in configuration during loop
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "data" => $del_conf);
                $api_resp["message"] = "Successfully deleted firewall alias";
                return $api_resp;
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4051);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_aliases_delete_address() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-aliases-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $allowed_alias_types = array("host", "network", "port");    // Array of allowed alias types
    $detail = array();
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        $api_resp = array("status" => "bad request", "code" => 400);
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
                $name = sanitize_str($name);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4050);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['address'])) {
                $address = $client_params['address'];
                // Convert string to array
                if (!is_array($address)) {
                    $address = array($address);
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4052);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "NAME:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
                echo "ALIAS VALUES TO DELETE:".PHP_EOL;
                echo var_dump($address).PHP_EOL;
            }
            // Check that our input is valid
            if (!is_string($name)) {
                $type_err = true;
                $api_resp["return"] = 4053;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            } elseif (!is_array($address)) {
                $type_err = true;
                $api_resp["return"] = 4054;
                $api_resp["message"] = $err_lib[$api_resp["return"]];            }
            // Loop through our existing firewall entries and check for our requested alias
            $c_count = 0;
            foreach ($config["aliases"]["alias"] as $ce) {
                if ($name === $ce["name"]) {
                    $alias_found = true;
                    $a_index = $c_count;
                    $curr_addr = explode(" ", $ce["address"]);
                    $curr_detail = explode("||", $ce["detail"]);
                    break;
                }
                $c_count++;    // Increase our counter
            }
            // If we could not find an alias, return error
            if ($alias_found !== true) {
                $type_err = true;
                $api_resp["return"] = 4055;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            }
            // Return bad request if error
            if (isset($type_err)) {
                return $api_resp;
            }
            // Loop through our existing configuration and remove alias address values on match
            $r_count = 0;
            foreach ($curr_addr as $re) {
                if (in_array($re, $address)) {
                    unset($curr_addr[$r_count]);
                    unset($curr_detail[$r_count]);
                }
                $r_count++;    // Increase our counter
            }
            // Add our alias
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Deleted firewall alias address via API";    // Add a change note
            $config["aliases"]["alias"][$a_index]["address"] = implode(" ", $curr_addr);
            $config["aliases"]["alias"][$a_index]["detail"] = implode("||", $curr_detail);
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            send_event("filter reload");    // Ensure our firewall filter is reloaded
            // Loop through each alias and see if our alias was added successfully
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted firewall alias address";
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_aliases_modify() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-aliases-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $allowed_alias_types = array("host", "network", "port");    // Array of allowed alias types
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
                $name = sanitize_str($name);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4050);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params["new_name"])) {
                $new_name = $client_params['new_name'];
                $new_name = sanitize_str($new_name);
            }
            if (isset($client_params['type'])) {
                $type = $client_params['type'];
                $type = trim($type);
            } else {
                $type = alias_get_type($name);
            }
            if (isset($client_params['address'])) {
                $address = $client_params['address'];
                // Convert string to array
                if (!is_array($address)) {
                    $address = array($address);
                }
            }
            if (isset($client_params['descr'])) {
                $descr = strval($client_params['descr']);
                $descr = str_replace(PHP_EOL, "", $descr);
            }
            if (isset($client_params['detail'])) {
                $detail = $client_params['detail'];
                // Convert string to array
                if (!is_array($detail)) {
                    $detail = array($detail);
                }
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "NAME:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
                echo "NEW NAME:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
                echo "TYPE:" . PHP_EOL;
                echo var_dump($type) . PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "ALIAS VALUES:".PHP_EOL;
                echo var_dump($address).PHP_EOL;
                echo "ALIAS VALUE DESCRIPTIONS:" . PHP_EOL;
                echo var_dump($detail) . PHP_EOL;
            }
            // INPUT VALIDATION
            if (!is_alias($name)) {
                $input_err = true;
                $api_resp["return"] = 4055;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            } elseif (isset($new_name) and is_alias($new_name)) {
                $input_err = true;
                $api_resp["return"] = 4056;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            } elseif (isset($type) and !in_array($type, $allowed_alias_types)) {
                $input_err = true;
                $api_resp["return"] = 4057;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
            }
            if (!$input_err and isset($address)) {
                foreach ($address as $nae) {
                    if ($type === "host") {
                        if (!is_ipaddr($nae) and !is_hostname($nae)) {
                            $input_err = true;
                            $api_resp["return"] = 4058;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            break;
                        }
                    }
                    if ($type === "network") {
                        if (!is_subnet($nae) and !is_hostname($nae)) {
                            $input_err = true;
                            $api_resp["return"] = 4059;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            break;
                        }
                    }
                    if ($type === "port") {
                        if (!is_port_or_range($nae)) {
                            $input_err = true;
                            $api_resp["return"] = 4060;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            break;
                        }
                    }
                }
            }
            // If we encountered an error, return error response
            if ($input_err) {
                $api_resp["code"] = 400;
                $api_resp["status"] = "bad request";
                return $api_resp;
            }
            // Save our alias ID
            $c_count = 0;
            foreach ($config["aliases"]["alias"] as $ca) {
                if ($name === $ca["name"]) {
                    $a_index = $c_count;
                }
                $c_count++;
            }
            // Make our alias change
            $curr_ent = $config["aliases"]["alias"][$a_index];    // Save our current alias entry
            if (isset($new_name)) {
                $curr_ent["name"] = $new_name;
                update_alias_name($new_name, $name);    // Update our alias name
            }
            if (isset($type)) {
                $curr_ent["type"] = $type;
            }
            if (isset($descr)) {
                $curr_ent["descr"] = $descr;
            }
            if (isset($address)) {
                $curr_ent["address"] = implode(" ", $address);
            }
            if (isset($detail)) {
                $curr_ent["detail"] = implode("||", $detail);
            }
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Modified firewall alias address via API";    // Add a change note
            $config["aliases"]["alias"][$a_index] = $curr_ent;    // Write to our master config
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            send_event("filter reload");    // Ensure our firewall filter is reloaded
            // Loop through each alias and see if our alias was added successfully
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully modified firewall alias";
            $api_resp["data"] = $config["aliases"]["alias"][$a_index];
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_aliases_add() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-aliases-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $allowed_alias_types = array("host", "network", "port");    // Array of allowed alias types
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        $api_resp = array("status" => "bad request", "code" => 400);
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
                $name = sanitize_str($name);
            } else {
                $api_resp["return"] = 4050;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['type'])) {
                $type = $client_params['type'];
                $type = trim($type);
            } else {
                $api_resp["return"] = 4061;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['address'])) {
                $address = $client_params['address'];
                // Convert string to array
                if (!is_array($address)) {
                    $address = array($address);
                }
            } else {
                $api_resp["return"] = 4052;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            }
            if (isset($client_params['detail'])) {
                $detail = $client_params['detail'];
                // Convert string to array
                if (!is_array($detail)) {
                    $detail = array($detail);
                }
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "NAME:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
                echo "TYPE:" . PHP_EOL;
                echo var_dump($type) . PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "ALIAS VALUES:".PHP_EOL;
                echo var_dump($address).PHP_EOL;
                echo "ALIAS VALUE DESCRIPTIONS:" . PHP_EOL;
                echo var_dump($detail) . PHP_EOL;
            }
            // Check that our input is valid
            if (!is_string($name)) {
                $api_resp["return"] = 4053;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!is_string($type)) {
                $api_resp["return"] = 4062;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!in_array($type, $allowed_alias_types)) {
                $api_resp["return"] = 4057;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (isset($descr) and !is_string($descr)) {
                $api_resp["return"] = 4063;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!is_array($address)) {
                $api_resp["return"] = 4054;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (isset($detail) and !is_array($detail)) {
                $api_resp["return"] = 4063;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (!isset($type_err)) {
                // Loop through our arrays and ensure the values are valid
                $a_count = 0;   // Define a loop counter
                foreach ($address as $ae) {
                    // Conditions for alias type 'port'
                    if ($type === "port") {
                        // Check that our value is numeric
                        if (is_numeric($ae)) {
                            if (1 <= intval($ae) and intval($ae) <= 65535) {
                                $address[$a_count] = strval($ae);
                            } else {
                                $api_resp["return"] = 4065;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        } else {
                            $api_resp["return"] = 4066;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    }
                    // Conditionals for alias type 'network'
                    if ($type === "network") {
                        // Check that values are strings
                        if (is_string($ae)) {
                            // Check that string is a network CIDR
                            if (strpos($ae, "/")) {
                                $net_ip = explode("/", $ae)[0];    // Save our network IP
                                $bit_mask = explode("/", $ae)[1];    // Save our subnet bit mask
                                // Check if our IP is IPv4
                                if (is_ipaddrv4($net_ip)) {
                                    $max_bits = 32;    // Assign our maximum IPv4 bitmask
                                } elseif (is_ipaddrv6($net_ip)) {
                                    $max_bits = 128;    // Assign our maximum IPv4 bitmask
                                } else {
                                    $api_resp["return"] = 4067;
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                                // Check if our bitmask is numeric and in range
                                if (is_numeric($bit_mask)) {
                                    if (1 <= intval($bit_mask) and intval($bit_mask) <= $max_bits) {
                                        continue;
                                    } else {
                                        $api_resp["return"] = 4068;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    }
                                } else {
                                    $api_resp["return"] = 4069;
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                            } else {
                                $api_resp["return"] = 4069;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        } else {
                            $api_resp["return"] = 4070;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    }
                    // Conditions for alias type 'host'
                    if ($type === "host") {
                        // Check that values are strings
                        if (is_string($ae)) {
                            $address[$a_count] = sanitize_str($ae);
                        } else {
                            $api_resp["return"] = 4070;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    }
                    // Increase our counter
                    $a_count++;
                }
                // Check each of our alias details
                foreach ($detail as $de) {
                    if (!is_string($de)) {
                        $api_resp["return"] = 4071;
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                }
            }
            // Check our existing aliases
            if (is_array($config["aliases"])) {
                $c_count = 0;
                // Loop through each alias and see if alias already exists
                foreach ($config["aliases"]["alias"] as $ce) {
                    if ($ce["name"] === $name) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 4056);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                }
            }
            // Add our alias
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added firewall alias via API";    // Add a change note
            $alias_ent = array();   // Init our array
            $alias_ent["name"] = $name;    // Save our alias name
            $alias_ent["type"] = $type;    // Save our type
            $alias_ent["descr"] = $descr;    // Save our description
            $alias_ent["address"] = implode(" ", $address);    // Join array in to space seperated string
            $alias_ent["detail"] = implode("||", $detail);    // Join array in to || seperated string
            $config["aliases"] = !is_array($config["aliases"]) ? array("alias" => []) : $config["aliases"];
            $config["aliases"]["alias"][] = $alias_ent;    // Write our configuration change
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            send_event("filter reload");    // Ensure our firewall filter is reloaded
            // Loop through each alias and see if our alias was added successfully
            foreach ($config["aliases"]["alias"] as $se) {
                if ($se["name"] === $name) {
                    $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                    $api_resp["message"] = "Successfully added firewall alias";
                    $api_resp["data"] = $se;
                    return $api_resp;
                }
            }
            // Return error response if our loop did not find a match
            $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_aliases_add_address() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-firewall-aliases-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $allowed_alias_types = array("host", "network", "port");    // Array of allowed alias types
    $detail = array();
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            $api_resp = array("status" => "bad request", "code" => 400);
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
                $name = sanitize_str($name);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4050);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['address'])) {
                $address = $client_params['address'];
                // Convert string to array
                if (!is_array($address)) {
                    $address = array($address);
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4052);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['detail'])) {
                $detail = $client_params['detail'];
                // Convert string to array
                if (!is_array($detail)) {
                    $detail = array($detail);
                }
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "NAME:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
                echo "ALIAS VALUES:".PHP_EOL;
                echo var_dump($address).PHP_EOL;
                echo "ALIAS VALUE DESCRIPTIONS:" . PHP_EOL;
                echo var_dump($detail) . PHP_EOL;
            }
            // Check that our input is valid
            if (!is_string($name)) {
                $api_resp["return"] = 4053;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (!is_array($address)) {
                $api_resp["return"] = 4054;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (isset($detail) and !is_array($detail)) {
                $api_resp["return"] = 4064;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Loop through our existing firewall entries and check for our requested alias
            $c_count = 0;
            foreach ($config["aliases"]["alias"] as $ce) {
                if ($name === $ce["name"]) {
                    $a_index = $c_count;
                    $type = $ce["type"];
                    $curr_addr = explode(" ", $ce["address"]);
                    $curr_detail = explode("||", $ce["detail"]);
                    break;
                }
                $c_count++;    // Increase our counter
            }
            // If we could not find an alias, return error
            if (!isset($type)) {
                $api_resp["return"] = 4055;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (!isset($type_err)) {
                // Loop through our arrays and ensure the values are valid
                $a_count = 0;   // Define a loop counter
                foreach ($address as $ae) {
                    // Conditions for alias type 'port'
                    if ($type === "port") {
                        // Check that our value is numeric
                        if (is_numeric($ae)) {
                            if (1 <= intval($ae) and intval($ae) <= 65535) {
                                $address[$a_count] = strval($ae);
                            } else {
                                $api_resp["return"] = 4065;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        } else {
                            $api_resp["return"] = 4066;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    }
                    // Conditionals for alias type 'network'
                    if ($type === "network") {
                        // Check that values are strings
                        if (is_string($ae)) {
                            // Check that string is a network CIDR
                            if (strpos($ae, "/")) {
                                $net_ip = explode("/", $ae)[0];    // Save our network IP
                                $bit_mask = explode("/", $ae)[1];    // Save our subnet bit mask
                                // Check if our IP is IPv4
                                if (is_ipaddrv4($net_ip)) {
                                    $max_bits = 32;    // Assign our maximum IPv4 bitmask
                                } elseif (is_ipaddrv6($net_ip)) {
                                    $max_bits = 128;    // Assign our maximum IPv4 bitmask
                                } else {
                                    $api_resp["return"] = 4067;
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                                // Check if our bitmask is numeric and in range
                                if (is_numeric($bit_mask)) {
                                    if (1 <= intval($bit_mask) and intval($bit_mask) <= $max_bits) {
                                        continue;
                                    } else {
                                        $api_resp["return"] = 4068;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    }
                                } else {
                                    $api_resp["return"] = 4069;
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                            } else {
                                $api_resp["return"] = 4069;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        } else {
                            $api_resp["return"] = 4070;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    }
                    // Conditions for alias type 'host'
                    if ($type === "host") {
                        // Check that values are strings
                        if (is_string($ae)) {
                            $address[$a_count] = sanitize_str($ae);
                        } else {
                            $api_resp["return"] = 4070;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    }
                    // Increase our counter
                    $a_count++;
                }
                // Check each of our alias details
                foreach ($detail as $de) {
                    if (!is_string($de)) {
                        $api_resp["return"] = 4071;
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                }
            }
            // Add our alias
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added firewall alias address via API";    // Add a change note
            $new_addr = array_merge($curr_addr, $address);
            $new_detail = array_merge($curr_detail, $detail);
            $config["aliases"]["alias"][$a_index]["address"] = implode(" ", $new_addr);
            $config["aliases"]["alias"][$a_index]["detail"] = implode("||", $new_detail);
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            send_event("filter reload");    // Ensure our firewall filter is reloaded
            // Loop through each alias and see if our alias was added successfully
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully added firewall alias address";
            $api_resp["data"] = $config["aliases"]["alias"][$a_index];
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_states() {
    # VARIABLES
    global $err_lib, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-diagnostics-statessummary");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            $state_array = sort_state_table();
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $state_array = api_extended_search($state_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $state_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_states_size() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-diagnostics-statessummary");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check our maximum state table size
            if (isset($config["system"]["maximumstates"])) {
                $size_array["maximumstates"] = intval($config["system"]["maximumstates"]);
            } else {
                $size_array["maximumstates"] = intval(pfsense_default_state_size());
            }
            // Check our current state table size
            $size_array["currentstates"] = count(sort_state_table());
            // Check our default state table size
            $size_array["defaultmaximumstates"] = intval(pfsense_default_state_size());
            // Allow extended search
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $size_array = api_extended_search($size_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $size_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_firewall_states_size_modify() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params, $client_id;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-advanced-firewall");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'POST') {
            // Check if we set a maximumstates size
            if (isset($client_params["maximumstates"])) {
                $maxsize = $client_params["maximumstates"];
                // Check that our state size is valid
                if (is_numeric($maxsize) and (10 <= intval($maxsize))) {
                    $config["system"]["maximumstates"] = $maxsize;
                } elseif ($maxsize === "default") {
                    unset($config["system"]["maximumstates"]);
                } else {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 4073);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 4072);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Write our state table size
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Modified firewall state table size via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            filter_configure();    // Update our firewall filter
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully modified firewall state table size";
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_config() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-diagnostics-backup-restore", "page-diagnostics-command");    // Allowed privs
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $config_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config)) {
                $config_array = $config;
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $config_array = api_extended_search($config_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $config_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_api() {
    # VARIABLES
    global $err_lib, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $api_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            $api_array = get_api_configuration()[1];
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $api_array = api_extended_search($api_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $api_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_version() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-dashboard-widgets", "page-diagnostics-command");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $ver_path = "/etc/version";    // Assign the path to our version file
    $ver_patch_path = "/etc/version.patch";    // Assign the path to our version patch file
    $ver_bt_path = "/etc/version.buildtime";    // Assign the path to our version build time file
    $ver_lc_path = "/etc/version.lastcommit";    // Assign the path to our version last commit file
    $ver_data = array();    // Init an empty array for our version data
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that our files exist, if so read the files. Otherwise return error
            if (file_exists($ver_path)) {
                $ver_file = fopen($ver_path, "r");    // Open our file
                $ver = str_replace(PHP_EOL, "", fread($ver_file, filesize($ver_path)));    // Save our version data
                $ver_data["version"] = $ver;    // Save to array
            } else {
                $ver_fail = false;    // Save a track indicating our read failed
            }
            if (file_exists($ver_patch_path)) {
                $ver_patch_file = fopen($ver_patch_path, "r");    // Open our file
                $ver_patch = str_replace(PHP_EOL, "", fread($ver_patch_file, filesize($ver_patch_path)));    // Save patch
                $ver_data["patch"] = $ver_patch;    // Save to array
            } else {
                $ver_fail = false;    // Save a track indicating our read failed
            }
            if (file_exists($ver_bt_path)) {
                $ver_bt_file = fopen($ver_bt_path, "r");    // Open our file
                $ver_bt = str_replace(PHP_EOL, "", fread($ver_bt_file, filesize($ver_bt_path)));    // Save bt data
                $ver_data["buildtime"] = $ver_bt;    // Save to array
            } else {
                $ver_fail = false;    // Save a track indicating our read failed
            }
            if (file_exists($ver_lc_path)) {
                $ver_lc_file = fopen($ver_lc_path, "r");    // Open our file
                $ver_lc = str_replace(PHP_EOL, "", fread($ver_lc_file, filesize($ver_lc_path)));    // Save bt data
                $ver_data["lastcommit"] = $ver_lc;    // Save to array
            } else {
                $ver_fail = false;    // Save a track indicating our read failed
            }
            // Check that our version read did not fail
            if ($ver_fail !== false) {
                // Print our JSON response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $ver_data);
                return $api_resp;
            } else {
                $api_resp = array("status" => "not found", "code" => 404, "return" => 8);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_hostname() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system");    // Allowed privs
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $host_array = array("hostname" => "", "domain" => "");    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a hostname configuration
            if (!empty($config["system"]["hostname"])) {
                $host_array["hostname"] = $config["system"]["hostname"];
            }
            // Check that we have a domain configuration
            if (!empty($config["system"]["domain"])) {
                $host_array["domain"] = $config["system"]["domain"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $host_array = api_extended_search($host_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $host_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_dns_modify() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params, $client_id;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $dns_ent = [];
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['dnsserver'])) {
                $dns_servers = $client_params['dnsserver'];
                // If value is not an array, convert it
                if (!is_array($dns_servers)) {
                    $dns_servers = array($dns_servers);
                }
                // Loop through our DNS servers and check that entry is valid
                foreach ($dns_servers as $ds) {
                    // Check if our DNS server is valid
                    if (!is_ipaddrv4($ds) and !is_ipaddrv6($ds)) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 1007);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                }
                // Add our system DNS values to config and track the change
                $config["system"]["dnsserver"] = $dns_servers;
                $dns_ent["dnsserver"] = $dns_servers;
            }
            if ($client_params['dnsallowoverride'] === true) {
                $config["system"]["dnsallowoverride"] = "";
                $dns_ent["dnsallowoverride"] = $client_params['dnsallowoverride'];
            } elseif ($client_params['dnsallowoverride'] === false) {
                $dns_ent["dnsallowoverride"] = $client_params['dnsallowoverride'];
                unset($config["system"]["dnsallowoverride"]);
            }
            if ($client_params['dnslocalhost'] === true) {
                $config["system"]["dnslocalhost"] = "";
                $dns_ent["dnslocalhost"] = $client_params['dnslocalhost'];
            } elseif ($client_params['dnslocalhost'] === false) {
                $dns_ent["dnslocalhost"] = $client_params['dnslocalhost'];
                unset($config["system"]["dnslocalhost"]);
            }
            // Write our new hostname
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Modified system DNS servers via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            // Update a slew of backend services
            system_resolvconf_generate();
            if (isset($config['dnsmasq']['enable'])) {
                services_dnsmasq_configure();
            } elseif (isset($config['unbound']['enable'])) {
                services_unbound_configure();
            }
            send_event("service reload dns");
            filter_configure();
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully modified system DNS servers";
            $api_resp["data"] = $dns_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_dns_delete_servers() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params, $client_id;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $del_ent = [];
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['dnsserver'])) {
                $del_server = $client_params['dnsserver'];
                $curr_servers = $config["system"]["dnsserver"];
                $del_server = (!is_array($del_server)) ? array($del_server) : $del_server;
                foreach ($del_server as $ds) {
                    // Ensure our config is array
                    if (!is_array($curr_servers)) {
                        $curr_servers = array($config["system"]["dnsserver"]);
                    }
                    // Loop through each server and check for matches, delete on match
                    foreach ($curr_servers as $id => $cs) {
                        if ($ds === $cs) {
                            $del_ent[] = $ds;
                            unset($config["system"]["dnsserver"][$id]);
                        }
                    }
                }
            }
            // Write our new hostname
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Deleted system DNS servers via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            // Update a slew of backend services
            system_resolvconf_generate();
            if (isset($config['dnsmasq']['enable'])) {
                services_dnsmasq_configure();
            } elseif (isset($config['unbound']['enable'])) {
                services_unbound_configure();
            }
            send_event("service reload dns");
            filter_configure();
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted system DNS servers";
            $api_resp["data"] = $del_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_hostname_modify() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params, $client_id;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['hostname'])) {
                $hostname = $client_params['hostname'];
                $hostname = trim($hostname);
                // Check if our hostname is valid
                if (!is_hostname($hostname) or !is_unqualified_hostname($hostname)) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 1000);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                } else {
                    $config["system"]["hostname"] = $hostname;
                }
            }
            if (isset($client_params['domain'])) {
                $domain = $client_params['domain'];
                $domain = trim($domain);
                // Check if our hostname is valid
                if (!is_domain($domain)) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 1001);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                } else {
                    $config["system"]["domain"] = $domain;
                }
            }
            // Write our new hostname
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Modified system hostname via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            // Update a slew of backend services
            if (isset($hostname) or isset($domain)) {
                system_hostname_configure();
                system_hosts_generate();
                system_resolvconf_generate();
                if (isset($config['dnsmasq']['enable'])) {
                    services_dnsmasq_configure();
                } elseif (isset($config['unbound']['enable'])) {
                    services_unbound_configure();
                }
                filter_configure();
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully modified system hostname";
            $api_resp["data"] = array("hostname" => $config["system"]["hostname"], "domain" => $config["system"]["domain"]);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_dns() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system");    // Allowed privs
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $dns_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["system"])) {
                $dns_keys = ["dnsserver", "dnsallowoverride", "dnslocalhost"];
                foreach ($config["system"] as $key => $sv) {
                    if (in_array($key, $dns_keys)) {
                        $dns_array[$key] = $sv;
                    }
                }
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $dns_array = api_extended_search($dns_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $dns_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_certificates() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-certmanager");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $crt_array = array();    // Init our array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a virtual IP configuration
            if (!empty($config["cert"])) {
                $crt_array = $config["cert"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $crt_array = api_extended_search($crt_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $crt_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_certificates_add() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params, $client_id;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-certmanager");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $cert_ent = [];    // Init our configuration array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 1002);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['method'])) {
                $method = $client_params['method'];
                // Check what method was picked
                if ($method === "import") {
                    $cert = base64_decode($client_params["cert"]);
                    $key = base64_decode($client_params["key"]);
                    $active = $client_params["active"];
                    // Check if our certificate and key are valid
                    if (!strstr($cert, "BEGIN CERTIFICATE") || !strstr($cert, "END CERTIFICATE")) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 1003);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    } elseif (cert_get_publickey($cert, false) != cert_get_publickey($key, false, 'prv')) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 1004);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                    // Populate our configuration array
                    $cert_ent["refid"] = uniqid();
                    $cert_ent["type"] = "server";
                    $cert_ent["descr"] = $descr;
                    cert_import($cert_ent, $cert, $key);
                    // If user requests this cert to be active, configure it as the wc cert
                    if ($active === true) {
                        $config["system"]["webgui"]["ssl-certref"] = $cert_ent["refid"];
                    }
                }
            }
            // Write our new certificate
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added certificate via API";    // Add a change note
            $config["cert"][] = $cert_ent;    // Add our configuration
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            // Restart our webgui if user set this cert as active
            if ($active === true) {
                restart_webconfigurator();
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully added system certificate";
            $api_resp["data"] = $cert_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_certificates_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params, $client_id;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-certmanager");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $cert_del = [];    // Init our configuration array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['refid'])) {
                $refid = $client_params['refid'];
                // Loop through our certificates and find a match
                foreach ($config["cert"] as $i => $c) {
                    if ($c["refid"] === $refid) {
                        $del_id = $i;
                        break;
                    }
                }
            } elseif (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
                // Loop through our certificates and find a match
                foreach ($config["cert"] as $i => $c) {
                    if ($c["descr"] === $descr) {
                        $del_id = $i;
                        break;
                    }
                }
            } elseif (isset($client_params['id'])) {
                $del_id = $client_params['id'];
            }
            // Write our new certificate
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Deleted certificate via API";    // Add a change note
            // Check if our del_id is in our array
            if (array_key_exists($del_id, $config["cert"])) {
                $cert_del = $config["cert"][$del_id];    // Save our cert we are deleting
                // Check if our certificate is currently in use
                if ($config["system"]["webgui"]["ssl-certref"] !== $cert_del["refid"]) {
                    unset($config["cert"][$del_id]);    // Delete our cert
                    write_config(sprintf(gettext($change_note)));    // Apply our configuration change
                } else {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 1005);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted system certificate";
            $api_resp["data"] = $cert_del;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_arp() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-diagnostics-arptable");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            $arp_data = get_arp_table();
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $arp_data);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_system_arp_delete() {
    # VARIABLES
    global $err_lib, $api_resp, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-diagnostics-arptable");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $arp_cmd = "arp -d ";    // Assign the command which deletes our ARP table value
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['ip'])) {
                $ip = $client_params['ip'];
                $ip = trim($ip);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 1006);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            $del_ent = get_arp_entry("ip", $ip);    // Save our deleted ARP config
            exec($arp_cmd.$ip, $arp_del);    // Delete our ARP address
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted system ARP entry";
            $api_resp["data"] = $del_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users() {
    # VARIABLES
    global $err_lib, $config, $userindex, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-usermanager");    // Array of privileges allowing this action
    $userindex = index_users();    // Index our users
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Save input data
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
            }
            // Input validation
            $base_data = $config["system"]["user"];
            $search_failed = false;
            // If client requests an extended search
            if (isset($search)) {
                $base_data = api_extended_search($base_data, $search);
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "SEARCH ATTRIBUTES:" . PHP_EOL;
                echo var_dump($search) . PHP_EOL;
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $base_data);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_delete() {
    # VARIABLES
    global $err_lib, $config, $userindex, $api_resp, $client_id, $client_params;
    $logging_level = LOG_WARNING;    // Set our log level
    $logging_prefix = gettext("Local User Database");    // Set our logging prefix
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-usermanager");    // Array of privileges allowing this action
    $userindex = index_users();    // Index our users
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $del_user = [];    // Init our deleted user config array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {

        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['username'])) {
                $username = $client_params['username'];
                $username = trim($username);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check if our user already exists, if not exit on non-zero
            if (!array_key_exists($username, $userindex)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5001);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "USERNAME:" . PHP_EOL;
                echo var_dump($username) . PHP_EOL;
            }
            // Delete our user
            $_SESSION["Username"] = $client_id;    // Save our username to session data for logging
            $change_note = " Deleted user `" . $username . "' via API";    // Add a change note
            $index_id = $userindex[$username];    // Save our user's index ID number
            local_user_del($config["system"]["user"][$index_id]);    // Delete our user on the backend
            $del_user = $config['system']['user'][$index_id];    // Save our deleted user config
            unset($config['system']['user'][$index_id]);    // Unset our user from config
            $config['system']['user'] = array_values($config['system']['user']);    // Reindex our users
            write_config(sprintf(gettext($change_note)));    // Write our new config
            //--Check that our changes were successful-----------
            $userindex = index_users();    // Update our user index
            if (!array_key_exists($username, $userindex)) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully deleted user";
                $api_resp["data"] = $del_user;
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_delete_privs() {
    # VARIABLES
    global $err_lib, $config, $priv_list, $userindex, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set if function requires read only access
    $userindex = index_users();    // Index our users
    $req_privs = array("page-all", "page-system-usermanager-addprivs");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['username'])) {
                $username = $client_params['username'];
                $username = trim($username);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['priv'])) {
                $del_priv = $client_params['priv'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5004);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check if our user already exists, if so exit on non-zero
            $user_config =& getUserEntry($username);
            $user_privs = get_user_privileges($user_config);
            if (!array_key_exists("uid", $user_config)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5001);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Ensure our new priv is array, if it is a string create an array containing the string
            if (is_string($del_priv)) {
                $del_priv = array($del_priv);
            }
            if (is_array($del_priv)) {
                // Loop through our new priv list and check that the privs are valid
                foreach ($del_priv as $dp) {
                    if (!array_key_exists($dp, $priv_list)) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 5006);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                    if (in_array($dp, $user_config["priv"])) {
                        $user_config["priv"] = \array_diff($user_config["priv"], array($dp));
                    }
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5005);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "USERNAME:" . PHP_EOL;
                echo var_dump($username) . PHP_EOL;
                echo "USER EXISTING PRIVILEGES:" . PHP_EOL;
                echo var_dump($user_privs) . PHP_EOL;
                echo "USER NEW PRIVILEGES:" . PHP_EOL;
                echo var_dump($user_config["priv"]) . PHP_EOL;
            }
            // Delete privilege
            $_SESSION["Username"] = $client_id;    // Save our session username for logging purposes
            $change_note = " Deleted privileges for user `" . $username . "' via API";    // Add a change note
            $user_id = $userindex[$username];    // Save our user's array index ID
            local_user_set($user_config);    // Set user backend parameters
            $config["system"]["user"][$user_id] = $user_config;    // Add our new config
            write_config(sprintf(gettext($change_note)));    // Write our new config
            // Check our updated config to ensure our change was successful
            parse_config(true);    // Update our entire config
            $userindex = index_users();    // Update our user index
            $user_config =& getUserEntry($username);    // Update our user config
            $user_privs = get_user_privileges($user_config);    // Update our user privs
            // Check if our output is expected
            if ($user_config["priv"] === $user_privs) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully deleted user privilege";
                $api_resp["data"] = $del_priv;
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_delete_groups() {
    # VARIABLES
    global $err_lib, $userindex, $groupindex, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-usermanager-addprivs", "page-system-groupmanager-");   // Allowed privs
    $userindex = index_users();    // Index our users
    $groupindex = index_groups();    // Index our groups
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['username'])) {
                $username = $client_params['username'];
                $username = trim($username);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['group'])) {
                $del_groups = $client_params['group'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5007);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check if our user already exists, if so exit on non-zero
            if (!array_key_exists($username, $userindex)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5001);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Ensure our new priv is array, if it is a string create an array containing the string
            if (is_string($del_groups)) {
                $del_groups = array($del_groups);
            }
            if (is_array($del_groups)) {
                // Get our current user's groups
                $user_config = getUserEntry($username);
                $user_groups = local_user_get_groups($user_config, true);
                // Loop through our del group list and ensure it is valid
                foreach ($del_groups as $dg) {
                    if (!array_key_exists($dg, $groupindex)) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 5008);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                    if (in_array($dg, $user_groups)) {
                        $new_groups = \array_diff($user_groups, array($dg));
                    }
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5009);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "USERNAME:" . PHP_EOL;
                echo var_dump($username) . PHP_EOL;
                echo "USER GROUPS:" . PHP_EOL;
                echo var_dump($user_groups) . PHP_EOL;
                echo "USER DELETE GROUPS:" . PHP_EOL;
                echo var_dump($del_groups) . PHP_EOL;
            }
            // Delete our group memberships
            $_SESSION["Username"] = $client_id;    // Save our session username for logging purposes
            $change_note = " Deleted group membership for user `" . $username . "' via API";    // Add a change note
            $user_id = $userindex[$username];    // Save our user's array index ID
            local_user_set_groups($user_config, $new_groups);    // Set our user's groups
            local_user_set($user_config);    // Reset our user
            write_config(sprintf(gettext($change_note)));    // Write our config
            // Update our current user's groups
            $user_config = getUserEntry($username);
            $user_groups = local_user_get_groups($user_config, true);
            if (array_diff($del_groups, $user_groups)) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully deleted group membership";
                $api_resp["data"] = $del_groups;
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_modify() {
    # VARIABLES
    global $err_lib, $userindex, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-usermanager");    // Array of privileges allowing this action
    $userindex = index_users();    // Index our users
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['username'])) {
                $username = $client_params['username'];
                $username = trim($username);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['password'])) {
                $password = $client_params['password'];
                $password = trim($password);
            }
            if (array_key_exists("disabled", $client_params)) {
                $disabled = true;
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
                $descr = trim($descr);
            }
            if (isset($client_params['expires'])) {
                $expires = $client_params['expires'];
                $expires = trim($expires);
                //$expires = new DateTime($expires);
            }
            if (isset($client_params['authorizedkeys'])) {
                $authorizedkeys = $client_params['authorizedkeys'];
                $authorizedkeys = trim($authorizedkeys);
            }
            if (isset($client_params['ipsecpsk'])) {
                $ipsecpsk = $client_params['ipsecpsk'];
                $ipsecpsk = trim($ipsecpsk);
            }
            // Check that our user already exists
            $user_config =& getUserEntry($username);
            if (!array_key_exists("uid", $user_config)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5001);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "USERNAME:" . PHP_EOL;
                echo var_dump($username) . PHP_EOL;
                echo "PASSWORD:" . PHP_EOL;
                echo var_dump(isset($password)) . PHP_EOL;
                echo "DISABLE:" . PHP_EOL;
                echo var_dump($disabled) . PHP_EOL;
                echo "FULL NAME:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "EXPIRES:".PHP_EOL;
                echo var_dump($expires).PHP_EOL;
                echo "AUTH KEY:" . PHP_EOL;
                echo var_dump($authorizedkeys) . PHP_EOL;
                echo "IPSEC KEY:" . PHP_EOL;
                echo var_dump($ipsecpsk) . PHP_EOL;
            }
            // Modify our user
            $_SESSION["Username"] = $client_id;    // Save our username to session for logging
            $change_note = " Modified user `" . $username . "' via API";    // Add a change note
            $prev_config = $user_config;    // Save a copy of our previous config for comparison
            $user_config["scope"] = "user";    // Set our new user's system scope
            // Check for our optional parameters
            if (!empty(($password))) {
                local_user_set_password($user_config, $password);  // Set our new user's password
            }
            if (!empty($descr)) {
                $user_config["descr"] = $descr;    // Update our user's full name
            }
            if (!empty($expires)) {
                $user_config["expires"] = $expires;    // Update our user's expiration date
            }
            if (!empty($authorizedkeys)) {
                $user_config["authorizedkeys"] = $authorizedkeys;    // Update our user's authorized keys
            }
            if (!empty($ipsecpsk)) {
                $user_config["ipsecpsk"] = $ipsecpsk;    // Update our user's IPsec pre-shared key
            }
            if ($disabled === true) {
                $user_config["disabled"] = "";    // Update our user's disabled value if not false
            } else {
                unset($user_config["disabled"]);    // Unset our disabled value if not requested
            }
            // Update our user's configuration to our user list, then set our configuration and write config
            local_user_set($user_config);
            write_config(sprintf(gettext($change_note)));
            // Check our updated config to ensure our change was successful
            unset($user_config);
            $userindex = index_users();    // Update our user index
            $user_config =& getUserEntry($username);    // Update our user config
            if ($user_config !== $prev_config) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully modified user";
                $api_resp["data"] = $user_config;
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_add() {
    # VARIABLES
    global $err_lib, $config, $userindex, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-usermanager");    // Array of privileges allowing this action
    $userindex = index_users();    // Index our users
    $uid = $config["system"]["nextuid"];    // Save our next UID
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['username'])) {
                $username = $client_params['username'];
                $username = trim($username);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['password'])) {
                $password = $client_params['password'];
                $password = trim($password);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5003);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if ($client_params["disabled"] === true) {
                $disabled = true;
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
                $descr = trim($descr);
            }
            if (isset($client_params['expires'])) {
                $expires = $client_params['expires'];
                $expires = trim($expires);
                $expires = new DateTime($expires);
            }
            if (isset($client_params['authorizedkeys'])) {
                $authorizedkeys = $client_params['authorizedkeys'];
                $authorizedkeys = trim($authorizedkeys);
            }
            if (isset($client_params['ipsecpsk'])) {
                $ipsecpsk = $client_params['ipsecpsk'];
                $ipsecpsk = trim($ipsecpsk);
            }

            // Check if our user already exists, if so exit on non-zero
            $user_config =& getUserEntry($username);
            if (array_key_exists("uid", $user_config)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5002);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "USERNAME:" . PHP_EOL;
                echo var_dump($username) . PHP_EOL;
                echo "DISABLE:" . PHP_EOL;
                echo var_dump($disabled) . PHP_EOL;
                echo "FULL NAME:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "EXPIRES:".PHP_EOL;
                echo var_dump($expires).PHP_EOL;
                echo "AUTH KEY:" . PHP_EOL;
                echo var_dump($authorizedkeys) . PHP_EOL;
                echo "IPSEC KEY:" . PHP_EOL;
                echo var_dump($ipsecpsk) . PHP_EOL;
            }
            // Add our user
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added user `" . $username . "' via API";    // Add a change note
            local_user_set_password($user_config, $password);  // Set our new user's password
            $user_config["scope"] = "user";    // Set our new user's system scope
            $user_config["uid"] = $uid;    // Set our new user's UID
            $user_config["name"] = $username;    // Set our new user's username
            // Check for our optional parameters
            if (!empty($descr)) {
                $user_config["descr"] = $descr;    // Set our new user's full name
            }
            if (!empty($expires)) {
                $user_config["expires"] = $expires;    // Set our new user's expiration date
            }
            if (!empty($authorizedkeys)) {
                $user_config["authorizedkeys"] = $authorizedkeys;    // Set our new user's authorized keys
            }
            if (!empty($ipsecpsk)) {
                $user_config["ipsecpsk"] = $ipsecpsk;    // Set our new user's IPsec pre-shared key
            }
            if ($disabled === true) {
                $user_config["disabled"] = "";    // Set our new user's disabled value if not false
            }
            $user_config["priv"] = array(null);    // Default our privs to empty array
            // Append our new user's configuration to our user list, then set our configuration and write config
            $config['system']['user'] = array_merge($config['system']['user'], array($user_config));
            $config["system"]["nextuid"] = strval(intval($uid) + 1);   // Increase our next UID
            local_user_set($user_config);
            write_config(sprintf(gettext($change_note)));
            // Check our updated config to ensure our change was successful
            $userindex = index_users();    // Update our user index
            $user_config =& getUserEntry($username);    // Update our user config
            // Check that our user is now in the user configuration
            if (array_key_exists("uid", $user_config)) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully added user";
                $api_resp["data"] = $user_config;
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_add_privs() {
    # VARIABLES
    global $err_lib, $config, $priv_list, $userindex, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-usermanager-addprivs");    // Array of privileges allowing this action
    $userindex = index_users();    // Index our users
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['username'])) {
                $username = $client_params['username'];
                $username = trim($username);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['priv'])) {
                $new_priv = $client_params['priv'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5004);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check if our user already exists, if so exit on non-zero
            $user_config =& getUserEntry($username);
            $user_privs = get_user_privileges($user_config);
            if (!array_key_exists("uid", $user_config)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5001);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Ensure our new priv is array, if it is a string create an array containing the string
            if (is_string($new_priv)) {
                $new_priv = array($new_priv);
            }
            if (is_array($new_priv)) {
                // Loop through our new priv list and check that the privs are valid
                foreach ($new_priv as $np) {
                    if (!array_key_exists($np, $priv_list)) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 5006);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                    if (in_array($np, $user_privs)) {
                        $new_priv = \array_diff($new_priv, array($np));
                    }
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5005);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "USERNAME:" . PHP_EOL;
                echo var_dump($username) . PHP_EOL;
                echo "USER EXISTING PRIVILEGES:" . PHP_EOL;
                echo var_dump($user_privs) . PHP_EOL;
                echo "USER NEW PRIVILEGES:" . PHP_EOL;
                echo var_dump($new_priv) . PHP_EOL;
            }
            // Add debug data if requested
            $_SESSION["Username"] = $client_id;    // Save our session username for logging purposes
            $change_note = " Modified privileges for user `" . $username . "' via API";    // Add a change note
            $user_id = $userindex[$username];    // Save our user's array index ID
            $user_config["priv"] = array_merge($user_config["priv"], $new_priv);    // Set our user's new permissions
            local_user_set($user_config);    // Set user backend parameters
            $config["system"]["user"][$user_id] = $user_config;    // Add our new config
            write_config(sprintf(gettext($change_note)));    // Write our new config
            // Check our updated config to ensure our change was successful
            parse_config(true);    // Update our entire config
            $userindex = index_users();    // Update our user index
            $user_config =& getUserEntry($username);    // Update our user config
            $user_privs = get_user_privileges($user_config);    // Update our user privs
            if ($user_config["priv"] === $user_privs) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully added user privilege";
                $api_resp["data"] = $new_priv;
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_add_groups() {
    # VARIABLES
    global $err_lib, $userindex, $groupindex, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-usermanager-addprivs", "page-system-groupmanager-");   // Allowed privs
    $userindex = index_users();    // Index our users
    $groupindex = index_groups();    // Index our groups
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['username'])) {
                $username = $client_params['username'];
                $username = trim($username);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5000);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['group'])) {
                $new_groups = $client_params['group'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5007);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;

            }
            // Check if our user already exists, if so exit on non-zero
            if (!array_key_exists($username, $userindex)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5001);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Ensure our new priv is array, if it is a string create an array containing the string
            if (is_string($new_groups)) {
                $new_groups = array($new_groups);
            }
            if (is_array($new_groups)) {
                // Get our current user's groups
                $user_config = getUserEntry($username);
                $user_groups = local_user_get_groups($user_config, true);
                // Loop through our new priv list and check that the privs are valid
                foreach ($new_groups as $ng) {
                    if (!array_key_exists($ng, $groupindex)) {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 5008);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                    if (in_array($ng, $user_groups)) {
                        $new_groups = \array_diff($new_groups, array($ng));
                    }
                }
                // Add our new groups to our existing groups
                $new_groups = array_merge($user_groups, $new_groups);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5009);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "USERNAME:" . PHP_EOL;
                echo var_dump($username) . PHP_EOL;
                echo "USER GROUPS:" . PHP_EOL;
                echo var_dump($user_groups) . PHP_EOL;
                echo "USER NEW GROUPS:" . PHP_EOL;
                echo var_dump($new_groups) . PHP_EOL;
            }
            // Add our group memberships
            $_SESSION["Username"] = $client_id;    // Save our session username for logging purposes
            $change_note = " Added group membership for user `" . $username . "' via API";    // Add a change note
            $user_id = $userindex[$username];    // Save our user's array index ID
            $user_uid = $user_config["uid"];    // Save our user's UID
            local_user_set_groups($user_config, $new_groups);    // Set our user's groups
            local_user_set($user_config);    // Reset our user
            write_config(sprintf(gettext($change_note)));    // Write our config
            // Update our current user's groups
            $user_config = getUserEntry($username);
            $user_groups = local_user_get_groups($user_config, true);
            if (!array_diff($new_groups, $user_groups)) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully added group memberships";
                $api_resp["data"] = $new_groups;
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_authservers() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-authservers");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $as_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["system"]["authserver"])) {
                $as_array = $config["system"]["authserver"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $as_array = api_extended_search($as_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $as_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_authservers_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-user-authserver");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $del_server = array();    // Init our deleted auth server config
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5010);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "AUTH SERVER NAME TO DELETE:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
            }
            // Loop through our servers and check that this server exists
            foreach ($config["system"]["authserver"] as $asid => $asc) {
                if ($name === $asc["name"]) {
                    $del_server = $config["system"]["authserver"][$asid];    // Save our config before deleting
                    unset($config["system"]["authserver"][$asid]);    // Remove our config
                    // Check if this auth server is our default, if so revert to local database
                    if ($name === $config["system"]["webgui"]["authmode"]) {
                        unset($config["system"]["webgui"]["authmode"]);
                    }
                }
            }
            // Add our auth server
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Deleted authentication server via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted user authentication server";
            $api_resp["data"] = $del_server;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_authservers_radius() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-authservers");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $as_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["system"]["authserver"])) {
                foreach ($config["system"]["authserver"] as $a) {
                    if ($a["type"] === "radius") {
                        $as_array[] = $a;
                    }
                }
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $as_array = api_extended_search($as_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $as_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_authservers_radius_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-user-authserver");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $del_server = array();    // Init our deleted auth server config
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5010);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "AUTH SERVER NAME TO DELETE:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
            }
            // Loop through our servers and check that this server exists and is a RADIUS server
            foreach ($config["system"]["authserver"] as $asid => $asc) {
                if ($name === $asc["name"] and $asc["type"] === "radius") {
                    $del_server = $config["system"]["authserver"][$asid];    // Save our config before deleting
                    unset($config["system"]["authserver"][$asid]);    // Remove our config
                    // Check if this auth server is our default, if so revert to local database
                    if ($name === $config["system"]["webgui"]["authmode"]) {
                        unset($config["system"]["webgui"]["authmode"]);
                    }
                }
            }
            // Add our auth server
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Deleted authentication server via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted RADIUS authentication server";
            $api_resp["data"] = $del_server;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_authservers_ldap() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-authservers");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $as_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["system"]["authserver"])) {
                foreach ($config["system"]["authserver"] as $a) {
                    if ($a["type"] === "ldap") {
                        $as_array[] = $a;
                    }
                }
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $as_array = api_extended_search($as_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $as_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_authservers_ldap_add() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params, $pf_ver_num;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-user-authserver");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $ldap_count = count($config["system"]["authserver"]);    // Save our total number of auth servers
    $ldap_ent = ["refid" => uniqid(), "type" => "ldap"];    // Init our new LDAP config
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $ldap_ent["name"] = sanitize_str($client_params['name']);
                if (is_authentication_server($ldap_ent["name"])) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5010);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                while (true) {
                    $ldap_ent["name"] = "LDAP_AUTHSERVER_" . strval($ldap_count);
                    // Check if auto name is already used
                    if (is_authentication_server($ldap_ent["name"])) {
                        $ldap_count++;
                    } else {
                        break;
                    }
                }
            }
            if (isset($client_params['host'])) {
                $ldap_ent["host"] = $client_params['host'];
                // Check if our hostname is valid
                if (!is_hostname($ldap_ent["host"]) and !is_ipaddrv4($ldap_ent["host"])) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5012);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5011);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (($client_params['ldap_port'])) {
                $ldap_ent["ldap_port"] = strval($client_params['ldap_port']);
                // Check if our hostname is valid
                if (!is_port($ldap_ent["ldap_port"])) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5014);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5013);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ldap_urltype'])) {
                // Assign default config values
                $allowed_url_types = array(
                    "standard" => "TCP - Standard",
                    "starttls" => "TCP - STARTTLS",
                    "encrypted" => "SSL - Encrypted"
                );
                // Accommodate config value changes for pfSense 2.5_
                if ($pf_ver_num >= 250) {
                    $allowed_url_types = array(
                        "standard" => "Standard TCP",
                        "starttls" => "STARTTLS Encrypted",
                        "encrypted" => "SSL/TLS Encrypted"
                    );
                }
                // Check if our URL type is allowed
                if (!array_key_exists($client_params["ldap_urltype"], $allowed_url_types)) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5016);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
                $ldap_ent["ldap_urltype"] = $allowed_url_types[$client_params['ldap_urltype']];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5015);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ldap_protver'])) {
                $ldap_ent["ldap_protver"] = intval($client_params['ldap_protver']);
                // Check if our LDAP version is valid
                $allowed_ldap_ver = array(2, 3);    // Array of allowed LDAP protocol versions
                if (!in_array($ldap_ent["ldap_protver"], $allowed_ldap_ver)) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5017);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $ldap_ent["ldap_protver"] = 3;
            }
            if (isset($client_params['ldap_timeout'])) {
                $ldap_ent["ldap_timeout"] = intval($client_params['ldap_timeout']);
            } else {
                $ldap_ent["ldap_timeout"] = 25;    // Assign default if not specified
            }
            if (isset($client_params['ldap_scope'])) {
                $ldap_ent["ldap_scope"] = $client_params['ldap_scope'];
                // Check if our LDAP scope is valid
                $allowed_ldap_scopes = array("one", "subtree");    // Array of allowed LDAP scopes
                if (!in_array($ldap_ent["ldap_scope"], $allowed_ldap_scopes)) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5019);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5018);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ldap_basedn'])) {
                $ldap_ent["ldap_basedn"] = strval($client_params['ldap_basedn']);
            } else {
                $ldap_ent["ldap_basedn"] = "";
            }
            if (isset($client_params['ldap_authcn'])) {
                $ldap_ent["ldap_authcn"] = strval($client_params['ldap_authcn']);
            } else {
                $ldap_ent["ldap_authcn"] = "";
            }
            if (isset($client_params['ldap_extended_query'])) {
                $ldap_ent["ldap_extended_enabled"] = "yes";
                $ldap_ent["ldap_extended_query"] = strval($client_params['ldap_extended_query']);
            }
            if (!isset($client_params['ldap_binddn']) and !isset($client_params['ldap_bindpw'])) {
                $bind_anon = true;
            }
            if (!$bind_anon) {
                if (isset($client_params['ldap_binddn'])) {
                    $ldap_ent["ldap_binddn"] = strval($client_params['ldap_binddn']);
                } else {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5020);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
                if (isset($client_params['ldap_bindpw'])) {
                    $ldap_ent["ldap_bindpw"] = strval($client_params['ldap_bindpw']);
                } else {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 5021);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            if (isset($client_params['ldap_attr_user'])) {
                $ldap_ent["ldap_attr_user"] = strval($client_params['ldap_attr_user']);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5022);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ldap_attr_group'])) {
                $ldap_ent["ldap_attr_group"] = strval($client_params['ldap_attr_group']);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5023);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ldap_attr_member'])) {
                $ldap_ent["ldap_attr_member"] = strval($client_params['ldap_attr_member']);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5024);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ldap_attr_groupobj'])) {
                $ldap_ent["ldap_rfc2307"] = "";    // Enable RFC2307 mode
                $ldap_ent["ldap_attr_groupobj"] = strval($client_params['ldap_attr_groupobj']);
            }
            if ($client_params['ldap_utf8'] === true) {
                $ldap_ent["ldap_utf8"] = "";    // Enable UTF8 LDAP parameters
            }
            if ($client_params['ldap_nostrip_at'] === true) {
                $ldap_ent["ldap_nostrip_at"] = "";    // Disable LDAP username alterations (@)
            }
            if ($client_params['active'] === true) {
                $set_as_default = true;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "NAME:" . PHP_EOL;
                echo var_dump($ldap_ent["name"]) . PHP_EOL;
                echo "HOSTNAME OR IP:" . PHP_EOL;
                echo var_dump($ldap_ent["host"]) . PHP_EOL;
                echo "REMOTE LDAP PORT:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_port"]) . PHP_EOL;
                echo "TRANSPORT METHOD:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_urltype"]) . PHP_EOL;
                echo "LDAP PROTOCOL VERSION:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_protver"]) . PHP_EOL;
                echo "LDAP TIMEOUT:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_timeout"]) . PHP_EOL;
                echo "LDAP SEARCH SCOPE:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_scope"]) . PHP_EOL;
                echo "BIND ANONYMOUSLY:" . PHP_EOL;
                echo var_dump($bind_anon) . PHP_EOL;
                echo "LDAP BASE DN:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_basedn"]) . PHP_EOL;
                echo "LDAP AUTH CONTAINER:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_authcn"]) . PHP_EOL;
                echo "LDAP EXTENDED QUERY:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_extended_query"]) . PHP_EOL;
                echo "LDAP BIND DN:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_binddn"]) . PHP_EOL;
                echo "LDAP BIND PASSWORD:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_bindpw"]) . PHP_EOL;
                echo "LDAP USER ATTRITBUTE:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_attr_user"]) . PHP_EOL;
                echo "LDAP GROUP ATTRITBUTE:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_attr_group"]) . PHP_EOL;
                echo "LDAP MEMBER ATTRIBUTE:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_attr_member"]) . PHP_EOL;
                echo "LDAP GROUP OBJECT:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_attr_groupobj"]) . PHP_EOL;
                echo "ENABLE RFC2307:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_rfc2307"]) . PHP_EOL;
                echo "ENABLE UTF-8 ENCODING:" . PHP_EOL;
                echo var_dump($ldap_ent["ldap_utf8"]) . PHP_EOL;
            }
            // Add our auth server
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added authentication server via API";    // Add a change note
            $config["system"]["authserver"][] = $ldap_ent;    // Add our new configuration
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            // Check if clients wants to set this as default auth server
            if ($set_as_default) {
                $change_note = " Added authentication server via API";    // Add a change note
                $config['system']['webgui']['authmode'] = $ldap_ent["name"];    // Add default auth server
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            }
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "");
            $api_resp["message"] = "Successfully added LDAP authentication server";
            $api_resp["data"] = $ldap_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_users_authservers_ldap_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-user-authserver");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $del_server = array();    // Init our deleted auth server config
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 5011);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "AUTH SERVER NAME TO DELETE:" . PHP_EOL;
                echo var_dump($name) . PHP_EOL;
            }
            // Loop through our servers and check that this server exists and is an LDAP server
            foreach ($config["system"]["authserver"] as $asid => $asc) {
                if ($name === $asc["name"] and $asc["type"] === "ldap") {
                    $del_server = $config["system"]["authserver"][$asid];    // Save our config before deleting
                    unset($config["system"]["authserver"][$asid]);    // Remove our config
                    // Check if this auth server is our default, if so revert to local database
                    if ($name === $config["system"]["webgui"]["authmode"]) {
                        unset($config["system"]["webgui"]["authmode"]);
                    }
                }
            }
            // Add our auth server
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Deleted authentication server via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted LDAP authentication server";
            $api_resp["data"] = $del_server;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_routing_gateways() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-gateways");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $gw_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["gateways"]["gateway_item"])) {
                $gw_array = $config["gateways"]["gateway_item"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $gw_array = api_extended_search($gw_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $gw_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_ntpd_start() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_ntpd_stop() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_ntpd_restart() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (READ)
        if ($http_method === 'GET') {
            if (isset($client_params['name'])) {
                $name = $client_params['name'];
            }
            $service_list = get_services();    // Stop our service
            // Loop through our service list and add our service status
            foreach ($service_list as $key => $srvc) {
                $s_status = get_service_status($srvc);
                // Check if service is started
                if ($s_status === true) {
                    $service_list[$key]["status"] = "running";
                } elseif ($s_status === false) {
                    $service_list[$key]["status"] = "stopped";
                } else {
                    $service_list[$key]["status"] = "unknown";
                }
                // Check if user requested only one service, if so remove unmatched services
                if (isset($name) and $name !== $srvc["name"]) {
                    unset($service_list[$key]);
                }
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $service_list);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_sshd() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-advanced-admin");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $ssh_array = array();    // Init our return array
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["system"]["ssh"])) {
                $ssh_array = $config["system"]["ssh"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $ssh_array = api_extended_search($ssh_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $ssh_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_sshd_modify() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-system-advanced-admin");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $allowed_auth_types = array("disabled", "enabled", "both");    // Array of allowed auth types
    $err_found = false;    // Track errors
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        $api_resp = array("status" => "bad request", "code" => 400);
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['enable'])) {
                $enable = $client_params['enable'];
                if ($enable === true) {
                    $config["system"]["ssh"]["enable"] = "enabled";
                } elseif ($enable === false) {
                    unset($config["system"]["ssh"]["enable"]);
                } else {
                    $api_resp["return"] = 2000;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            if (isset($client_params["sshdkeyonly"])) {
                $sshdkeyonly = $client_params["sshdkeyonly"];
                // Check if our auth type is valid
                if (in_array($sshdkeyonly, $allowed_auth_types)) {
                    if ($sshdkeyonly === "disabled") {
                        unset($config["system"]["ssh"]["sshdkeyonly"]);
                    } else {
                        $config["system"]["ssh"]["sshdkeyonly"] = $sshdkeyonly;
                    }
                } else {
                    $api_resp["return"] = 2001;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            if (isset($client_params['sshdagentforwarding'])) {
                $sshdagentforwarding = $client_params['sshdagentforwarding'];
                if ($sshdagentforwarding === true) {
                    $config["system"]["ssh"]["sshdagentforwarding"] = "enabled";
                } elseif ($sshdagentforwarding === false) {
                    unset($config["system"]["ssh"]["sshdagentforwarding"]);
                } else {
                    $api_resp["return"] = 2002;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            if (isset($client_params['port'])) {
                $port = strval($client_params['port']);
                // Convert string to array
                if (is_port($port)) {
                    $config["system"]["ssh"]["port"] = $port;
                } else {
                    $api_resp["return"] = 2003;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "ENABLE SSHD:" . PHP_EOL;
                echo var_dump($enable) . PHP_EOL;
                echo "SSHD KEY ONLY:" . PHP_EOL;
                echo var_dump($sshdkeyonly) . PHP_EOL;
                echo "SSHD AGENT FORWARDING:" . PHP_EOL;
                echo var_dump($sshdagentforwarding) . PHP_EOL;
                echo "SSHD PORT:" . PHP_EOL;
                echo var_dump($port) . PHP_EOL;
            }
            // Modify our SSHD config
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Modified sshd configuration via API";    // Add a change note
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            // Check that something was changed before altering service
            if (isset($enable) or isset($port) or isset($sshdagentforwarding) or isset($sshdkeyonly)) {
                killbyname("sshd");    // Kill SSHD
                log_error(gettext("secure shell configuration has changed. Stopping sshd."));
                if ($config['system']['ssh']['enable']) {
                    log_error(gettext("secure shell configuration has changed. Restarting sshd."));
                    send_event("service restart sshd");
                }
            }
            // Loop through each alias and see if our alias was added successfully
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully modified sshd configuration";
            $api_resp["data"] = $config["system"]["ssh"];
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_sshd_start() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_sshd_stop() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_sshd_restart() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_syslogd_start() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_syslogd_stop() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_syslogd_restart() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_start() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            // Check if user specified which services to set
            if (isset($client_params['service'])) {
                $service = is_array($client_params['service']) ? $client_params['service'] : [$client_params['service']];
            }
            $service_list = get_services();
            $services_set = [];
            // Loop through our service list and add our service status
            foreach ($service_list as $key => $srvc) {
                // Check if this service should be set
                if (!isset($service) or in_array($srvc["name"], $service)) {
                    $set_service = service_control_start($srvc["name"], []);    // Start our service
                    $services_set[] = $srvc["name"];
                }
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "Successfully started all services");
            $api_resp["data"] = $services_set;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_stop() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            // Check if user specified which services to set
            if (isset($client_params['service'])) {
                $service = is_array($client_params['service']) ? $client_params['service'] : [$client_params['service']];
            }
            $service_list = get_services();
            $services_set = [];
            // Loop through our service list and add our service status
            foreach ($service_list as $key => $srvc) {
                // Check if this service should be set
                if (!isset($service) or in_array($srvc["name"], $service)) {
                    $set_service = service_control_stop($srvc["name"], []);    // Stop our service
                    $services_set[] = $srvc["name"];
                }
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "Successfully stopped all services");
            $api_resp["data"] = $services_set;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_restart() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            // Check if user specified which services to set
            if (isset($client_params['service'])) {
                $service = is_array($client_params['service']) ? $client_params['service'] : [$client_params['service']];
            }
            $service_list = get_services();
            $services_set = [];
            // Loop through our service list and add our service status
            foreach ($service_list as $key => $srvc) {
                // Check if this service should be set
                if (!isset($service) or in_array($srvc["name"], $service)) {
                    $set_service = service_control_restart($srvc["name"], []);    // Restart our service
                    $services_set[] = $srvc["name"];
                }
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "Successfully restarted all services");
            $api_resp["data"] = $services_set;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_dhcpd_start() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_dhcpd_stop() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_dhcpd_restart() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_dpinger_start() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_dpinger_stop() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_dpinger_restart() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-services-dnsresolver");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            // Check that we have a configuration
            if (!empty($config["unbound"])) {
                $unbound_array = $config["unbound"];
            }
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $unbound_array = api_extended_search($unbound_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $unbound_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound_delete_hosts() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-services-dnsresolver-edithost");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $del_mode = "";
    // FUNCTIONS------------------------------------------------------------------------------------------------------------
    function host_cmp($a, $b) {
        return strcasecmp($a['host'], $b['host']);
    }
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (DELETE)
        if ($http_method === 'POST') {
            if (isset($client_params['host'])) {
                $hostname = $client_params['host'];
                $hostname = trim($hostname);
                $h_mode = true;
            }
            if (isset($client_params['domain'])) {
                $domain = $client_params['domain'];
                $domain = trim($domain);
                $d_mode = true;
            }
            if (isset($client_params['ip'])) {
                $ipaddr = $client_params['ip'];
                $ipaddr = trim($ipaddr);
                $i_mode = true;
            }
            if ($client_params['aliases'] === true) {
                $a_mode = true;
            }
            if ($client_params['apply'] === true) {
                $apply = $client_params['apply'];
            }
            // Determine criteria for deletion
            if ($h_mode and !$d_mode and !$i_mode) {
                $del_mode = "h";
            } elseif ($h_mode and $d_mode and !$i_mode) {
                $del_mode = "hd";
            } elseif ($h_mode and !$d_mode and $i_mode) {
                $del_mode = "hi";
            } elseif ($h_mode and $d_mode and $i_mode) {
                $del_mode = "hdi";
            } elseif (!$h_mode and $d_mode and !$i_mode) {
                $del_mode = "d";
            } elseif (!$h_mode and $d_mode and $i_mode) {
                $del_mode = "di";
            } elseif (!$h_mode and !$d_mode and $i_mode) {
                $del_mode = "i";
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 45);
                $api_resp["message"] = "host override deletion criteria not met";
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "HOSTNAME:" . PHP_EOL;
                echo var_dump($hostname) . PHP_EOL;
                echo "DOMAIN:" . PHP_EOL;
                echo var_dump($domain) . PHP_EOL;
                echo "IP ADDRESS:" . PHP_EOL;
                echo var_dump($ipaddr) . PHP_EOL;
                echo "ALIASES:" . PHP_EOL;
                echo var_dump($a_mode) . PHP_EOL;
                echo "MODE:" . PHP_EOL;
                echo var_dump($del_mode) . PHP_EOL;
                echo "APPLY:" . PHP_EOL;
                echo var_dump($apply) . PHP_EOL;
            }
            // Check that our configuration is a list and loop through each item, otherwise return ok resp
            if (array_key_exists("hosts", $config["unbound"]) and is_array($config["unbound"]["hosts"])) {
                $del_list = array("hosts" => array(), "aliases" => array());    // List of deleted items
                $hosts_conf = &$config["unbound"]["hosts"];    // Current Unbound host overrides
                $h_count = 0;    // Define counter for our hosts loop
                foreach ($hosts_conf as $he) {
                    // Check aliases for match if alias mode
                    if ($a_mode and is_array($he["aliases"])) {
                        $a_count = 0;    // Define counter for our aliases loop
                        // Loop through aliases to check for matches
                        foreach ($he["aliases"]["item"] as $ae) {
                            if ($del_mode === "h") {
                                if ($hostname === $ae["host"]) {
                                    unset($hosts_conf[$h_count]["aliases"]["item"][$a_count]);
                                    $del_list["aliases"][] = $ae["host"].".".$ae["domain"];
                                }
                            } elseif ($del_mode === "d") {
                                if ($domain === $ae["domain"]) {
                                    unset($hosts_conf[$h_count]["aliases"]["item"][$a_count]);
                                    $del_list["aliases"][] = $ae["host"].".".$ae["domain"];
                                }
                            } elseif ($del_mode === "hd") {
                                if ($hostname === $ae["host"] and $domain === $ae["domain"]) {
                                    unset($hosts_conf[$h_count]["aliases"]["item"][$a_count]);
                                    $del_list["aliases"][] = $ae["host"].".".$ae["domain"];
                                }
                            }
                            // If all aliases were removed, restore aliases key to empty string
                            if (empty($hosts_conf[$h_count]["aliases"]["item"])) {
                                $hosts_conf[$h_count]["aliases"] = "";
                            }
                            // Increase our alias counter
                            $a_count++;
                        }
                    }
                    // Check parent host entries
                    if ($del_mode === "h") {
                        if ($hostname === $he["host"]) {
                            unset($hosts_conf[$h_count]);
                            $del_list["hosts"][] = $he["host"].".".$he["domain"];
                        }
                    } elseif ($del_mode === "d") {
                        if ($domain === $he["domain"]) {
                            unset($hosts_conf[$h_count]);
                            $del_list["hosts"][] = $he["host"].".".$he["domain"];
                        }
                    } elseif ($del_mode === "i") {
                        if ($ipaddr === $he["ip"]) {
                            unset($hosts_conf[$h_count]);
                            $del_list["hosts"][] = $he["host"].".".$he["domain"];
                        }
                    } elseif ($del_mode === "hd") {
                        if ($hostname === $he["host"] and $domain === $he["domain"]) {
                            unset($hosts_conf[$h_count]);
                            $del_list["hosts"][] = $he["host"].".".$he["domain"];
                        }
                    } elseif ($del_mode === "hi") {
                        if ($hostname === $he["host"] and $ipaddr === $he["ip"]) {
                            unset($hosts_conf[$h_count]);
                            $del_list["hosts"][] = $he["host"].".".$he["domain"];
                        }
                    } elseif ($del_mode === "di") {
                        if ($domain === $he["domain"] and $ipaddr === $he["ip"]) {
                            unset($hosts_conf[$h_count]);
                            $del_list["hosts"][] = $he["host"].".".$he["domain"];
                        }
                    } elseif ($del_mode === "hdi") {
                        if ($hostname === $he["host"] and $domain === $he["domain"] and $ipaddr === $he["ip"]) {
                            unset($hosts_conf[$h_count]);
                            $del_list["hosts"][] = $he["host"].".".$he["domain"];
                        }
                    }
                    // Increase our host counter
                    $h_count++;
                }
                // Sort and write our new configuration
                $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                $change_note = " Deleted DNS Resolver host override via API";    // Add a change note
                usort($hosts_conf, "strcmp");
                $config["unbound"]["hosts"] = $hosts_conf;
                write_config(sprintf(gettext($change_note)));
                mark_subsystem_dirty("unbound");
                # If user requests immediately application
                if ($apply === true) {
                    $applied = unbound_reload_config();
                }
                // Return success if our function was successful
                if ($applied === true or $apply !== true) {
                    $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                    $api_resp["message"] = "host override deleted";
                    $api_resp["data"] = $del_list;
                    return $api_resp;
                } else {
                    $api_resp = array("status" => "server error", "code" => 500, "return" => 4, "message" => "process fail");
                    return $api_resp;
                }
            } else {
                //to do
                return;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound_modify_hosts() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-services-dnsresolver-edithost");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    // FUNCTIONS------------------------------------------------------------------------------------------------------------
    function host_cmp($a, $b) {
        return strcasecmp($a['host'], $b['host']);
    }
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            if (isset($client_params['host'])) {
                $hostname = $client_params['host'];
                $hostname = trim($hostname);
                $h_mode = true;
            }
            if (isset($client_params['new_host'])) {
                $new_hostname = $client_params['new_host'];
                $new_hostname = trim($new_hostname);
            }
            if (isset($client_params['domain'])) {
                $domain = $client_params['domain'];
                $domain = trim($domain);
            } elseif ($h_mode) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2005);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['new_domain'])) {
                $new_domain = $client_params['new_domain'];
                $new_domain = trim($new_domain);
            }
            if (isset($client_params['ip']) and !$h_mode) {
                $ipaddr = $client_params['ip'];
                $ipaddr = trim($ipaddr);
                $i_mode = true;
            }
            if (isset($client_params['new_ip'])) {
                $new_ipaddr = $client_params['new_ip'];
                $new_ipaddr = trim($new_ipaddr);
            } elseif ($i_mode) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2012);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            }
            if (isset($client_params['aliases'])) {
                $aliases = $client_params['aliases'];
            }
            if ($client_params['apply'] === true) {
                $apply = $client_params['apply'];
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "HOSTNAME:" . PHP_EOL;
                echo var_dump($hostname) . PHP_EOL;
                echo "DOMAIN:" . PHP_EOL;
                echo var_dump($domain) . PHP_EOL;
                echo "IP ADDRESS:" . PHP_EOL;
                echo var_dump($ipaddr) . PHP_EOL;
                echo "NEW HOSTNAME:" . PHP_EOL;
                echo var_dump($new_hostname) . PHP_EOL;
                echo "NEW DOMAIN:" . PHP_EOL;
                echo var_dump($new_domain) . PHP_EOL;
                echo "NEW IP ADDRESS:" . PHP_EOL;
                echo var_dump($new_ipaddr) . PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "ALIASES:" . PHP_EOL;
                echo var_dump($aliases) . PHP_EOL;
                echo "APPLY:" . PHP_EOL;
                echo var_dump($aliases) . PHP_EOL;
            }
            // Validate our input against our exist configuration
            if (unbound_host_override_exists($hostname, $domain) or $i_mode) {
                $hosts_conf = &$config["unbound"]["hosts"];    // Current Unbound host overrides
                $h_count = 0;    // Assign a loop counter
                $update_list = array();    // Assign array to track which values were updated
                // Check modification mode
                if ($i_mode) {
                    if (is_ipaddrv4($new_ipaddr) or is_ipaddrv6($new_ipaddr)) {
                        foreach ($hosts_conf as $he) {
                            // If our IP matches, update our IP
                            if ($ipaddr === $he["ip"]) {
                                $hosts_conf[$h_count]["ip"] = $new_ipaddr;
                                $update_list[] = $hosts_conf[$h_count];
                            }
                            // Increase our counter
                            $h_count++;
                        }
                    } else {
                        $api_resp = array("status" => "bad request", "code" => 400, "return" => 2011);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                } elseif ($h_mode) {
                    foreach ($hosts_conf as $he) {
                        $he_updated = false;
                        // Check if both our hostname and domain names were changed
                        if (isset($new_hostname) and isset($new_domain)) {
                            if ($hostname === $he["host"] and $domain === $he["domain"]) {
                                if (!unbound_host_override_exists($new_hostname, $new_domain)) {
                                    $hosts_conf[$h_count]["host"] = $new_hostname;
                                    $hosts_conf[$h_count]["domain"] = $new_domain;
                                    $he_updated = true;
                                } else {
                                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 2010);
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                            }
                        } elseif (isset($new_hostname)) {
                            if ($hostname === $he["host"] and $domain === $he["domain"]) {
                                if (!unbound_host_override_exists($new_hostname, $he["domain"])) {
                                    $hosts_conf[$h_count]["host"] = $new_hostname;
                                    $he_updated = true;
                                } else {
                                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 2010);
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                            }
                        } elseif (isset($new_domain)) {
                            if ($hostname === $he["host"] and $domain === $he["domain"]) {
                                if (!unbound_host_override_exists($he["host"], $new_domain)) {
                                    $hosts_conf[$h_count]["domain"] = $new_domain;
                                    $he_updated = true;
                                } else {
                                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 2010);
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                            }
                        }
                        if (isset($new_ipaddr)) {
                            if (is_ipaddrv4($new_ipaddr) or is_ipaddrv6($new_ipaddr)) {
                                if ($hostname === $he["host"] and $domain === $he["domain"]) {
                                    $hosts_conf[$h_count]["ip"] = $new_ipaddr;
                                    $he_updated = true;
                                }
                            } else {
                                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2011);
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        }
                        if (isset($descr)) {
                            if ($hostname === $he["host"] and $domain === $he["domain"]) {
                                $hosts_conf[$h_count]["descr"] = $descr;
                                $he_updated = true;
                            }
                        }
                        if (isset($aliases)) {
                            // Check if we have more than one
                            if (count($update_list) <= 1) {
                                $aliases_fin = unbound_parse_aliases($aliases);    // Parse our aliases
                                if ($aliases_fin !== "") {
                                    $hosts_conf[$h_count]["aliases"] = $aliases_fin;
                                    $he_updated = true;
                                }
                            }
                        }
                        // Check if our entry was updated, if so add it to our update list
                        if ($he_updated) {
                            $update_list[] = $hosts_conf[$h_count];
                        }
                        // Increase our counter
                        $h_count++;
                    }
                }
                // Sort and write our new configuration
                $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                $change_note = " Modified DNS Resolver host override via API";    // Add a change note
                usort($hosts_conf, "strcmp");
                $config["unbound"]["hosts"] = $hosts_conf;
                write_config(sprintf(gettext($change_note)));
                mark_subsystem_dirty("unbound");
                # If user requests immediately application
                if ($apply === true) {
                    $applied = unbound_reload_config();
                }
                // Return success if our function was successful
                if ($applied === true or $apply !== true) {
                    $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                    $api_resp["message"] = "Successfully updated unbound host override";
                    $api_resp["data"] = $update_list;
                    return $api_resp;
                } else {
                    $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2013);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound_start() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound_stop() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound_restart() {
    # VARIABLES
    global $err_lib, $api_resp;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-status-services");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
    $service = $uri_parse[4];    // Save our service name
    $action = $uri_parse[5];    // Save our action (start, stop, restart)
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (UPDATE)
        if ($http_method === 'POST') {
            # Check our aciton
            if ($action === "start") {
                $set_service = service_control_start($service, []);    // Start our service
                $act_str = "started";
            } elseif ($action === "stop") {
                $set_service = service_control_stop($service, []);    // Stop our service
                $act_str = "stopped";
            } elseif ($action === "restart") {
                $set_service = service_control_restart($service, []);    // Restart our service
                $act_str = "restarted";
            } else {
                $act_failed = true;
            }
            // Check if our action succeeded or failed
            if (!$act_failed) {
                // Print our success response
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully ".$act_str." service ".$service;
                return $api_resp;
            } else {
                // Print our fail response
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound_add_hosts() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-services-dnsresolver-edithost");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    // FUNCTIONS------------------------------------------------------------------------------------------------------------
    function host_cmp($a, $b) {
        return strcasecmp($a['host'], $b['host']);
    }
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['host'])) {
                $hostname = $client_params['host'];
                $hostname = trim($hostname);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2004);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['domain'])) {
                $domain = $client_params['domain'];
                $domain = trim($domain);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2005);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['ip'])) {
                $ipaddr = $client_params['ip'];
                $ipaddr = trim($ipaddr);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2006);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            }
            if (isset($client_params['aliases'])) {
                $aliases = $client_params['aliases'];
            }
            if ($client_params['apply'] === true) {
                $apply = $client_params['apply'];
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "HOSTNAME:" . PHP_EOL;
                echo var_dump($hostname) . PHP_EOL;
                echo "DOMAIN:" . PHP_EOL;
                echo var_dump($domain) . PHP_EOL;
                echo "IP ADDRESS:" . PHP_EOL;
                echo var_dump($ipaddr) . PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
                echo "ALIASES:" . PHP_EOL;
                echo var_dump($aliases) . PHP_EOL;
                echo "APPLY:" . PHP_EOL;
                echo var_dump($apply) . PHP_EOL;
            }
            // Validate our input against our exist configuration
            if (!unbound_host_override_exists($hostname, $domain)) {
                if (is_ipaddrv4($ipaddr) or is_ipaddrv6($ipaddr)) {
                    $aliases_fin = unbound_parse_aliases($aliases);    // Parse our aliases
                    // Add our host override
                    $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                    $change_note = " Added host override to DNS Resolver via API";    // Add a change note
                    $host_ent = array();
                    $host_ent["host"] = $hostname;
                    $host_ent["domain"] = $domain;
                    $host_ent["ip"] = $ipaddr;
                    $host_ent["descr"] = $descr;
                    $host_ent["aliases"] = $aliases_fin;
                    $config["unbound"]["hosts"][] = $host_ent;
                    usort($config["unbound"]["hosts"], "host_cmp");
                    write_config(sprintf(gettext($change_note)));
                    mark_subsystem_dirty("unbound");
                    # If user requests immediately application
                    if ($apply === true) {
                        $applied = unbound_reload_config();
                    }
                    // Return success if our function was successful
                    if ($applied === true or $apply !== true) {
                        $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                        $api_resp["message"] = "Successfully added unbound host override";
                        $api_resp["data"] = $host_ent;
                        return $api_resp;
                    } else {
                        $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                } else {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 2011);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2010);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }

        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_services_unbound_apply() {
    # VARIABLES;
    global $err_lib;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-services-dnsresolver-edithost");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            // Check if application was successful
            if (unbound_reload_config() === true) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "Successfully applied unbound configuration";
                $api_resp["data"] = "";
                return $api_resp;
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        }
    }
}


function api_interfaces_vlans() {
    # VARIABLES
    global $err_lib, $g, $config, $argv, $userindex, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-interfaces-vlan");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $vlan_array = array();
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $vlan_array = api_extended_search($vlan_array, $search);
            }
            if (!empty($config["vlans"]["vlan"])) {
                $vlan_array = $config["vlans"]["vlan"];
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $vlan_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_interfaces_vlans_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-interfaces-vlan-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $if_list = get_interface_list();    // Get our interfaces list
    $lagg_list = get_lagg_interface_list();    // Get our lagg interfaces list
    $avail_ifs = $if_list + $lagg_list;    // Combine the two lists
    $curr_vlans = $config["vlans"]["vlan"];    // Save our current VLANs
    $del_ent = [];    // Init our return data, this will be populated with data of deleted vlan if exists
    // Remove LAGG interface members as they cannot be assigned VLANs
    foreach ($lagg_list as $lagg_if => $lagg) {
        $lagg_members = explode(',', $lagg['members']);
        foreach ($lagg_members as $lagm) {
            if (isset($avail_ifs[$lagm])) {
                unset($avail_ifs[$lagm]);
            }
        }
    }
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (Delete)
        if ($http_method === 'POST') {
            if (isset($client_params['vlanif'])) {
                $vlan_if = $client_params['vlanif'];
            }
            elseif (isset($client_params['id'])) {
                $id = $client_params['id'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 3048);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Ensure we have a vlanif and id regardless of which input selector was provided
            if (isset($vlan_if)) {
                foreach ($curr_vlans as $ind => $cve) {
                    if ($vlan_if === $cve["vlanif"]) {
                        $id = $ind;
                        break;
                    }
                }
            } else {
                $vlan_if = $curr_vlans[$id]["vlanif"];
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "VLAN INTERFACE TO DELETE:" . PHP_EOL;
                echo var_dump($vlan_if) . PHP_EOL;
                echo "VLAN ID TO DELETE:" . PHP_EOL;
                echo var_dump($id) . PHP_EOL;
            }
            // Check that our interface is not in use currently
            if (convert_real_interface_to_friendly_interface_name($vlan_if)) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 3049);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check that we have both our ID and VLAN interface
            if (isset($id) and isset($vlan_if)) {
                // Add our VLAN
                $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                $change_note = " Deleted interface VLAN via API";    // Add a change note
                $del_ent = $config["vlans"]["vlan"][$id];    // Save our deleted VLAN
                pfSense_interface_destroy($config["vlans"]["vlan"][$id]["vlanif"]);    // delete our VLAN on the backend
                unset($config["vlans"]["vlan"][$id]);    // Remove our VLAN configuration
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            }
            // Return our success data
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully deleted interface VLAN";
            $api_resp["data"] = $del_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_interfaces_vlans_modify() {
    # VARIABLES
    global $err_lib, $g, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-interfaces-vlan-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $if_list = get_interface_list();    // Get our interfaces list
    $lagg_list = get_lagg_interface_list();    // Get our lagg interfaces list
    $avail_ifs = $if_list + $lagg_list;    // Combine the two lists
    $curr_vlans = $config["vlans"]["vlan"];    // Save our current VLANs
    // Remove LAGG interface members as they cannot be assigned VLANs
    foreach ($lagg_list as $lagg_if => $lagg) {
        $lagg_members = explode(',', $lagg['members']);
        foreach ($lagg_members as $lagm) {
            if (isset($avail_ifs[$lagm])) {
                unset($avail_ifs[$lagm]);
            }
        }
    }
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['vlanif'])) {
                $vlan_if = $client_params['vlanif'];
            }
            elseif (isset($client_params['id'])) {
                $id = $client_params['id'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 3048);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['if'])) {
                $interface = $client_params['if'];
            }
            if (isset($client_params['tag'])) {
                $tag = $client_params['tag'];
                $tag = intval(trim($tag));
                $str_tag = strval($tag);
            }
            if (isset($client_params['pcp'])) {
                $pcp = $client_params['pcp'];
                $pcp = intval(trim($pcp));
            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "PARENT INTERFACE:" . PHP_EOL;
                echo var_dump($interface) . PHP_EOL;
                echo "VLAN TAG:" . PHP_EOL;
                echo var_dump($tag) . PHP_EOL;
                echo "VLAN PRIORITY:".PHP_EOL;
                echo var_dump($pcp).PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
            }
            // Ensure we have a vlanif and id regardless of which input selector was provided
            if (isset($vlan_if)) {
                foreach ($curr_vlans as $ind => $cve) {
                    if ($vlan_if === $cve["vlanif"]) {
                        $id = $ind;
                        break;
                    }
                }
            } else {
                $vlan_if = $curr_vlans[$id]["vlanif"];
            }
            // Input validation
            $api_resp = array("status" => "bad request", "code" => 400);
            // Check if our parent interface exists
            if (!array_key_exists($id, $curr_vlans)) {
                $api_resp["return"] = 3050;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (isset($interface) and !does_interface_exist($interface)) {
                $api_resp["return"] = 3051;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (isset($tag) and (1 > $tag or $tag > 4096)) {
                $api_resp["return"] = 3052;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (isset($pcp) and (0 > $pcp or $pcp > 7)) {
                $api_resp["return"] = 3053;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check if our VLAN is already in use
            if (is_array($curr_vlans)) {
                if (isset($interface) and isset($tag)) {
                    foreach ($curr_vlans as $vle) {
                        if ($interface === $vle["if"] and $str_tag === $vle["tag"]) {
                            $api_resp["return"] = 3054;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                    }
                }
            }
            // Modify our VLAN
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Modified VLAN interface via API";    // Add a change note
            $vlan_ent = $curr_vlans[$id];   // Pull our target VLAN's current configuration
            $assigned_if = get_pfsense_if_id($vlan_if);    // Check if an interface is using this VLAN if
            $prev_vlanif = $vlan_if;    // Save our previous VLAN interface ID
            // Save our vlan interface if defined
            if (isset($interface)) {
                $vlan_ent["if"] = $interface;
            }
            // Save our tag if defined
            if (isset($tag)) {
                $vlan_ent["tag"] = $tag;
            }
            // Save our priority if defined
            if (isset($pcp)) {
                $vlan_ent["pcp"] = $pcp;
            }
            // Save our description if defined
            if (isset($descr)) {
                $vlan_ent["descr"] = $descr;
            }
            $vlan_ent["vlanif"] = $vlan_ent["if"].".".$vlan_ent["tag"];    // Format our physical interface ID
            $config["vlans"] = !is_array($config["vlans"]) ? [] : $config["vlans"];    // Init empty VLAN array if needed
            $config["vlans"]["vlan"][$id] = $vlan_ent;    // Write our configuration change
            pfSense_interface_destroy($prev_vlanif);    // Delete our previous VLAN on the backend
            interface_vlan_configure($vlan_ent);    // Configure our modified VLAN on the backend
            // Check if we need to reassign an interface
            if (!empty($assigned_if)) {
                $config['interfaces'][$assigned_if]['if'] = $vlan_ent["vlanif"];    // Write interface config
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
                interface_configure($assigned_if);    // Configure our assigned interface
            } else {
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            }
            // Return success
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "Successfully modified interface VLAN";
            $api_resp["data"] = $vlan_ent;
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_interfaces_vlans_add() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-interfaces-vlan-edit");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $if_list = get_interface_list();    // Get our interfaces list
    $lagg_list = get_lagg_interface_list();    // Get our lagg interfaces list
    $avail_ifs = $if_list + $lagg_list;    // Combine the two lists
    $curr_vlans = $config["vlans"]["vlan"];    // Save our current VLANs
    // Remove LAGG interface members as they cannot be assigned VLANs
    foreach ($lagg_list as $lagg_if => $lagg) {
        $lagg_members = explode(',', $lagg['members']);
        foreach ($lagg_members as $lagm) {
            if (isset($avail_ifs[$lagm])) {
                unset($avail_ifs[$lagm]);
            }
        }
    }
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['if'])) {
                $interface = $client_params['if'];
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 3055);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['tag'])) {
                $tag = $client_params['tag'];
                $tag = intval(trim($tag));
                $str_tag = strval($tag);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 3048);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            if (isset($client_params['pcp'])) {
                $pcp = $client_params['pcp'];
                $pcp = intval(trim($pcp));

            }
            if (isset($client_params['descr'])) {
                $descr = $client_params['descr'];
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "PARENT INTERFACE:" . PHP_EOL;
                echo var_dump($interface) . PHP_EOL;
                echo "VLAN TAG:" . PHP_EOL;
                echo var_dump($tag) . PHP_EOL;
                echo "VLAN PRIORITY:".PHP_EOL;
                echo var_dump($pcp).PHP_EOL;
                echo "DESCRIPTION:" . PHP_EOL;
                echo var_dump($descr) . PHP_EOL;
            }
            // Input validation
            $api_resp = array("status" => "bad request", "code" => 400);
            // Check if our parent interface exists
            if (!does_interface_exist($interface)) {
                $api_resp["return"] = 3051;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (1 > $tag or $tag > 4096) {
                $api_resp["return"] = 3052;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            } elseif (0 > $pcp or $pcp > 7) {
                $api_resp["return"] = 3053;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check if our VLAN is already in use
            if (is_array($curr_vlans)) {
                foreach ($curr_vlans as $vle) {
                    if ($interface === $vle["if"] and $str_tag === $vle["tag"]) {
                        $api_resp["return"] = 3054;
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                }
            }
            // Add our VLAN
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added interface VLAN via API";    // Add a change note
            $vlan_ent = array();   // Init our array
            $vlan_ent["if"] = $interface;    // Save our alias name
            $vlan_ent["tag"] = $tag;    // Save our type
            $vlan_ent["pcp"] = isset($pcp) ? $pcp : "";    // Save our priority if provided
            $vlan_ent["descr"] = isset($descr) ? $descr : "";    // Save our priority if provided
            $vlan_ent["vlanif"] = $interface.".".$tag;    // Format our physical interface ID
            $config["vlans"] = !is_array($config["vlans"]) ? [] : $config["vlans"];    // Init empty VLAN array if needed
            $config["vlans"]["vlan"][] = $vlan_ent;    // Write our configuration change
            interface_vlan_configure($vlan_ent);    // Configure our VLAN on the backend
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
            // Loop through each alias and see if our VLAN was added successfully
            foreach ($config["vlans"]["vlan"] as $se) {
                if ($interface === $se["if"] and $str_tag === $se["tag"]) {
                    $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                    $api_resp["message"] = "Successfully added interface VLAN";
                    $api_resp["data"] = $se;
                    return $api_resp;
                }
            }
            // Return error response if our loop did not find a match
            $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_interfaces_delete() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-interfaces-assignnetworkports");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            if (isset($client_params['if'])) {
                $interface = $client_params['if'];
                $interface = get_pfsense_if_id($interface);
                if (empty($interface)) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 3000);
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 3002);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Add debug data if requested
            if (array_key_exists("debug", $client_params)) {
                echo "INTERFACE TO DELETE:" . PHP_EOL;
                echo var_dump($interface) . PHP_EOL;
            }
            // Add our VLAN
            $curr_config = $config["interfaces"][$interface];    // Save our interface to delete
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $del_stat = destroy_interface($interface);    // Destroy our interface
            if ($del_stat["status"] === true) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "interface deleted");
                $api_resp["data"] = $curr_config;
                return $api_resp;
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => $del_stat["msg"]);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_interfaces() {
    # VARIABLES
    global $err_lib, $config, $api_resp, $client_params;
    $read_only_action = true;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-interfaces-assignnetworkports");    // Array of privs allowed
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is GET (READ)
        if ($http_method === 'GET') {
            $interface_array = $config["interfaces"];
            if (isset($client_params['search'])) {
                $search = $client_params['search'];
                $interface_array = api_extended_search($interface_array, $search);
            }
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $interface_array);
            return $api_resp;
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

function api_interfaces_add() {
    # VARIABLES
    global $err_lib, $g, $config, $api_resp, $client_id, $client_params;
    $read_only_action = false;    // Set whether this action requires read only access
    $req_privs = array("page-all", "page-interfaces");    // Array of privileges allowing this action
    $http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
    $if_list = get_all_avail_interfaces();    // Save all our available interfaces
    $allowed_ip4_types = array("staticv4", "dhcp");    // List of allowed IPv4 if types
    $allowed_ip6_types = array("staticv6", "dhcp6", "slaac", "6rd", "6to4", "track6");    // List of allowed IPv6 if types
    $next_if = get_next_pfsense_if_id();
    $if_ent = array($next_if => []);
    # RUN TIME
    // Check that client is authenticated and authorized
    if (api_authorized($req_privs, $read_only_action)) {
        // Check that our HTTP method is POST (CREATE)
        if ($http_method === 'POST') {
            $api_resp = array("status" => "bad request", "code" => 400);
            // Get our requested physical interface
            if (isset($client_params["if"])) {
                $interface = trim($client_params["if"]);
                // Check that our interface exists and is not in use
                if (!array_key_exists($interface, $if_list)) {
                    $api_resp["return"] = 3000;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                } elseif (isset($if_list[$interface]["in_use"])) {
                    $api_resp["return"] = 3001;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
                $if_ent[$next_if]["if"] = $interface;
            } else {
                $api_resp["return"] = 3002;
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
            // Check for our enable value
            if (isset($client_params["enable"])) {
                $enable = true;
                $if_ent[$next_if]["enable"] = "";
            }
            // Check for our MAC address value
            if (isset($client_params["spoofmac"])) {
                $mac_addr = $client_params["spoofmac"];
                // Check if mac addr is valid
                if (is_macaddr($mac_addr)) {
                    $if_ent[$next_if]["spoofmac"] = $mac_addr;
                } else {
                    $api_resp["return"] = 3003;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            // Check for our MTU value
            if (isset($client_params["mtu"])) {
                $mtu = intval($client_params["mtu"]);
                // Check if MTU is within range
                if (1280 > $mtu or $mtu > 8192) {
                    $api_resp["return"] = 3004;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                } elseif ($if_list[$interface]["is_vlan"]) {
                    // Check if interface is VLAN and that it's MTU is lower than it's parent interface
                    $parent_if = $if_list[$interface]["if"];
                    if ($mtu > $parent_if["mtu"]) {
                        $api_resp["return"] = 3006;
                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                        return $api_resp;
                    }
                } else {
                    $if_ent[$next_if]["mtu"] = $mtu;
                }
            }
            // Check for our MSS value
            if (isset($client_params["mss"])) {
                $mss = intval($client_params["mss"]);
                // Check if MSS is within range
                if (576 > $mss or $mss > 65535) {
                    $api_resp["return"] = 3005;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                } else {
                    $if_ent[$next_if]["mss"] = $mss;
                }
            }
            // Check for our SPEED/DUPLEX value
            if (isset($client_params["media"])) {
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
                    $api_resp["return"] = 3007;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            // Check for our description value
            if (isset($client_params["descr"])) {
                $descr = sanitize_str($client_params["descr"]);
                // Check that is interface descriptive name does not alrady exist
                if (!get_pfsense_if_id($descr)) {
                    $if_ent[$next_if]["descr"] = $descr;
                } else {
                    $api_resp["return"] = 3008;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            } else {
                $descr = strtoupper($next_if);
                $if_ent[$next_if]["descr"] = $descr;
            }
            // Check for our block private IP value
            if (isset($client_params["blockpriv"])) {
                $block_priv = true;
                $if_ent[$next_if]["blockpriv"] = "";
                return $api_resp;
            }
            // Check for our block private IP value
            if (isset($client_params["blockbogons"])) {
                $block_bogons = true;
                $if_ent[$next_if]["blockbogons"] = "";
            }
            // Check if we have an IPv4 configuration
            if (isset($client_params["type"])) {
                $type = $client_params["type"];
                // Check if our IPv4 config type is allowed
                if (in_array($type, $allowed_ip4_types)) {
                    // Gather input for our various IPv4 interface configuration types
                    // IPv4 STATICV4 TYPE
                    if ($type === "staticv4") {
                        // Check if our IP is set
                        if (isset($client_params["ipaddr"])) {
                            $ipaddr = $client_params["ipaddr"];
                            // Check if IP address is valid
                            if (!is_ipaddrv4($ipaddr)) {
                                $api_resp["return"] = 3010;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } elseif (is_ip_in_use($ipaddr)) {
                                $api_resp["return"] = 3009;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } else {
                                $if_ent[$next_if]["ipaddr"] = $ipaddr;
                            }
                        } else {
                            $api_resp["return"] = 3011;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                        // Check if our subnet is valid
                        if (isset($client_params["subnet"])) {
                            $subnet = strval($client_params["subnet"]);
                            // Check if our subnet is within range
                            if (!is_subnet($ipaddr."/".$subnet)) {
                                $api_resp["return"] = 3012;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } else {
                                $if_ent[$next_if]["subnet"] = $subnet;
                            }
                        } else {
                            // Update our message if we did not already encounter an error
                            $api_resp["return"] = 3013;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                        // Check if user specified a network gateway, if so check if it's valid
                        if (isset($client_params["gateway"])) {
                            $gateway = $client_params["gateway"];
                            // Check if this gateway exists
                            if (!is_gateway($gateway)) {
                                $api_resp["return"] = 3014;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } else {
                                $if_ent[$next_if]["gateway"] = $gateway;
                            }
                        }
                        // IPv4 DHCP TYPE
                    } elseif ($type === "dhcp") {
                        $if_ent[$next_if]["ipaddr"] = $type;    // Set our ipaddr value to dhcp
                        // Check if we have a dhcphostname value
                        if (isset($client_params["dhcphostname"])) {
                            $if_ent[$next_if]["dhcphostname"] = strval($client_params["dhcphostname"]);
                        }
                        // Check if we have a alias-address value
                        if (isset($client_params["alias-address"])) {
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
                                $api_resp["return"] = 3015;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        }
                        // Check if we have a dhcprejectfrom value
                        if (isset($client_params["dhcprejectfrom"])) {
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
                                $api_resp["return"] = 3016;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
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
                            if (isset($client_params[$tp])) {
                                // Check that value is in range
                                $dhcp_attr = intval($client_params[$tp]);
                                if ($dhcp_attr >= $data["min"]) {
                                    $if_ent[$next_if][$tp] = $dhcp_attr;
                                    $if_ent[$next_if]["adv_dhcp_pt_values"] = "SavedCfg";
                                } else {
                                    if ($data["keyword"] === "timeout") {
                                        $api_resp["return"] = 3017;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    } elseif ($data["keyword"] === "retry") {
                                        $api_resp["return"] = 3018;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    } elseif ($data["keyword"] === "select timeout") {
                                        $api_resp["return"] = 3019;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    } elseif ($data["keyword"] === "reboot") {
                                        $api_resp["return"] = 3020;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    } elseif ($data["keyword"] === "backoff cutoff") {
                                        $api_resp["return"] = 3021;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    } elseif ($data["keyword"] === "initial interval") {
                                        $api_resp["return"] = 3022;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    }
                                }
                            }
                        }
                        // Check for advance DHCP config
                        if (isset($client_params["adv_dhcp_config_advanced"])) {
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
                                if (isset($client_params[$do])) {
                                    $if_ent[$next_if][$do] = strval($client_params[$do]);
                                }
                            }
                        }
                        // Check for DHCP configuration file override option
                        if (isset($client_params["adv_dhcp_config_file_override"])) {
                            $if_ent[$next_if]["adv_dhcp_config_file_override"] = "";
                            // Check if a file path was given
                            if (isset($client_params["adv_dhcp_config_file_override_path"])) {
                                $dhcp_conf_file = $client_params["adv_dhcp_config_file_override_path"];
                                // Check that our conf file exists
                                if (is_file($dhcp_conf_file)) {
                                    $if_ent[$next_if]["adv_dhcp_config_file_override"] = $dhcp_conf_file;
                                } else {
                                    $api_resp["return"] = 3023;
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
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
                        if (isset($client_params["dhcpvlanenable"])) {
                            $if_ent[$next_if]["dhcpvlanenable"] = "";
                            if (isset($client_params["dhcpcvpt"])) {
                                $vlan_prio = strtolower($client_params["dhcpcvpt"]);
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
                                        $api_resp["return"] = 3024;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    $api_resp["return"] = 3025;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
                }
            }
            // Check if we have an IPv6 configuration
            if (isset($client_params["type6"])) {
                $type6 = $client_params["type6"];
                // Check if our IPv6 config type is allowed
                if (in_array($type6, $allowed_ip6_types)) {
                    // Gather input for our various IPv6 interface configuration types
                    // IPv6 STATICV6 TYPE
                    if ($type6 === "staticv6") {
                        // Check if our IP is set
                        if (isset($client_params["ipaddrv6"])) {
                            $ipaddrv6 = $client_params["ipaddrv6"];
                            // Check if IP address is valid
                            if (!is_ipaddrv6($ipaddrv6)) {
                                $api_resp["return"] = 3026;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } elseif (is_ip_in_use($ipaddrv6)) {
                                $api_resp["return"] = 3027;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } else {
                                $if_ent[$next_if]["ipaddrv6"] = $ipaddrv6;
                            }
                        } else {
                            $api_resp["return"] = 3028;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                        // Check if our subnet is valid
                        if (isset($client_params["subnetv6"])) {
                            $subnetv6 = strval($client_params["subnetv6"]);
                            // Check if our subnet is within range
                            if (!is_subnet($ipaddrv6 . "/" . $subnetv6)) {
                                $api_resp["return"] = 3029;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } else {
                                $if_ent[$next_if]["subnetv6"] = $subnetv6;
                            }
                        } else {
                            $api_resp["return"] = 3030;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                        // Check if user specified a network gateway, if so check if it's valid
                        if (isset($client_params["gatewayv6"])) {
                            $gatewayv6 = $client_params["gatewayv6"];
                            // Check if this gateway exists
                            if (!is_gateway($gatewayv6)) {
                                $api_resp["return"] = 3031;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            } else {
                                $if_ent[$next_if]["gatewayv6"] = $gatewayv6;
                            }
                        }
                        // Check if user set ipv6usev4iface value
                        if (isset($client_params["ipv6usev4iface"])) {
                            $if_ent[$next_if]["ipv6usev4iface"] = "";
                        }
                        // IPv6 DHCP6 TYPE
                    } elseif ($type6 === "dhcp6") {
                        $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to dhcp6
                        // Check if user set ipv6usev4iface value
                        if (isset($client_params["ipv6usev4iface"])) {
                            $if_ent[$next_if]["ipv6usev4iface"] = "";
                        }
                        // Check if user set dhcp6prefixonly value
                        if (isset($client_params["dhcp6prefixonly"])) {
                            $if_ent[$next_if]["dhcp6prefixonly"] = "";
                        }
                        // Check if user set dhcp6-ia-pd-send-hint value
                        if (isset($client_params["dhcp6-ia-pd-send-hint"])) {
                            $if_ent[$next_if]["dhcp6-ia-pd-send-hint"] = "";
                        }
                        // Check if user set dhcp6debug value
                        if (isset($client_params["dhcp6debug"])) {
                            $if_ent[$next_if]["dhcp6debug"] = "";
                        }
                        // Check if user set dhcp6withoutra value
                        if (isset($client_params["dhcp6withoutra"])) {
                            $if_ent[$next_if]["dhcp6withoutra"] = "";
                        }
                        // Check if user set dhcp6norelease value
                        if (isset($client_params["dhcp6norelease"])) {
                            $if_ent[$next_if]["dhcp6norelease"] = "";
                        }
                        // Check if user set dhcp6vlanenable value
                        if (isset($client_params["dhcp6vlanenable"])) {
                            $if_ent[$next_if]["dhcp6vlanenable"] = "";
                        }
                        // Check if user set dhcp6-ia-pd-len value
                        if (isset($client_params["dhcp6-ia-pd-len"])) {
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
                                $api_resp["return"] = 3032;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
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
                        if (isset($client_params["dhcp6vlanenable"])) {
                            $if_ent[$next_if]["dhcp6vlanenable"] = "";
                            if (isset($client_params["dhcp6cvpt"])) {
                                $vlan_prio = strtolower($client_params["dhcp6cvpt"]);
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
                                        $api_resp["return"] = 3033;
                                        $api_resp["message"] = $err_lib[$api_resp["return"]];
                                        return $api_resp;
                                    }
                                }
                            }
                        }
                        // Check for DHCP configuration file override option
                        if (isset($client_params["adv_dhcp6_config_file_override"])) {
                            $if_ent[$next_if]["adv_dhcp6_config_file_override"] = "";
                            // Check if a file path was given
                            if (isset($client_params["adv_dhcp6_config_file_override_path"])) {
                                $dhcp_conf_file = $client_params["adv_dhcp6_config_file_override_path"];
                                // Check that our conf file exists
                                if (is_file($dhcp_conf_file)) {
                                    $if_ent[$next_if]["adv_dhcp6_config_file_override_path"] = $dhcp_conf_file;
                                } else {
                                    $api_resp["return"] = 3034;
                                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                                    return $api_resp;
                                }
                            }
                        }
                        // IPv6 SLAAC TYPE
                    } elseif ($type6 === "slaac") {
                        $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to slaac
                        // IPv6 6RD TYPE
                    } elseif ($type6 === "6rd") {
                        $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to 6rd
                        $if_ent[$next_if]["prefix-6rd-v4plen"] = $client_params["prefix-6rd-v4plen"];    // Default prefix len
                        // Check for a 6RD border relay
                        if (isset($client_params["gateway-6rd"])) {
                            $gw6rd = $client_params["gateway-6rd"];
                            // Check that our gateway is a valid IPv4 address
                            if (is_ipaddrv4($gw6rd)) {
                                $if_ent[$next_if]["gateway-6rd"] = $client_params["gateway-6rd"];
                            } else {
                                $api_resp["return"] = 3035;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        } else {
                            $api_resp["return"] = 3036;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                        // Check for a 6RD prefix
                        if (isset($client_params["prefix-6rd"])) {
                            $if_ent[$next_if]["prefix-6rd"] = $client_params["prefix-6rd"];
                        }
                        // Check for a 6RD prefix length
                        if (isset($client_params["prefix-6rd-v4plen"])) {
                            $prefix_len = $client_params["prefix-6rd-v4plen"];
                            // Check if our prefix length is within range
                            if (is_numeric($prefix_len) and (0 <= intval($prefix_len) and intval($prefix_len) <= 32)) {
                                $if_ent[$next_if]["prefix-6rd-v4plen"] = $client_params["prefix-6rd-v4plen"];
                            } else {
                                $api_resp["return"] = 3037;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        }
                        // IPv6 TRACK TYPE
                    } elseif ($type6 === "track6") {
                        $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to track6
                        // Check for track 6 interface
                        if (isset($client_params["track6-interface"])) {
                            $track_if = $client_params["track6-interface"];
                            $track_if = get_pfsense_if_id($track_if);
                            // Check that our gateway is a valid IPv4 address
                            if (array_key_exists($track_if, get_ipv6_if_list())) {
                                $if_ent[$next_if]["track6-interface"] = $client_params["track6-interface"];
                            } else {
                                $api_resp["return"] = 3038;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        } else {
                            $api_resp["return"] = 3039;
                            $api_resp["message"] = $err_lib[$api_resp["return"]];
                            return $api_resp;
                        }
                        // Check for track 6 prefix ID
                        $track_prefix = 0;    // Default our prefix value
                        if (isset($client_params["track6-prefix-id-hex"])) {
                            $track_prefix = $client_params["track6-prefix-id-hex"];
                            // Check that our gateway is a valid IPv4 address
                            if (is_numeric($track_prefix) and ctype_xdigit(strval($track_prefix))) {
                                $if_ent[$next_if]["track6-prefix-id--hex"] = intval($track_prefix);
                            } else {
                                $api_resp["return"] = 3040;
                                $api_resp["message"] = $err_lib[$api_resp["return"]];
                                return $api_resp;
                            }
                        }
                        // IPv6 6-to-4 TYPE
                    } elseif ($type6 === "6to4") {
                        $if_ent[$next_if]["ipaddrv6"] = $type6;    // Set our ipaddrv6 value to 6to4
                    }
                } else {
                    $api_resp["return"] = 3041;
                    $api_resp["message"] = $err_lib[$api_resp["return"]];
                    return $api_resp;
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
            // Apply our configuration
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Added interface via API";    // Add a change note
            $config["interfaces"] = $config["interfaces"] + $if_ent;    // Add our new interface config
            write_config(sprintf(gettext($change_note)));    // Write our configuration change
            $apply_if = apply_interface_config($if_ent);
            if ($apply_if) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "interface added");
                $api_resp["data"] = $if_ent;
                return $api_resp;
            } else {
                // Return error response if our loop did not find a match
                $api_resp = array("status" => "server error", "code" => 500, "return" => 1);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                return $api_resp;
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 2);
            $api_resp["message"] = $err_lib[$api_resp["return"]];
            return $api_resp;
        }
    } else {
        return $api_resp;
    }
}

