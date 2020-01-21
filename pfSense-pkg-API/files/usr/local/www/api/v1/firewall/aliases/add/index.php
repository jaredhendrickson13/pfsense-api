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

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (CREATE)
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
        if (isset($client_params['type'])) {
            $type = $client_params['type'];
            $type = trim($type);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 61);
            $api_resp["message"] = "alias type required";
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
        if (isset($client_params['descr'])) {
            $descr = $client_params['descr'];
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
            echo "TYPE:" . PHP_EOL;
            echo var_dump($type) . PHP_EOL;
            echo "DESCRIPTION:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;
            echo "ALIAS VALUES:".PHP_EOL;
            echo var_dump($address).PHP_EOL;
            echo "ALIAS VALUE DESCRIPTIONS:" . PHP_EOL;
            echo var_dump($detail) . PHP_EOL;
        }

        // Check that our input is valid
        $err_code = 10;    // Default our error code to 10 as this applies to most checks
        if (!is_string($name)) {
            $type_err = "alias name must be type string";
        } elseif (!is_string($type)) {
            $type_err = "alias type must be type string";
        } elseif (!in_array($type, $allowed_alias_types)) {
            $type_err = "unknown alias type";
            $err_code = 64;
        } elseif (isset($descr) and !is_string($descr)) {
            $type_err = "alias descr must be type string";
        } elseif (!is_array($address)) {
            $type_err = "alias address must be type array";
        } elseif (isset($detail) and !is_array($detail)) {
            $type_err = "alias detail must be type array";
        }
        if (!isset($type_err)) {
            // Loop through our arrays and ensure the values are valid
            $a_count = 0;   // Define a loop counter
            foreach ($address as $ae) {
                // Conditions for alias type 'port'
                if ($type === "port") {
                    // Check that our value is numeric
                    if (is_numeric($ae)) {
                        if (1 <= intval($ae) and intval($ae) <= 65535) {
                            $address[$a_count] = strval($ae);
                        } else {
                            $type_err = "port out of range";
                            $err_code = 8;
                            break;
                        }
                    } else {
                        $type_err = "alias address contents must be numeric string";
                        break;
                    }
                }
                // Conditionals for alias type 'network'
                if ($type === "network") {
                    // Check that values are strings
                    if (is_string($ae)) {
                        // Check that string is a network CIDR
                        if (strpos($ae, "/")) {
                            $net_ip = explode("/", $ae)[0];    // Save our network IP
                            $bit_mask = explode("/", $ae)[1];    // Save our subnet bit mask
                            // Check if our IP is IPv4
                            if (is_ipaddrv4($net_ip)) {
                                $max_bits = 32;    // Assign our maximum IPv4 bitmask
                            } elseif (is_ipaddrv6($net_ip)) {
                                $max_bits = 128;    // Assign our maximum IPv4 bitmask
                            } else {
                                $type_err = "invalid ipv4 or ipv6 network address";
                                $err_code = 7;
                                break;
                            }
                            // Check if our bitmask is numeric and in range
                            if (is_numeric($bit_mask)) {
                                if (1 <= intval($bit_mask) and intval($bit_mask) <= $max_bits) {
                                    continue;
                                } else {
                                    $type_err = "subnet out of range";
                                    $err_code = 9;
                                    break;
                                }
                            } else {
                                $type_err = "alias address contents must be valid cidr";
                                $err_code = 63;
                                break;
                            }
                        } else {
                            $type_err = "alias address contents must be valid cidr";
                            $err_code = 63;
                            break;
                        }
                    } else {
                        $type_err = "alias address contents must be type string";
                        break;
                    }
                }
                // Conditions for alias type 'host'
                if ($type === "host") {
                    // Check that values are strings
                    if (is_string($ae)) {
                        $address[$a_count] = sanitize_str($ae);
                    } else {
                        $type_err = "alias address contents must be type string";
                        break;
                    }
                }
                // Increase our counter
                $a_count++;
            }
            // Check each of our alias details
            foreach ($detail as $de) {
                if (!is_string($de)) {
                    $type_err = "alias detail contents must be type string";
                    break;
                }
            }
        }
        // Return bad request if error
        if (isset($type_err)) {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => $err_code);
            $api_resp["message"] = $type_err;
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        // Check our existing aliases
        if (is_array($config["aliases"])) {
            $c_count = 0;
            // Loop through each alias and see if alias already exists
            foreach ($config["aliases"]["alias"] as $ce) {
                if ($ce["name"] === $name) {
                    $api_resp = array("status" => "bad request", "code" => 400, "return" => 65);
                    $api_resp["message"] = "alias already exists";
                    http_response_code(400);
                    echo json_encode($api_resp) . PHP_EOL;
                    die();
                }
            }
        }

        // Add our alias
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Added firewall alias via API";    // Add a change note
        $alias_ent = array();   // Init our array
        $alias_ent["name"] = $name;    // Save our alias name
        $alias_ent["type"] = $type;    // Save our type
        $alias_ent["descr"] = $descr;    // Save our description
        $alias_ent["address"] = implode(" ", $address);    // Join array in to space seperated string
        $alias_ent["detail"] = implode("||", $detail);    // Join array in to || seperated string
        $config["aliases"] = !is_array($config["aliases"]) ? array("alias" => []) : $config["aliases"];
        $config["aliases"]["alias"][] = $alias_ent;    // Write our configuration change
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        // Loop through each alias and see if our alias was added successfully
        foreach ($config["aliases"]["alias"] as $se) {
            if ($se["name"] === $name) {
                $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
                $api_resp["message"] = "alias added";
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

