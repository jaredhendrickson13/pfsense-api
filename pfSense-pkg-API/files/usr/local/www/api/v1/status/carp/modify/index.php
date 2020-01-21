<?php

// IMPORTS--------------------------------------------------------------------------------------------------------------
require_once("config.inc");
require_once("auth.inc");
require_once("functions.inc");
require_once("api.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
api_runtime_allowed();    // Check that our configuration allows this API call to run first
header('Content-Type: application/json');
header("Referer: no-referrer");
session_start();    // Start our session. This is only used for tracking user name

// VARIABLES------------------------------------------------------------------------------------------------------------
global $g, $config, $api_resp, $client_id, $client_params;
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-status-carp");    // Array of privileges allowing this action
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$carp_stat_arr = [];    // Init array to track what changes were made

// RUN TIME-------------------------------------------------------------------------------------------------------------
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
        $change_note = " Modified CARP status via API";    // Add a change note
        interfaces_carp_set_maintenancemode($mm_enable);    // Set our maintenance mode value
        enable_carp($enable);    // Set our CARP enable value
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "success", "data" => $carp_stat_arr);
        http_response_code(200);
        echo json_encode($api_resp) . PHP_EOL;
        die();
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

