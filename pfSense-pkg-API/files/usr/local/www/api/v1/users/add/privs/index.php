<?php

// IMPORTS--------------------------------------------------------------------------------------------------------------
require_once("config.inc");
require_once("auth.inc");
require_once("functions.inc");
require_once("priv.defs.inc");
require_once("api.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
api_runtime_allowed();    // Check that our configuration allows this API call to run first
header('Content-Type: application/json');
header("Referer: no-referrer");
session_start();    // Start our session. This is only used for tracking user name

// VARIABLES------------------------------------------------------------------------------------------------------------
global $g, $config, $priv_list, $userindex, $api_resp, $client_id, $client_params;
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-system-usermanager-addprivs");    // Array of privileges allowing this action
$userindex = index_users();    // Index our users
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
        if (isset($client_params['priv'])) {
            $new_priv = $client_params['priv'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 25);
            $api_resp["message"] = "priv required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        // Check if our user already exists, if so exit on non-zero
        $user_config =& getUserEntry($username);
        $user_privs = get_user_privileges($user_config);
        if (!array_key_exists("uid", $user_config)) {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 23);
            $api_resp["message"] = "user does not exist";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        // Ensure our new priv is array, if it is a string create an array containing the string
        if (is_string($new_priv)) {
            $new_priv = array($new_priv);
        }
        if (is_array($new_priv)) {
            // Loop through our new priv list and check that the privs are valid
            foreach ($new_priv as $np) {
                if (!array_key_exists($np, $priv_list)) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 27);
                    $api_resp["message"] = "unknown priv `" . $np . "`";
                    http_response_code(400);
                    echo json_encode($api_resp) . PHP_EOL;
                    die();
                }
                if (in_array($np, $user_privs)) {
                    $new_priv = \array_diff($new_priv, array($np));
                }
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 26);
            $api_resp["message"] = "priv must be list or string";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
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
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "priv added");
            $api_resp["data"] = $user_privs;
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
