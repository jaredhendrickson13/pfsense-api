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
$req_privs = array("page-all", "page-system-usermanager");    // Array of privileges allowing this action
$userindex = index_users();    // Index our users
$uid = $config["system"]["nextuid"];    // Save our next UID
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
        if (isset($client_params['password'])) {
            $password = $client_params['password'];
            $password = trim($password);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 21);
            $api_resp["message"] = "password required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (array_key_exists("disabled", $client_params)) {
            $disabled = true;
        }
        if (isset($client_params['descr'])) {
            $descr = $client_params['descr'];
            $descr = trim($descr);
        }
        if (isset($client_params['expires'])) {
            $expires = $client_params['expires'];
            $expires = trim($expires);
            $expires = new DateTime($expires);
        }
        if (isset($client_params['authorizedkeys'])) {
            $authorizedkeys = $client_params['authorizedkeys'];
            $authorizedkeys = trim($authorizedkeys);
        }
        if (isset($client_params['ipsecpsk'])) {
            $ipsecpsk = $client_params['ipsecpsk'];
            $ipsecpsk = trim($ipsecpsk);
        }

        // Check if our user already exists, if so exit on non-zero
        $user_config =& getUserEntry($username);
        if (array_key_exists("uid", $user_config)) {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 22);
            $api_resp["message"] = "user already exists";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "USERNAME:" . PHP_EOL;
            echo var_dump($username) . PHP_EOL;
            echo "DISABLE:" . PHP_EOL;
            echo var_dump($disabled) . PHP_EOL;
            echo "FULL NAME:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;
            echo "EXPIRES:".PHP_EOL;
            echo var_dump($expires).PHP_EOL;
            echo "AUTH KEY:" . PHP_EOL;
            echo var_dump($authorizedkeys) . PHP_EOL;
            echo "IPSEC KEY:" . PHP_EOL;
            echo var_dump($ipsecpsk) . PHP_EOL;
        }
        // Add our user
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Added user `" . $username . "' via API";    // Add a change note
        local_user_set_password($user_config, $password);  // Set our new user's password
        $user_config["scope"] = "user";    // Set our new user's system scope
        $user_config["uid"] = $uid;    // Set our new user's UID
        $user_config["name"] = $username;    // Set our new user's username
        // Check for our optional parameters
        if (!empty($descr)) {
            $user_config["descr"] = $descr;    // Set our new user's full name
        }
        if (!empty($expires)) {
            $user_config["expires"] = $expires;    // Set our new user's expiration date
        }
        if (!empty($authorizedkeys)) {
            $user_config["authorizedkeys"] = $authorizedkeys;    // Set our new user's authorized keys
        }
        if (!empty($ipsecpsk)) {
            $user_config["ipsecpsk"] = $ipsecpsk;    // Set our new user's IPsec pre-shared key
        }
        if ($disabled === true) {
            $user_config["disabled"] = "";    // Set our new user's disabled value if not false
        }
        $user_config["priv"] = array(null);    // Default our privs to empty array
        // Append our new user's configuration to our user list, then set our configuration and write config
        $config['system']['user'] = array_merge($config['system']['user'], array($user_config));
        $config["system"]["nextuid"] = strval(intval($uid) + 1);   // Increase our next UID
        local_user_set($user_config);
        write_config(sprintf(gettext($change_note)));
        // Check our updated config to ensure our change was successful
        $userindex = index_users();    // Update our user index
        $user_config =& getUserEntry($username);    // Update our user config
        // Check that our user is now in the user configuration
        if (array_key_exists("uid", $user_config)) {
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "user added");
            $api_resp["data"] = $user_config;
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

