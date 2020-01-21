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
$req_privs = array("page-all", "page-diagnostics-arptable");    // Array of privs allowed
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$arp_cmd = "arp -an";    // Assign the command which reads our ARP table
$arp_array = array();    // Init our ARP array

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is GET (READ)
    if ($http_method === 'GET') {
        exec($arp_cmd, $arp_array);    // Output our ARP table into an array
        $arp_data = array();
        foreach ($arp_array as $arp_line) {
            $elements = explode(' ', $arp_line, 7);
            $arp_entry = array();
            $arp_entry['ip'] = trim(str_replace(array('(', ')'), '', $elements[1]));
            $arp_entry['mac'] = trim($elements[3]);
            $arp_entry['interface'] = trim($elements[5]);
            $arp_entry['status'] = trim(substr($elements[6], 0, strrpos($elements[6], ' ')));
            $arp_entry['linktype'] = trim(str_replace(array('[', ']'), '', strrchr($elements[6], ' ')));
            $arp_data[] = $arp_entry;
        }
        // Print our JSON response
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "", "data" => $arp_data);
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
