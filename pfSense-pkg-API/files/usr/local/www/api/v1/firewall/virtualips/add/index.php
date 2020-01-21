<?php

// IMPORTS--------------------------------------------------------------------------------------------------------------
require_once("config.inc");
require_once("auth.inc");
require_once("functions.inc");
require_once("filter.inc");
require_once("api.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
api_runtime_allowed();    // Check that our configuration allows this API call to run first
header('Content-Type: application/json');
header("Referer: no-referrer");
session_start();    // Start our session. This is only used for tracking user name

// VARIABLES------------------------------------------------------------------------------------------------------------
global $g, $config, $tracker, $api_resp, $client_id, $client_params;
$user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
$read_only_action = false;    // Set whether this action requires read only access
$req_privs = array("page-all", "page-firewall-virtualipaddress-edit");    // Array of privileges allowing this action
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$allowed_vip_modes = array("ipalias", "carp", "proxyarp", "other");    // Save our allowed vip modes in array

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (CREATE)
    if ($http_method === 'POST') {
        if (isset($client_params['mode'])) {
            $mode = strtolower($client_params['mode']);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 100);
            $api_resp["message"] = "virtual ip mode required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['interface'])) {
            $interface = $client_params['interface'];
            $interface = get_pfsense_if_id($interface);
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 11);
            $api_resp["message"] = "interface required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        if (isset($client_params['subnet'])) {
            $subnet = $client_params['subnet'];
            // If a single IPv4 or IPv6, append the subnet mask for one address
            if (is_ipaddrv4($subnet)) {
                $subnet = $subnet."/32";
            } elseif (is_ipaddrv6($subnet)) {
                $subnet = $subnet."/128";
            }
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 83);
            $api_resp["message"] = "virtual ip subnet required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['descr'])) {
            $descr = $client_params['descr'];
        } else {
            $descr = "";
        }
        if (isset($client_params['noexpand'])) {
            $noexpand = true;
        }
        // CARP required attributes
        if ($mode === "carp" and isset($client_params['vhid'])) {
            $vhid = $client_params['vhid'];
        }
        if ($mode === "carp" and isset($client_params['advskew'])) {
            $advskew = intval($client_params['advskew']);
        } else {
            $advskew = 0;    // Default skew to 0
        }
        if ($mode === "carp" and isset($client_params['advbase'])) {
            $advbase = intval($client_params['advbase']);
        } else {
            $advbase = 1;    // Default base to 0
        }
        if ($mode === "carp" and isset($client_params['password'])) {
            $password = $client_params['password'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 21);
            $api_resp["message"] = "carp password required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }

        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "MODE:" . PHP_EOL;
            echo var_dump($mode) . PHP_EOL;
            echo "INTERFACE:" . PHP_EOL;
            echo var_dump($interface) . PHP_EOL;
            echo "SUBNET:" . PHP_EOL;
            echo var_dump($subnet) . PHP_EOL;
            echo "DESCRIPTION:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;
            echo "DISABLE IP EXPANSION:" . PHP_EOL;
            echo var_dump($noexpand) . PHP_EOL;
            echo "CARP VHID" . PHP_EOL;
            echo var_dump($vhid) . PHP_EOL;
            echo "CARP ADVERTISEMENT BASE" . PHP_EOL;
            echo var_dump($advbase) . PHP_EOL;
            echo "CARP ADVERTISEMENT SKEW" . PHP_EOL;
            echo var_dump($advskew) . PHP_EOL;
            echo "CARP PASSWORD" . PHP_EOL;
            echo var_dump(isset($password)) . PHP_EOL;
        }

        // INPUT VALIDATION/FORMATTING
        $api_resp = array("status" => "bad request", "code" => 400);
        // Check that our required array/interface values are valid
        if (!in_array($mode, $allowed_vip_modes)) {
            $input_err = true;
            $api_resp["message"] = "invalid virtual ip mode";
            $api_resp["return"] = 101;
        } elseif (!is_string($interface)) {
            $input_err = true;
            $api_resp["message"] = "unknown interface";
            $api_resp["return"] = 11;
        } elseif (!is_subnet($subnet)) {
            $input_err = true;
            $api_resp["message"] = "invalid subnet";
            $api_resp["return"] = 13;
        }
        // Split our subnet into an address and subnet mask
        $subnet_split = explode("/", $subnet);
        $subnet = $subnet_split[0];
        $subnet_bits = $subnet_split[1];
        // Check that our subnet is not used elsewhere
        if (!$input_err and is_ip_in_use($subnet)) {
            $input_err = true;
            $api_resp["message"] = "subnet already in use";
            $api_resp["return"] = 14;
        }
        // Check if VHID was specified
        if (!$input_err and isset($vhid)) {
            if (vhid_exists($vhid)) {
                $input_err = true;
                $api_resp["message"] = "vhid already exists";
                $api_resp["return"] = 102;
            } elseif (1 > $vhid or $vhid > 255) {
                $input_err = true;
                $api_resp["message"] = "vhid out of range";
                $api_resp["return"] = 104;
            }
        } elseif ($mode === "carp" and !isset($vhid)) {
            $vhid = next_vhid();    // Pull our next available VHID
        }
        // Check if advertisement base was specified
        if (isset($advbase)) {
            if (1 > $advbase or $advbase > 254) {
                $input_err = true;
                $api_resp["message"] = "carp advertisement out of range";
                $api_resp["return"] = 103;
            }
        }
        // Check if advertisement skew was specified
        if (isset($advskew)) {
            if (0 > $advskew or $advskew > 254) {
                $input_err = true;
                $api_resp["message"] = "carp advertisement out of range";
                $api_resp["return"] = 103;
            }
        }
        // Return error if an error was found during input validation
        if ($input_err) {
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        // Initialize our virtual IP configuration array
        if (!is_array($config["virtualip"]["vip"])) {
            $config["virtualip"] = array("vip" => []);
        }
        // Populate our new virtual IP entry
        $vip_ent = array();
        $vip_ent["mode"] = $mode;
        $vip_ent["interface"] = $interface;
        $vip_ent["type"] = "network";
        $vip_ent["subnet"] = $subnet;
        $vip_ent["subnet_bits"] = $subnet_bits;
        $vip_ent["descr"] = $descr;
        // Values specific to CARP
        if ($mode === "carp") {
            $vip_ent["vhid"] = $vhid;
            $vip_ent["advbase"] = $advbase;
            $vip_ent["advskew"] = $advskew;
            $vip_ent["password"] = $password;
            $vip_ent["uniqid"] = uniqid();
        }
        // Values specific to Proxy ARP and other
        if (in_array($mode, array("proxyarp", "other")) and $noexpand) {
            $vip_ent["noexpand"] = "";
        }
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Added virtual IP via API";    // Add a change note
        $config["virtualip"]["vip"][] = $vip_ent;    // Write to our master config
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        apply_virtual_ip($vip_ent);    // Apply our backend changes with our new configuration
        // Loop through each alias and see if our alias was added successfully
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
        $api_resp["message"] = "virtual ip added";
        $api_resp["data"] = $vip_ent;
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

