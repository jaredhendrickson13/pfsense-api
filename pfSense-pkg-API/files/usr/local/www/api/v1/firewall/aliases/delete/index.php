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
global $g, $config, $argv, $userindex, $api_resp, $client_id, $client_params;
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-firewall-aliases-edit");    // Array of privileges allowing this action
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$allowed_alias_types = array("host", "network", "port");    // Array of allowed alias types

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (CREATE)
    if ($http_method === 'POST') {
        if (isset($client_params['name'])) {
            $name = $client_params['name'];
            $name = sanitize_str($name);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 60);
            $api_resp["message"] = "alias name required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
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
            if (is_numeric($del_index)) {
                $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
                $change_note = " Deleted firewall alias via API";    // Add a change note
                unset($config["aliases"]["alias"][$del_index]);    // Remove this alias from our configuration
                $config["aliases"]["alias"] = array_values($config["aliases"]["alias"]);    // Reindex array
                write_config(sprintf(gettext($change_note)));    // Apply our configuration change
                send_event("filter reload");    // Ensure our firewall filter is reloaded
            }
            // Check if our alias is still in the configuration, if so return error response
            foreach ($config["aliases"]["alias"] as $se) {
                if ($se["name"] === $name) {
                    $api_resp = array("status" => "server error", "code" => 500, "return" => 4, "message" => "process fail");
                    http_response_code(500);
                    echo json_encode($api_resp) . PHP_EOL;
                    die();
                }
            }
            // Return success if we did not find our deleted alias in configuration during loop
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
            $api_resp["message"] = "alias deleted";
            http_response_code(200);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 66, "message" => "alias in use");
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

