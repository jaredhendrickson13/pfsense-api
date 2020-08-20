<?php
namespace APITools;
require_once("apiresp.inc");
require_once("php-jwt/src/JWT.php");
require_once("php-jwt/src/ExpiredException.php");
require_once("php-jwt/src/SignatureInvalidException.php");
require_once("php-jwt/src/BeforeValidException.php");
require_once("config.inc");
require_once("util.inc");
require_once("interfaces.inc");
require_once("interfaces_fast.inc");
require_once("service-utils.inc");
require_once("filter.inc");
require_once("shaper.inc");
require_once("auth.inc");
require_once("functions.inc");
use Firebase\JWT\JWT;
use Firebase\JWT\ExpiredException;
use Firebase\JWT\SignatureInvalidException;
use Firebase\JWT\BeforeValidException;

# Gathers our URL form encoded data or JSON body data from our request and places them in a single array
function get_request_data() {
    $data = $_GET;    // Accept HTTP requests in URL encoded format
    // Check if our URL encoded parameters are empty, if so try JSON encoded parameters
    if (empty($data)) {
        $data = json_decode(file_get_contents('php://input'), true);
    }
    return $data;
}

# Locates our API configuration from pfSense's XML configuration. Returns
function get_api_config() {
    global $config;
    $api_pkg_name = "API";
    $pkg_conf = $config["installedpackages"]["package"];
    // Check that our configuration is an array
    if (is_array($pkg_conf)) {
        // Loop through our packages and find our API package config
        foreach ($pkg_conf as $id => $pkg) {
            if ($pkg["name"] === $api_pkg_name) {
                return array($id, $pkg["conf"]);
            }
        }
    }
}

# Checks if a specified user is disabled
function is_user_disabled($username) {
    global $config;
    $users = index_users();
    if (array_key_exists("disabled", $config["system"]["user"][$users[$username]])) {
        return true;
    }
    return false;
}

# Creates JWT server key if one does not exist, or optionally allows rotation of the JWT server key
function create_jwt_server_key($rotate=false) {
    global $config;
    $pkg_index = get_api_configuration()[0];    // Save our current API configs pkg index
    $api_config = get_api_configuration()[1];    // Save our current API config
    # Create a new server key if one is not set
    if (empty($api_config["server_key"]) or $rotate === true) {
        $config["installedpackages"]["package"][$pkg_index]["conf"]["server_key"] = bin2hex(random_bytes(32));
        write_config();
    }
}

# Creates a JWT to use for JWT authentication
function create_jwt($data) {
    global $config;
    $api_config = get_api_config()[1];    // Save our current API config
    $token_exp = $api_config["jwt_exp"];    // Expire token in one hours
    create_jwt_server_key();    // Ensure we have a JWT server key
    $payload = array(
        "iss" => $config["system"]["hostname"],
        "exp" => time() + $token_exp,
        "nbf" => time(),
        "data" => $data
    );
    return JWT::encode($payload, $api_config["server_key"]);
}

# Decodes a JWT using our store server key
function decode_jwt($token) {
    $key = get_api_config()[1]["server_key"];    // Save our current server key
    try {
        $decoded = (array) JWT::decode($token, $key, array('HS256'));
    } catch (Exception $e) {
        $decoded = false;
    }
    return $decoded;
}

# Get our API tokens for a given username
function get_existing_tokens($username) {
    // Local variables
    $api_config = get_api_config()[1];
    $key_user = bin2hex($username);    // Save our user's dedicated API client-ID
    $user_keys = [];
    foreach ($api_config["keys"]["key"] as $id => $key) {
        if ($key["client_id"] === $key_user) {
            $user_keys[$id] = array("client_token" => $key["client_token"], "algo" => $key["algo"]);
        }
    }
    return $user_keys;
}

# Authenticate using an API token
function authenticate_token($cid, $ctoken) {
    $authenticated = false;
    $hex_to_user = pack("H*", $cid);
    // First check if our hex decoded user exists
    if (in_array($hex_to_user, index_users())) {
        // Loop through each of our users API tokens and check if key matches
        foreach (get_existing_tokens($hex_to_user) as $id => $data) {
            $hash_input_key = hash($data["algo"], $ctoken);    // Hash our key using our configured algos
            if ($hash_input_key === $data["client_token"]) {
                $authenticated = true;
                break;
            }
        }
    }
    return $authenticated;
}