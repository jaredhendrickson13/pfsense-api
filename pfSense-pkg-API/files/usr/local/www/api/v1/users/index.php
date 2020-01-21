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
global $g, $config, $argv, $userindex, $api_resp, $client_params;
$read_only_action = true;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-system-usermanager");    // Array of privileges allowing this action
$userindex = index_users();    // Index our users
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is GET (READ)
    if ($http_method === 'GET') {
        // Save input data
        if (isset($client_params['search'])) {
            $search = $client_params['search'];
        }
        // Input validation
        $base_data = $config["system"]["user"];
        $search_failed = false;
        // If client requests an extended search
        if (isset($search)) {
            if (is_array($search)) {
                $target_item = end($search);    // Save our last array item
                // Loop through each attribute in the search list and check if it exists
                foreach ($search as &$df) {
                    if (array_key_exists($df, $base_data)) {
                        // If this attribute is our target item, format and return the value
                        if ($df === $target_item) {
                            if (!is_array($base_data[$df])) {
                                $base_data = array($df => $base_data[$df]);
                            } else {
                                $base_data = $base_data[$df];
                            }
                            break;
                        } else {
                            // Check if this item is an array, if so search the nested array. Otherwise failed
                            if (is_array($base_data[$df])) {
                                $base_data = $base_data[$df];
                            } else {
                                $search_failed = true;
                                break;
                            }
                        }
                    } else {
                        $search_failed = true;
                        break;
                    }
                }
                // Check if we failed to find our search attributes
                if ($search_failed === true) {
                    $api_resp = array("status" => "not found", "code" => 404, "return" => 5);
                    $api_resp["message"] = "search attribute not found";
                    http_response_code(404);
                    echo json_encode($api_resp) . PHP_EOL;
                    die();
                }
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 24);
                $api_resp["message"] = "search must be array";
                http_response_code(400);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            }
        }
        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "SEARCH ATTRIBUTES:" . PHP_EOL;
            echo var_dump($search) . PHP_EOL;
        }
        // Print our JSON response
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $base_data);
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
