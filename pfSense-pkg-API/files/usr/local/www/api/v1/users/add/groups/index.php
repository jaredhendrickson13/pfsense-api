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
global $g, $config, $userindex, $groupindex, $api_resp, $client_id, $client_params;
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-system-usermanager-addprivs", "page-system-groupmanager-");   // Allowed privs
$userindex = index_users();    // Index our users
$groupindex = index_groups();    // Index our groups
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (CREATE)
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
        if (isset($client_params['group'])) {
            $new_groups = $client_params['group'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 25);
            $api_resp["message"] = "group required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        // Check if our user already exists, if so exit on non-zero
        if (!array_key_exists($username, $userindex)) {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 23);
            $api_resp["message"] = "user does not exist";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
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
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 30);
                    $api_resp["message"] = "unknown group `" . $ng . "`";
                    http_response_code(400);
                    echo json_encode($api_resp) . PHP_EOL;
                    die();
                }
                if (in_array($ng, $user_groups)) {
                    $new_groups = \array_diff($new_groups, array($ng));
                }
            }
            // Add our new groups to our existing groups
            $new_groups = array_merge($user_groups, $new_groups);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 29);
            $api_resp["message"] = "group must be list or string";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
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
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "groups added");
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
