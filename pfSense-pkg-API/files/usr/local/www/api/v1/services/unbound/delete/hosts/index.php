<?php

// IMPORTS--------------------------------------------------------------------------------------------------------------
require_once("config.inc");
require_once("services.inc");
require_once("system.inc");
require_once("util.inc");
require_once("functions.inc");
require_once("api.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
api_runtime_allowed();    // Check that our configuration allows this API call to run first
header('Content-Type: application/json');
header("Referer: no-referrer");
session_start();    // Start our session. This is only used for tracking user name

// VARIABLES------------------------------------------------------------------------------------------------------------
global $g, $config, $argv, $userindex, $api_resp, $client_id, $client_params;
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-services-dnsresolver-edithost");    // Array of privileges allowing this action
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$del_mode = "";

// FUNCTIONS------------------------------------------------------------------------------------------------------------
function host_cmp($a, $b) {
    return strcasecmp($a['host'], $b['host']);
}

// RUN TIME-------------------------------------------------------------------------------------------------------------
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
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
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
            $reload_unbound = 0;
            $reload_unbound |= services_unbound_configure();
            if ($reload_unbound == 0) {
                system_resolvconf_generate();    // Update resolveconf
                system_dhcpleases_configure();    // Update DHCPD
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "host override deleted";
                $api_resp["data"] = $del_list;
                http_response_code(200);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            } else {
                $api_resp = array("status" => "server error", "code" => 500, "return" => 4, "message" => "process fail");
                http_response_code(500);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            }
        } else {
            //to do
            return;
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

