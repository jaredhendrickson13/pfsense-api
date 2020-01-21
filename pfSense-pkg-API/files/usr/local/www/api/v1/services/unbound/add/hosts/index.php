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
    // Check that our HTTP method is POST (CREATE)
    if ($http_method === 'POST') {
        if (isset($client_params['host'])) {
            $hostname = $client_params['host'];
            $hostname = trim($hostname);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 40);
            $api_resp["message"] = "hostname required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['domain'])) {
            $domain = $client_params['domain'];
            $domain = trim($domain);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 41);
            $api_resp["message"] = "domain required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['ip'])) {
            $ipaddr = $client_params['ip'];
            $ipaddr = trim($ipaddr);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 42);
            $api_resp["message"] = "ip required";
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
            echo "DESCRIPTION:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;
            echo "ALIASES:" . PHP_EOL;
            echo var_dump($aliases) . PHP_EOL;
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
                $reload_unbound = 0;
                $reload_unbound |= services_unbound_configure();
                if ($reload_unbound == 0) {
                    system_resolvconf_generate();    // Update resolveconf
                    system_dhcpleases_configure();    // Update DHCPD
                    $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                    $api_resp["message"] = "host override added";
                    $api_resp["data"] = $host_ent;
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
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 7);
                $api_resp["message"] = "invalid ip address";
                http_response_code(400);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 43);
            $api_resp["message"] = "host override already exists";
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

