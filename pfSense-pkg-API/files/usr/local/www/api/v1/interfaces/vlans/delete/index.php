<?php

// IMPORTS--------------------------------------------------------------------------------------------------------------
require_once("config.inc");
require_once("auth.inc");
require_once("functions.inc");
require_once("interfaces.inc");
require_once("api.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
api_runtime_allowed();    // Check that our configuration allows this API call to run first
header('Content-Type: application/json');
header("Referer: no-referrer");
session_start();    // Start our session. This is only used for tracking user name

// VARIABLES------------------------------------------------------------------------------------------------------------
global $g, $config, $api_resp, $client_id, $client_params;
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-interfaces-vlan-edit");    // Array of privileges allowing this action
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$if_list = get_interface_list();    // Get our interfaces list
$lagg_list = get_lagg_interface_list();    // Get our lagg interfaces list
$avail_ifs = $if_list + $lagg_list;    // Combine the two lists
$curr_vlans = $config["vlans"]["vlan"];    // Save our current VLANs
$del_ent = [];    // Init our return data, this will be populated with data of deleted vlan if exists

// Remove LAGG interface members as they cannot be assigned VLANs
foreach ($lagg_list as $lagg_if => $lagg) {
    $lagg_members = explode(',', $lagg['members']);
    foreach ($lagg_members as $lagm) {
        if (isset($avail_ifs[$lagm])) {
            unset($avail_ifs[$lagm]);
        }
    }
}

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (Delete)
    if ($http_method === 'POST') {
        if (isset($client_params['vlanif'])) {
            $vlan_if = $client_params['vlanif'];
        }
        elseif (isset($client_params['id'])) {
            $id = $client_params['id'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 11);
            $api_resp["message"] = "vlan id required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        // Ensure we have a vlanif and id regardless of which input selector was provided
        if (isset($vlan_if)) {
            foreach ($curr_vlans as $ind => $cve) {
                if ($vlan_if === $cve["vlanif"]) {
                    $id = $ind;
                    break;
                }
            }
        } else {
            $vlan_if = $curr_vlans[$id]["vlanif"];
        }

        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "VLAN INTERFACE TO DELETE:" . PHP_EOL;
            echo var_dump($vlan_if) . PHP_EOL;
            echo "VLAN ID TO DELETE:" . PHP_EOL;
            echo var_dump($id) . PHP_EOL;
        }

        // Check that our interface is not in use currently
        if (convert_real_interface_to_friendly_interface_name($vlan_if)) {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 11, "message" => "interface in use");
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        // Check that we have both our ID and VLAN interface
        if (isset($id) and isset($vlan_if)) {
            // Add our VLAN
            $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
            $change_note = " Deleted interface VLAN via API";    // Add a change note
            $del_ent = $config["vlans"]["vlan"][$id];    // Save our deleted VLAN
            pfSense_interface_destroy($config["vlans"]["vlan"][$id]["vlanif"]);    // delete our VLAN on the backend
            unset($config["vlans"]["vlan"][$id]);    // Remove our VLAN configuration
            write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        }
        // Return our success data
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0, "message" => "vlan deleted");
        $api_resp["data"] = $del_ent;
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

