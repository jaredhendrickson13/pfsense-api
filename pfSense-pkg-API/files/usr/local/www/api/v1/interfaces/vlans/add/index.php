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
    // Check that our HTTP method is POST (CREATE)
    if ($http_method === 'POST') {
        if (isset($client_params['if'])) {
            $interface = $client_params['if'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 11);
            $api_resp["message"] = "parent interface required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['tag'])) {
            $tag = $client_params['tag'];
            $tag = intval(trim($tag));
            $str_tag = strval($tag);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 110);
            $api_resp["message"] = "vlan tag required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['pcp'])) {
            $pcp = $client_params['pcp'];
            $pcp = intval(trim($pcp));

        }
        if (isset($client_params['descr'])) {
            $descr = $client_params['descr'];
        }

        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "PARENT INTERFACE:" . PHP_EOL;
            echo var_dump($interface) . PHP_EOL;
            echo "VLAN TAG:" . PHP_EOL;
            echo var_dump($tag) . PHP_EOL;
            echo "VLAN PRIORITY:".PHP_EOL;
            echo var_dump($pcp).PHP_EOL;
            echo "DESCRIPTION:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;

        }

        // Input validation
        $api_resp = array("status" => "bad request", "code" => 400);
        $err_found = false;
        // Check if our parent interface exists
        if (!does_interface_exist($interface)) {
            $err_found = true;
            $api_resp["message"] = "parent interface does not exist";
            $api_resp["return"] = 11;
        } elseif (1 > $tag or $tag > 4096) {
            $err_found = true;
            $api_resp["message"] = "vlan tag out of range";
            $api_resp["return"] = 111;
        } elseif (0 > $pcp or $pcp > 7) {
            $err_found = true;
            $api_resp["message"] = "vlan priority out of range";
            $api_resp["return"] = 112;
        }
        // Check if our VLAN is already in use
        if (!$err_found and is_array($curr_vlans)) {
            foreach ($curr_vlans as $vle) {
                if ($interface === $vle["if"] and $str_tag === $vle["tag"]) {
                    $err_found = true;
                    $api_resp["message"] = "vlan already exists for this interface";
                    $api_resp["return"] = 113;
                    break;
                }
            }
        }
        // Return error if input validation failed
        if ($err_found) {
            echo json_encode($api_resp);
            http_response_code($api_resp["code"]);
            die();
        }

        // Add our VLAN
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Added interface VLAN via API";    // Add a change note
        $vlan_ent = array();   // Init our array
        $vlan_ent["if"] = $interface;    // Save our alias name
        $vlan_ent["tag"] = $tag;    // Save our type
        $vlan_ent["pcp"] = isset($pcp) ? $pcp : "";    // Save our priority if provided
        $vlan_ent["descr"] = isset($descr) ? $descr : "";    // Save our priority if provided
        $vlan_ent["vlanif"] = $interface.".".$tag;    // Format our physical interface ID
        $config["vlans"] = !is_array($config["vlans"]) ? [] : $config["vlans"];    // Init empty VLAN array if needed
        $config["vlans"]["vlan"][] = $vlan_ent;    // Write our configuration change
        interface_vlan_configure($vlan_ent);    // Configure our VLAN on the backend
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        // Loop through each alias and see if our VLAN was added successfully
        foreach ($config["vlans"]["vlan"] as $se) {
            if ($interface === $se["if"] and $str_tag === $se["tag"]) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "vlan added";
                $api_resp["data"] = $se;
                http_response_code(200);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            }
        }
        // Return error response if our loop did not find a match
        $api_resp = array("status" => "server error", "code" => 500, "return" => 4, "message" => "process fail");
        http_response_code(500);
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

