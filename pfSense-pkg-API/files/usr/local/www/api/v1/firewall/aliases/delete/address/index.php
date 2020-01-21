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
$detail = array();

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (DELETE)
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
        if (isset($client_params['address'])) {
            $address = $client_params['address'];
            // Convert string to array
            if (!is_array($address)) {
                $address = array($address);
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 61);
            $api_resp["message"] = "alias address required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "NAME:" . PHP_EOL;
            echo var_dump($name) . PHP_EOL;
            echo "ALIAS VALUES TO DELETE:".PHP_EOL;
            echo var_dump($address).PHP_EOL;
        }

        // Check that our input is valid
        $err_code = 10;    // Default our error code to 10 as this applies to most checks
        if (!is_string($name)) {
            $type_err = "alias name must be type string";
        } elseif (!is_array($address)) {
            $type_err = "alias address must be type array";
        }
        // Loop through our existing firewall entries and check for our requested alias
        $c_count = 0;
        foreach ($config["aliases"]["alias"] as $ce) {
            if ($name === $ce["name"]) {
                $alias_found = true;
                $a_index = $c_count;
                $curr_addr = explode(" ", $ce["address"]);
                $curr_detail = explode("||", $ce["detail"]);
                break;
            }
            $c_count++;    // Increase our counter
        }
        // If we could not find an alias, return error
        if ($alias_found !== true) {
            $type_err = "alias does not exist";
            $err_code = 67;
        }

        // Return bad request if error
        if (isset($type_err)) {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => $err_code);
            $api_resp["message"] = $type_err;
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        // Loop through our existing configuration and remove alias address values on match
        $r_count = 0;
        foreach ($curr_addr as $re) {
            if (in_array($re, $address)) {
                unset($curr_addr[$r_count]);
                unset($curr_detail[$r_count]);
            }
            $r_count++;    // Increase our counter
        }
        // Add our alias
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Deleted firewall alias address via API";    // Add a change note
        $config["aliases"]["alias"][$a_index]["address"] = implode(" ", $curr_addr);
        $config["aliases"]["alias"][$a_index]["detail"] = implode("||", $curr_detail);
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        // Loop through each alias and see if our alias was added successfully
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
        $api_resp["message"] = "alias address deleted";
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

