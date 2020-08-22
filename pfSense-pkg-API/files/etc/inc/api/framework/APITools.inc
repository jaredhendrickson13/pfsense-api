<?php
namespace APITools;
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

# Gathers our URL form encoded data or JSON body data from our request and places them in a single array
function get_request_data() {
    $data = $_GET;    // Accept HTTP requests in URL encoded format
    // Check if our URL encoded parameters are empty, if so try JSON encoded parameters
    if (empty($data)) {
        $data = json_decode(file_get_contents('php://input'), true);
    }
    return $data;
}

# Check our local pfSense version
function get_pfsense_version() {
    # VARIABLES
    $ver_path = "/etc/version";    // Assign the path to our version file
    $ver_patch_path = "/etc/version.patch";    // Assign the path to our version patch file
    $ver_bt_path = "/etc/version.buildtime";    // Assign the path to our version build time file
    $ver_lc_path = "/etc/version.lastcommit";    // Assign the path to our version last commit file
    $ver_data = array();    // Init an empty array for our version data

    // Check that our files exist, if so read the files. Otherwise return error
    if (file_exists($ver_path)) {
        $ver_file = fopen($ver_path, "r");    // Open our file
        $ver = str_replace(PHP_EOL, "", fread($ver_file, filesize($ver_path)));    // Save our version data
        $ver_data["version"] = $ver;    // Save to array
    }
    if (file_exists($ver_patch_path)) {
        $ver_patch_file = fopen($ver_patch_path, "r");    // Open our file
        $ver_patch = str_replace(PHP_EOL, "", fread($ver_patch_file, filesize($ver_patch_path)));    // Save patch
        $ver_data["patch"] = $ver_patch;    // Save to array
    }
    if (file_exists($ver_bt_path)) {
        $ver_bt_file = fopen($ver_bt_path, "r");    // Open our file
        $ver_bt = str_replace(PHP_EOL, "", fread($ver_bt_file, filesize($ver_bt_path)));    // Save bt data
        $ver_data["buildtime"] = $ver_bt;    // Save to array
    }
    if (file_exists($ver_lc_path)) {
        $ver_lc_file = fopen($ver_lc_path, "r");    // Open our file
        $ver_lc = str_replace(PHP_EOL, "", fread($ver_lc_file, filesize($ver_lc_path)));    // Save bt data
        $ver_data["lastcommit"] = $ver_lc;    // Save to array
    }
    $ver_data["program"] = floatval(str_replace(".", "", explode("-", $ver)[0]).".".$ver_patch);
    return $ver_data;
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
    $pkg_index = get_api_config()[0];    // Save our current API configs pkg index
    $api_config = get_api_config()[1];    // Save our current API config
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
    $token = (is_string($token)) ? $token : "";
    $key = get_api_config()[1]["server_key"];    // Save our current server key
    try {
        $decoded = (array) JWT::decode($token, $key, array('HS256'));
    } catch (\Exception $e) {
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

// Generate new API tokens for token auth mode
function generate_token($username) {
    // Local variables
    global $config;
    $pkg_index = get_api_config()[0];    // Save our current API configs pkg index
    $api_config = get_api_config()[1];    // Save our current API config
    $key_hash_algo = $api_config["keyhash"];    // Pull our configured key hash algorithm
    $key_bit_strength = $api_config["keybytes"];    // Pull our configured key bit strength
    $key_user = bin2hex($username);    // Save our user's dedicated API client-ID
    $key_new = bin2hex(random_bytes(intval($key_bit_strength)));    // Generate a new key
    $key_hash = hash($key_hash_algo, $key_new);    // Hash our key using our configured algos

    // Loop through our existing keys to see
    $api_config["keys"] = !is_array($api_config["keys"]) ? array("key" => []) : $api_config["keys"];
    $api_config["keys"]["key"][] = array("client_id" => $key_user, "client_token" => $key_hash, "algo" => $key_hash_algo);

    // Write our changes
    $config["installedpackages"]["package"][$pkg_index]["conf"] = $api_config;    // Write change to config
    $change_note = " Generated API key";    // Add a change note
    write_config(sprintf(gettext($change_note)));    // Apply our configuration change
    return $key_new;
}

// Check if CARP is enabled for disabled
function is_carp_enabled() {
    // Check current CARP status
    $status = get_single_sysctl('net.inet.carp.allow');
    $enabled = boolval(intval($status) > 0);
    return $enabled;
}

// Check each CARP interface's status
function get_carp_if_status() {
    // Local variables
    global $err_lib, $config;
    $carp_if_stats = [];
    $carp_enabled = is_carp_enabled();
    foreach ($config['virtualip']['vip'] as $carp) {
        if ($carp['mode'] == "carp") {
            $carp_if_ent = [];
            $carp_if_ent["interface"] = $carp["interface"];
            $carp_if_ent["vhid"] = $carp['vhid'];
            $carp_if_ent["subnet"] = $carp['subnet'];
            $carp_if_ent["subnet_bits"] = $carp['subnet_bits'];
            $status = get_carp_interface_status("_vip{$carp['uniqid']}");
            if ($carp_enabled == false) {
                $carp_if_ent["status"] = "disabled";
            } else {
                if ($status == "MASTER") {
                    $carp_if_ent["status"] = "master";
                } else if ($status == "BACKUP") {
                    $carp_if_ent["status"] = "backup";
                } else if ($status == "INIT") {
                    $carp_if_ent["status"] = "init";
                }
            }
            // Add config to our array
            $carp_if_stats[] = $carp_if_ent;
        }
    }
    // Return our status
    return $carp_if_stats;
}

// Enables CARP interfaces
function enable_carp($enable) {
    // Local variables
    global $config;
    $vip_arr = $config['virtualip']['vip'];
    $no_action = (is_carp_enabled() === $enable) ? true : false;    // Check if a change is even requried
    // Disable if $enable is false, enable if $enable is true
    if (!$no_action and $enable === false) {
        set_single_sysctl('net.inet.carp.allow', '0');
        foreach ($vip_arr as $vip) {
            if ($vip['mode'] != "carp" && $vip['mode'] != "ipalias")
                continue;
            if ($vip['mode'] == "ipalias" && substr($vip['interface'], 0, 4) != "_vip")
                continue;
            interface_vip_bring_down($vip);
        }
    } elseif (!$no_action and $enable === true) {
        foreach ($vip_arr as $vip) {
            switch ($vip['mode']) {
                case "carp":
                    interface_carp_configure($vip);
                    break;
                case 'ipalias':
                    if (substr($vip['interface'], 0, 4) == "_vip") {
                        interface_ipalias_configure($vip);
                    }
                    break;
            }
        }
        interfaces_sync_setup();
        set_single_sysctl('net.inet.carp.allow', '1');
    }
}