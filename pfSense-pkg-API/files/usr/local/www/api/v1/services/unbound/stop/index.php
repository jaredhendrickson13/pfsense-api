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

// VARIABLES------------------------------------------------------------------------------------------------------------
global $config, $api_resp, $client_params;
$read_only_action = true;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-status-services");    // Array of privs allowed
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$uri_parse = explode("/", $_SERVER["REQUEST_URI"]);    // Save our URI
$service = $uri_parse[4];    // Save our service name
$action = $uri_parse[5];    // Save our action (start, stop, restart)

// RUN TIME-------------------------------------------------------------------------------------------------------------
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
            $api_resp["message"] = $service." has been ".$act_str;
            http_response_code(200);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        } else {
            // Print our fail response
            $api_resp = array("status" => "server error", "code" => 500, "return" => 1, "message" => "process failed");
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