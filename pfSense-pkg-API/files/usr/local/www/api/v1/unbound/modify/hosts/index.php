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

// FUNCTIONS------------------------------------------------------------------------------------------------------------
function host_cmp($a, $b) {
    return strcasecmp($a['host'], $b['host']);
}

// RUN TIME-------------------------------------------------------------------------------------------------------------
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
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 41);
            $api_resp["message"] = "domain required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
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
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 7);
            $api_resp["message"] = "new ip required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['descr'])) {
            $descr = $client_params['descr'];
        }
        if (isset($client_params['aliases'])) {
            $aliases = $client_params['aliases'];
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
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 7);
                    $api_resp["message"] = "invalid ip address";
                    http_response_code(400);
                    echo json_encode($api_resp) . PHP_EOL;
                    die();
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
                                $api_resp = array("status" => "bad request", "code" => 400, "return" => 43);
                                $api_resp["message"] = "host override already exists";
                                http_response_code(400);
                                echo json_encode($api_resp) . PHP_EOL;
                                die();
                            }
                        }
                    } elseif (isset($new_hostname)) {
                        if ($hostname === $he["host"] and $domain === $he["domain"]) {
                            if (!unbound_host_override_exists($new_hostname, $he["domain"])) {
                                $hosts_conf[$h_count]["host"] = $new_hostname;
                                $he_updated = true;
                            } else {
                                $api_resp = array("status" => "bad request", "code" => 400, "return" => 43);
                                $api_resp["message"] = "host override already exists";
                                http_response_code(400);
                                echo json_encode($api_resp) . PHP_EOL;
                                die();
                            }
                        }
                    } elseif (isset($new_domain)) {
                        if ($hostname === $he["host"] and $domain === $he["domain"]) {
                            if (!unbound_host_override_exists($he["host"], $new_domain)) {
                                $hosts_conf[$h_count]["host"] = $new_hostname;
                                $he_updated = true;
                            } else {
                                $api_resp = array("status" => "bad request", "code" => 400, "return" => 43);
                                $api_resp["message"] = "host override already exists";
                                http_response_code(400);
                                echo json_encode($api_resp) . PHP_EOL;
                                die();
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
                            $api_resp = array("status" => "bad request", "code" => 400, "return" => 7);
                            $api_resp["message"] = "invalid ip address";
                            http_response_code(400);
                            echo json_encode($api_resp) . PHP_EOL;
                            die();
                        }
                    }
                    if (isset($descr)) {
                        if ($hostname === $he["host"] and $domain === $he["domain"]) {
                            $hosts_conf[$h_count]["descr"] = $descr;
                            $he_updated = true;
                        }
                    }
                    if (isset($aliases)) {
                        $aliases_fin = unbound_parse_aliases($aliases);    // Parse our aliases
                        if ($aliases_fin !== "") {
                            $hosts_conf[$h_count]["aliases"] = $aliases_fin;
                            $he_updated = true;
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
            $reload_unbound = 0;
            $reload_unbound |= services_unbound_configure();
            if ($reload_unbound == 0) {
                system_resolvconf_generate();    // Update resolveconf
                system_dhcpleases_configure();    // Update DHCPD
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "host override updated";
                $api_resp["data"] = $update_list;
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
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 44);
            $api_resp["message"] = "host override does not exist";
            http_response_code(400);
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

