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
global $g, $config, $userindex, $api_resp, $client_id, $client_params;
$logging_level = LOG_WARNING;    // Set our log level
$logging_prefix = gettext("Local User Database");    // Set our logging prefix
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-system-usermanager");    // Array of privileges allowing this action
$userindex = index_users();    // Index our users
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {

    // Check that our HTTP method is POST (DELETE)
    if ($http_method === 'POST') {
        if (isset($client_params['username'])) {
            $username = $client_params['username'];
            $username = trim($username);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 20);
            $api_resp["message"] = "username required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        // Check if our user already exists, if not exit on non-zero
        if (!array_key_exists($username, $userindex)) {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 23);
            $api_resp["message"] = "user does not exist";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
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
        unset($config['system']['user'][$index_id]);    // Unset our user from config
        $config['system']['user'] = array_values($config['system']['user']);    // Reindex our users
        write_config(sprintf(gettext($change_note)));    // Write our new config
        //--Check that our changes were successful-----------
        $userindex = index_users();    // Update our user index
        if (!array_key_exists($username, $userindex)) {
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "user deleted");
            http_response_code(200);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        } else {
            $api_resp = array("status" => "server error", "code" => 500, "return" => 1, "message" => "process fail");
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
