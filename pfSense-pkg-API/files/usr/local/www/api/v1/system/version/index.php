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
$req_privs = array("page-all", "page-dashboard-widgets", "page-diagnostics-command");    // Array of privs allowed
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$ver_path = "/etc/version";    // Assign the path to our version file
$ver_patch_path = "/etc/version.patch";    // Assign the path to our version patch file
$ver_bt_path = "/etc/version.buildtime";    // Assign the path to our version build time file
$ver_lc_path = "/etc/version.lastcommit";    // Assign the path to our version last commit file
$ver_data = array();    // Init an empty array for our version data

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is GET (READ)
    if ($http_method === 'GET') {
        // Check that our files exist, if so read the files. Otherwise return error
        if (file_exists($ver_path)) {
            $ver_file = fopen($ver_path, "r");    // Open our file
            $ver = str_replace(PHP_EOL, "", fread($ver_file, filesize($ver_path)));    // Save our version data
            $ver_data["version"] = $ver;    // Save to array
        } else {
            $ver_fail = false;    // Save a track indicating our read failed
        }
        if (file_exists($ver_patch_path)) {
            $ver_patch_file = fopen($ver_patch_path, "r");    // Open our file
            $ver_patch = str_replace(PHP_EOL, "", fread($ver_patch_file, filesize($ver_patch_path)));    // Save patch
            $ver_data["patch"] = $ver_patch;    // Save to array
        } else {
            $ver_fail = false;    // Save a track indicating our read failed
        }
        if (file_exists($ver_bt_path)) {
            $ver_bt_file = fopen($ver_bt_path, "r");    // Open our file
            $ver_bt = str_replace(PHP_EOL, "", fread($ver_bt_file, filesize($ver_bt_path)));    // Save bt data
            $ver_data["buildtime"] = $ver_bt;    // Save to array
        } else {
            $ver_fail = false;    // Save a track indicating our read failed
        }
        if (file_exists($ver_lc_path)) {
            $ver_lc_file = fopen($ver_lc_path, "r");    // Open our file
            $ver_lc = str_replace(PHP_EOL, "", fread($ver_lc_file, filesize($ver_lc_path)));    // Save bt data
            $ver_data["lastcommit"] = $ver_lc;    // Save to array
        } else {
            $ver_fail = false;    // Save a track indicating our read failed
        }
        // Check that our version read did not fail
        if ($ver_fail !== false) {
            // Print our JSON response
            $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $ver_data);
            http_response_code(200);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        } else {
            $api_resp = array("status" => "not found", "code" => 404, "return" => 6, "message" => "file not found");
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
