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
$req_privs = array("page-all", "page-firewall-rules-edit");    // Array of privileges allowing this action
$http_method = $_SERVER['REQUEST_METHOD'];    // Save our HTTP method
$allowed_rule_types = array("pass", "block", "reject");    // Array of allowed rule types
$allowed_ip_prot = array("inet", "inet6", "inet46");    // Array of allowed IP protocols
$next_rule_id = count($config["filter"]["rule"]);    // Save our next rule ID
// Array of supported protocols
$allowed_prot = array(
    "any",
    "tcp",
    "udp",
    "tcp/udp",
    "icmp",
    "esp",
    "ah",
    "gre",
    "ipv6",
    "igmp",
    "pim",
    "ospf",
    "carp",
    "pfsync"
);
// Array of allowed ICMP subtypes
$icmp_subtypes = array(
    "althost",
    "dataconv",
    "echorep",
    "echoreq",
    "inforep",
    "inforeq",
    "ipv6-here",
    "ipv6-where",
    "maskrep",
    "maskreq",
    "mobredir",
    "mobregrep",
    "mobregreq",
    "paramprob",
    "photuris",
    "redir",
    "routeradv",
    "routersol",
    "skip",
    "squench",
    "timerep",
    "timereq",
    "timex",
    "trace",
    "unreach"
);

// RUN TIME-------------------------------------------------------------------------------------------------------------
// Check that client is authenticated and authorized
if (api_authorized($req_privs, $read_only_action)) {
    // Check that our HTTP method is POST (CREATE)
    if ($http_method === 'POST') {
        if (isset($client_params['type'])) {
            $type = $client_params['type'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 80);
            $api_resp["message"] = "rule type required";
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
        if (isset($client_params['ipprotocol'])) {
            $ipprotocol = $client_params['ipprotocol'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 81);
            $api_resp["message"] = "rule ip protocol required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['protocol'])) {
            $protocol = $client_params['protocol'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 82);
            $api_resp["message"] = "rule protocol required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['src'])) {
            $src = $client_params['src'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 83);
            $api_resp["message"] = "rule source required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['srcport'])) {
            $srcport = $client_params['srcport'];
        }
        if (isset($client_params['dst'])) {
            $dst = $client_params['dst'];
        } else {
            $api_resp = array("status" => "bad request", "code" => 400, "return" => 84);
            $api_resp["message"] = "rule destination required";
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        if (isset($client_params['dstport'])) {
            $dstport = $client_params['dstport'];
        }
        if (isset($client_params['icmptype'])) {
            $icmp_type = $client_params['icmptype'];
            if (!is_array($icmp_type)) {
                $icmp_type = array($icmp_type);
            }
        }
        if (isset($client_params['gateway'])) {
            $gateway = $client_params['gateway'];
        }
        if (isset($client_params['disabled'])) {
            $disabled = true;
        }
        if (isset($client_params['descr'])) {
            $descr = $client_params['descr'];
        }
        if (isset($client_params['log'])) {
            $log = true;
        }
        if (isset($client_params['top'])) {
            $top = "top";
        }

        // Add debug data if requested
        if (array_key_exists("debug", $client_params)) {
            echo "TYPE:" . PHP_EOL;
            echo var_dump($type) . PHP_EOL;
            echo "INTERFACE:" . PHP_EOL;
            echo var_dump($interface) . PHP_EOL;
            echo "IP PROTOCOL:" . PHP_EOL;
            echo var_dump($ipprotocol) . PHP_EOL;
            echo "PROTOCOL:" . PHP_EOL;
            echo var_dump($protocol) . PHP_EOL;
            echo "ICMP SUBTYPES:" . PHP_EOL;
            echo var_dump($icmp_type) . PHP_EOL;
            echo "SOURCE:" . PHP_EOL;
            echo var_dump($src) . PHP_EOL;
            echo "SOURCE PORT:" . PHP_EOL;
            echo var_dump($srcport) . PHP_EOL;
            echo "DESTINATION:" . PHP_EOL;
            echo var_dump($dst) . PHP_EOL;
            echo "DESTINATION PORT:" . PHP_EOL;
            echo var_dump($dstport) . PHP_EOL;
            echo "GATEWAY:" . PHP_EOL;
            echo var_dump($gateway) . PHP_EOL;
            echo "DISABLED:" . PHP_EOL;
            echo var_dump($disabled) . PHP_EOL;
            echo "DESCRIPTION:" . PHP_EOL;
            echo var_dump($descr) . PHP_EOL;
            echo "LOG MATCHES:" . PHP_EOL;
            echo var_dump($log) . PHP_EOL;
            echo "ADD TO TOP:" . PHP_EOL;
            echo var_dump($top) . PHP_EOL;
            echo "NEXT RULE ID:" . PHP_EOL;
            echo var_dump($next_rule_id) . PHP_EOL;
        }

        // INPUT VALIDATION/FORMATTING
        $api_resp = array("status" => "bad request", "code" => 400);
        // Check that our required array/interface values are valid
        if (!in_array($type, $allowed_rule_types)) {
            $input_err = true;
            $api_resp["message"] = "invalid rule type";
            $api_resp["return"] = 85;
        } elseif (!is_string($interface)) {
            $input_err = true;
            $api_resp["message"] = "unknown interface";
            $api_resp["return"] = 11;
        } elseif (!in_array($ipprotocol, $allowed_ip_prot)) {
            $input_err = true;
            $api_resp["message"] = "invalid ip protocol";
            $api_resp["return"] = 86;
        } elseif (!in_array($protocol, $allowed_prot)) {
            $input_err = true;
            $api_resp["message"] = "invalid protocol";
            $api_resp["return"] = 87;
        } elseif (isset($gateway) and !is_gateway($gateway)) {
            $input_err = true;
            $api_resp["message"] = "invalid gateway";
            $api_resp["return"] = 12;
        }
        $rule_ent = array();
        // Check if rule is not disabled
        if (!$disabled) {
            $rule_ent["id"] = "";
        }
        // Check if logging is enabled
        if ($log) {
            $rule_ent["log"] = "";
        }
        // Check if gateway was specified
        if (isset($gateway)) {
            $rule_ent["gateway"] = $gateway;
        }
        $rule_ent["type"] = $type;
        $rule_ent["interface"] = $interface;
        $rule_ent["ipprotocol"] = $ipprotocol;
        $rule_ent["source"] = array();
        $rule_ent["destination"] = array();
        $rule_ent["descr"] = $descr;
        $rule_ent["created"] = array("time" => time(), "username" => $user_created_msg);
        $rule_ent["updated"] = $rule_ent["created"];
        // Save our protocol if it is not 'any'
        if ($protocol !== "any") {
            $rule_ent["protocol"] = $protocol;
        }
        // Add logging to config if enabled
        if ($log) {
            $rule_ent["log"] = "";
        }
        // Check if our source and destination values are valid
        foreach (array("source" => $src, "destination" => $dst) as $dir => $val) {
            $dir_check = is_valid_rule_addr($val, $dir);
            if (!$dir_check["valid"] === true) {
                $input_err = true;
                $api_resp["message"] = $dir_check["message"];
                $api_resp["return"] = $dir_check["return"];
            } else {
                $rule_ent = array_merge($rule_ent, $dir_check["data"]);
            }
        }
        // Check if protocol calls for additional specifications
        if (in_array($protocol, array("tcp", "udp", "tcp/udp"))) {
            $port_req = true;
        } elseif ($protocol === "icmp") {
            // Check if user specified ICMP subtypes
            if (!$input_err and is_array($icmp_type)) {
                // Loop through each of our subtypes
                foreach ($icmp_type as $ict) {
                    if (!in_array($ict, $icmp_subtypes)) {
                        $input_err = true;
                        $api_resp["message"] = "invalid icmp subtype";
                        $api_resp["return"] = 89;
                    }
                }
                // Write our ICMP subtype config
                $rule_ent["icmptype"] = implode(",", $icmp_type);
            }
        }
        // Check our src and dst port values if ports are required
        if (!$input_err and $port_req) {
            if (!isset($srcport) or !isset($dstport)) {
                $input_err = true;
                $api_resp["message"] = "source and destination port required";
                $api_resp["return"] = 8;
            }
            foreach (array("source" => $srcport, "destination" => $dstport) as $dir => $val) {
                if (!is_port_or_range($val) and $val !== "any") {
                    $input_err = true;
                    $api_resp["message"] = "invalid ".$val." port";
                    $api_resp["return"] = 8;
                } elseif ($val !== "any") {
                    $rule_ent[$dir]["port"] = $val;
                }
            }
        }
        // Return error if an error was found during input validation
        if ($input_err) {
            http_response_code(400);
            echo json_encode($api_resp) . PHP_EOL;
            die();
        }
        $_SESSION["Username"] = $client_id;    // Save our CLIENT ID to session data for logging
        $change_note = " Added firewall rule via API";    // Add a change note
        $config["filter"]["rule"][] = $rule_ent;    // Write to our master config
        sort_firewall_rules($top, $next_rule_id);    // Sort our firewall rules
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        // Loop through each alias and see if our alias was added successfully
        $api_resp = array("status" => "ok", "code" => 200, "return" => 0);
        $api_resp["message"] = "firewall rule added";
        $api_resp["data"] = $rule_ent;
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

