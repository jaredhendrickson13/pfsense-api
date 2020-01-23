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

// RUN TIME-------------------------------------------------------------------------------------------------------------
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
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "services have been restarted");
        $api_resp["data"] = $services_set;
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