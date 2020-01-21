<?php
// Notes: Functions `is_alias(), alias_get_type(), is_ipaddr(), is_hostname(), is_subnet()
//        is_port_or_range()` referenced in /etc/inc/utils.inc

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

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (UPDATE)
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
        if (isset($client_params["new_name"])) {
            $new_name = $client_params['new_name'];
            $new_name = sanitize_str($new_name);
        }
        if (isset($client_params['type'])) {
            $type = $client_params['type'];
            $type = trim($type);
        } else {
            $type = alias_get_type($name);
        }
        if (isset($client_params['address'])) {
            $address = $client_params['address'];
            // Convert string to array
            if (!is_array($address)) {
                $address = array($address);
            }
        }
        if (isset($client_params['descr'])) {
            $descr = strval($client_params['descr']);
            $descr = str_replace(PHP_EOL, "", $descr);
        }
        if (isset($client_params['detail'])) {
            $detail = $client_params['detail'];
            // Convert string to array
            if (!is_array($detail)) {
                $detail = array($detail);
            }
        }

        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "NAME:" . PHP_EOL;
            echo var_dump($name) . PHP_EOL;
            echo "NEW NAME:" . PHP_EOL;
            echo var_dump($name) . PHP_EOL;
            echo "TYPE:" . PHP_EOL;
            echo var_dump($type) . PHP_EOL;
            echo "DESCRIPTION:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;
            echo "ALIAS VALUES:".PHP_EOL;
            echo var_dump($address).PHP_EOL;
            echo "ALIAS VALUE DESCRIPTIONS:" . PHP_EOL;
            echo var_dump($detail) . PHP_EOL;
        }

        // INPUT VALIDATION
        if (!is_alias($name)) {
            $input_err = true;
            $api_resp["message"] = "alias does not exist";
            $api_resp["return"] = 67;
        } elseif (isset($new_name) and is_alias($new_name)) {
            $input_err = true;
            $api_resp["message"] = "alias already exists";
            $api_resp["return"] = 65;
        } elseif (isset($type) and !in_array($type, $allowed_alias_types)) {
            $input_err = true;
            $api_resp["message"] = "unknown alias type";
            $api_resp["return"] = 64;
        }
        if (!$input_err and isset($address)) {
            foreach ($address as $nae) {
                if ($type === "host") {
                    if (!is_ipaddr($nae) and !is_hostname($nae)) {
                        $input_err = true;
                        $api_resp["message"] = "invalid ip address or hostname";
                        $api_resp["return"] = 7;
                        break;
                    }
                }
                if ($type === "network") {
                    if (!is_subnet($nae) and !is_hostname($nae)) {
                        $input_err = true;
                        $api_resp["message"] = "invalid subnet";
                        $api_resp["return"] = 9;
                        break;
                    }
                }
                if ($type === "port") {
                    if (!is_port_or_range($nae)) {
                        $input_err = true;
                        $api_resp["message"] = "invalid port";
                        $api_resp["return"] = 8;
                        break;
                    }
                }
            }
        }
        // If we encountered an error, return error response
        if ($input_err) {
            $api_resp["code"] = 400;
            $api_resp["status"] = "bad request";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        // Save our alias ID
        $c_count = 0;
        foreach ($config["aliases"]["alias"] as $ca) {
            if ($name === $ca["name"]) {
                    $a_index = $c_count;
                }
            $c_count++;
        }

        // Make our alias change
        $curr_ent = $config["aliases"]["alias"][$a_index];    // Save our current alias entry
        if (isset($new_name)) {
            $curr_ent["name"] = $new_name;
            update_alias_name($new_name, $name);    // Update our alias name
        }
        if (isset($type)) {
            $curr_ent["type"] = $type;
        }
        if (isset($descr)) {
            $curr_ent["descr"] = $descr;
        }
        if (isset($address)) {
            $curr_ent["address"] = implode(" ", $address);
        }
        if (isset($detail)) {
            $curr_ent["detail"] = implode("||", $detail);
        }
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Modified firewall alias address via API";    // Add a change note
        $config["aliases"]["alias"][$a_index] = $curr_ent;    // Write to our master config
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        // Loop through each alias and see if our alias was added successfully
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
        $api_resp["message"] = "alias address modified";
        $api_resp["data"] = $config["aliases"]["alias"][$a_index];
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

