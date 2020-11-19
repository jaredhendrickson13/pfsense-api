<?php
//   Copyright 2020 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

namespace APITools;
require_once("php-jwt/src/JWT.php");
require_once("php-jwt/src/ExpiredException.php");
require_once("php-jwt/src/SignatureInvalidException.php");
require_once("php-jwt/src/BeforeValidException.php");
require_once("config.inc");
require_once("util.inc");
require_once("interfaces.inc");
require_once("interfaces_fast.inc");
require_once("priv.defs.inc");
require_once("service-utils.inc");
require_once("filter.inc");
require_once("shaper.inc");
require_once("auth.inc");
require_once("functions.inc");
use Firebase\JWT\JWT;

# Checks our content type header and parses the content accordingly
function get_request_data() {
    $content_types = [
        "application/x-www-form-urlencoded" => $_GET,
        "application/json" => json_decode(file_get_contents('php://input'), true)
    ];
    # If client passed in a static content type via header, attempt to parse data using that type
    if (!empty($_SERVER["HTTP_CONTENT_TYPE"])) {
        # Check if content type is supported, if so return corresponding parsed request data
        if (array_key_exists($_SERVER["HTTP_CONTENT_TYPE"], $content_types)) {
            return $content_types[$_SERVER["HTTP_CONTENT_TYPE"]];
        }
        # Return false if the content type is unknown or unsupported
        else {
            return false;
        }
    }
    # If a static content type was not provided, attempt to determine the content type
    else {
        if (!empty($_GET)) {
            return $_GET;
        } else {
            return json_decode(file_get_contents('php://input'), true);
        }
    }
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

// Sorts nat rules by specified criteria and reloads the filter
function sort_nat_rules($top=false, $data=null) {
    // Variables
    global $config;
    $sort_arr = [];

    foreach ($config["nat"]["rule"] as $idx => $fre) {
        # Check if top mode is enabled, if so add this item to the start of the array
        if ($top === true and $idx === $data) {
            array_unshift($sort_arr, $fre);
        } else {
            $sort_arr[] = $fre;
        }
    }
    if (!empty($sort_arr)) {
        $config["nat"]["rule"] = $sort_arr;
    } else {
        unset($config["nat"]["rule"]);
    }
}

# Input a physical interface ID, or a descriptive interface name, and return the pfSense interface ID (lan,wan,optx)
function get_pfsense_if_id($interface) {
    # Variables
    global $config;
    # Loop through our config and check each interface for a physical ID match
    foreach ($config["interfaces"] as $if_id => $if_ent) {
        # First check if the requested interface matches this physical interface ID
        if (array_key_exists("if", $if_ent)) {
            // Check if the interface id matches
            if ($interface === $if_ent["if"]) {
                return $if_id;
            }
        }
        # Next, check if the requested interface matches this pfSense interface description
        if (array_key_exists("descr", $if_ent)) {
            // Check if the interface descr matches
            if ($interface === $if_ent["descr"]) {
                return $if_id;
            }
        }
        # Next, check if the requested interface matches this pfSense interface ID (wan, lan, opt1, opt2, etc.)
        if ($interface === $if_id) {
            return $if_id;
        }
    }
}

// Check if input is valid for rule source and destination
function is_valid_rule_addr($addr, $direction) {
    // Variables
    $addr_types = array("any", "pppoe", "l2tp");   // Array of special src/dst types
    $ret_val = array("valid" => true, "data" => array());
    // Check if our source values are valid
    if (is_string($addr)) {
        // Check src/dst is negated
        if (str_starts_with("!", $addr)) {
            $addr_not = true;
            $addr = str_replace("!", "", $addr);
        }
        // Check if our source data is valid
        $addr_if = str_replace("ip", "", $addr);    // Save seperate variable to check for interface sourcees
        if (is_ipaddr($addr) or is_subnet($addr)) {
            $ret_val["data"] = array($direction => array("address" => $addr));
        } elseif (is_alias($addr)) {
            $ret_val["data"] = array($direction => array("address" => $addr));
        } elseif (get_pfsense_if_id($addr_if)) {
            $addr_pfif = get_pfsense_if_id($addr_if);    // Save our interface pfid
            // If source was interface address (ending in ip), otherwise assume entire subnet
            if (str_replace($addr_if, "", $addr) === "ip") {
                $ret_val["data"] = array($direction => array("network" => $addr_pfif . "ip"));
            } else {
                $ret_val["data"] = array($direction => array("network" => $addr_pfif));
            }
        } elseif (in_array($addr, $addr_types)) {
            if ($addr === "any") {
                $ret_val["data"] = array($direction => array("any" => ""));
            } else {
                $ret_val["data"] = array($direction => array("network" => $addr));
            }
        } else {
            $ret_val["valid"] = false;
        }
        // If source is negated, add not to our array
        if ($addr_not) {
            $ret_val["data"][$direction]["not"] = "";
        }
    } else {
        $ret_val["valid"] = false;
    }
    return $ret_val;
}

// Checks if a given string starts with another given string
function str_starts_with($needle, $haystack) {
    $length = strlen($needle);
    return (substr($haystack, 0, $length) === $needle);
}

// Remove virtual IP and bring down virtual interface
function bring_down_virtual_ip($vip_ent, $id, $delete=true) {
    global $config;
    if ($vip_ent['mode'] == "proxyarp") {
        if ($delete) {
            unset($config["virtualip"]["vip"][$id]);
        }
        $viface = $vip_ent['interface'];
        interface_proxyarp_configure($viface);
    } else {
        interface_vip_bring_down($vip_ent);
        if ($delete) {
            unset($config["virtualip"]["vip"][$id]);
        }
    }
    if ($delete and count($config['virtualip']['vip']) == 0) {
        unset($config['virtualip']['vip']);
    }
}

// Apply changes to virtual IPs
function apply_virtual_ip($vip_ent) {
    $check_carp = false;
    if (is_array($vip_ent)) {
        foreach ($vip_ent as $vid => $ovip) {
            if (!empty($ovip)) {
                interface_vip_bring_down($ovip);
            }
            if ($vip_ent) {
                switch ($vip_ent['mode']) {
                    case "ipalias":
                        interface_ipalias_configure($vip_ent);
                        break;
                    case "proxyarp":
                        interface_proxyarp_configure($vip_ent['interface']);
                        break;
                    case "carp":
                        $check_carp = true;
                        interface_carp_configure($vip_ent);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    /* Before changing check #4633 */
    if ($check_carp === true && !get_carp_status()) {
        set_single_sysctl("net.inet.carp.allow", "1");
    }
    filter_configure();
}

// Find an available virtual IP vhid
function next_vhid() {
    global $err_lib, $config;
    $vhid_config = $config["virtualip"]["vip"];
    $ret_vhid = null;
    # Loop through our range of valid VHID ids
    foreach (range(1, 255) as $idx) {
        $vhid_exists = false;
        # Loop through our virutal IPs and check if this VHID already exists
        foreach ($vhid_config as $vip) {
            if (intval($vip["vhid"]) === intval($idx)) {
                $vhid_exists = true;
            }
        }
        // Check if our VHID was already used
        if (!$vhid_exists) {
            $ret_vhid = $idx;
            break;
        }
    }
    return $ret_vhid;
}

// Checks if an IP is in use elsewhere in our configuration
function is_ip_in_use($ip) {
    global $err_lib, $config;
    $vip_conf = $config["virtualip"]["vip"];
    $if_conf = $config["interfaces"];
    // Check if IP is used in our virtual IP configuration
    foreach ($vip_conf as $vip) {
        if ($ip === $vip["subnet"]) {
            return true;
        }
    }
    // Check if IP is used in our interface configuration
    foreach ($if_conf as $iface) {
        if ($ip === $iface["ipaddr"]) {
            return true;
        } elseif ($ip === $iface["ipaddrv6"]) {
            return true;
        }
    }
    return false;
}

// Sorts filter rules by specified criteria and reloads the filter
function sort_firewall_rules($mode=null, $data=null) {
    // Variables
    global $config;
    $sort_arr = [];
    $master_arr = [];
    foreach ($config["filter"]["rule"] as $idx => $fre) {
        $curr_iface = $fre["interface"];    // Save our current entries interface
        // Create our interface array if does not exist
        if (!isset($sort_arr[$curr_iface])) {
            $sort_arr[$curr_iface] = [];
        }
        // Check if user requested this rule to be placed at the top of array
        if ($mode === "top" and $idx === $data) {
            array_unshift($sort_arr[$curr_iface], $fre);
        } else {
            $sort_arr[$curr_iface][] = $fre;
        }
    }
    foreach ($sort_arr as $if) {
        foreach ($if as $rule) {
            $master_arr[] = $rule;
        }
    }
    $config["filter"]["rule"] = $master_arr;
}

// Checks if inputted routing gateway exists
function is_gateway($gw) {
    // Local variables
    $gw_config = return_gateways_array(true, true);
    $gw_exists = false;
    // Check that we have gateways configured
    if (is_array($gw_config)) {
        // Loop through each gateway and see if name matches
        foreach ($gw_config as $gw_name => $gw_item) {
            if ($gw === $gw_name) {
                $gw_exists = true;
                break;
            }
        }
    }
    return $gw_exists;
}

// Duplicate function from /firewall_aliases.php: accept input and check if alias exists
function alias_find_references($section, $field, $origname, &$is_alias_referenced, &$referenced_by) {
    global $err_lib, $config;
    if (!$origname || $is_alias_referenced) {
        return;
    }
    $sectionref = &$config;
    foreach ($section as $sectionname) {
        if (is_array($sectionref) && isset($sectionref[$sectionname])) {
            $sectionref = &$sectionref[$sectionname];
        } else {
            return;
        }
    }
    if (is_array($sectionref)) {
        foreach ($sectionref as $itemkey => $item) {
            $fieldfound = true;
            $fieldref = &$sectionref[$itemkey];
            foreach ($field as $fieldname) {
                if (is_array($fieldref) && isset($fieldref[$fieldname])) {
                    $fieldref = &$fieldref[$fieldname];
                } else {
                    $fieldfound = false;
                    break;
                }
            }
            if ($fieldfound && $fieldref == $origname) {
                $is_alias_referenced = true;
                if (is_array($item)) {
                    $referenced_by = $item['descr'];
                }
                break;
            }
        }
    }
}

// Input an alias name and check if the alias exists
function alias_in_use($alias_name) {
    $is_alias_referenced = false;
    $referenced_by = false;
    // Firewall rules
    alias_find_references(array('filter', 'rule'), array('source', 'address'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('filter', 'rule'), array('destination', 'address'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('filter', 'rule'), array('source', 'port'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('filter', 'rule'), array('destination', 'port'), $alias_name, $is_alias_referenced, $referenced_by);
    // NAT Rules
    alias_find_references(array('nat', 'rule'), array('source', 'address'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'rule'), array('source', 'port'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'rule'), array('destination', 'address'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'rule'), array('destination', 'port'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'rule'), array('target'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'rule'), array('local-port'), $alias_name, $is_alias_referenced, $referenced_by);
    // NAT 1:1 Rules
    //alias_find_references(array('nat', 'onetoone'), array('external'), $alias_name, $is_alias_referenced, $referenced_by);
    //alias_find_references(array('nat', 'onetoone'), array('source', 'address'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'onetoone'), array('destination', 'address'), $alias_name, $is_alias_referenced, $referenced_by);
    // NAT Outbound Rules
    alias_find_references(array('nat', 'outbound', 'rule'), array('source', 'network'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'outbound', 'rule'), array('sourceport'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'outbound', 'rule'), array('destination', 'address'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'outbound', 'rule'), array('dstport'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('nat', 'outbound', 'rule'), array('target'), $alias_name, $is_alias_referenced, $referenced_by);
    // Alias in an alias
    alias_find_references(array('aliases', 'alias'), array('address'), $alias_name, $is_alias_referenced, $referenced_by);
    // Load Balancer
    alias_find_references(array('load_balancer', 'lbpool'), array('port'), $alias_name, $is_alias_referenced, $referenced_by);
    alias_find_references(array('load_balancer', 'virtual_server'), array('port'), $alias_name, $is_alias_referenced, $referenced_by);
    // Static routes
    alias_find_references(array('staticroutes', 'route'), array('network'), $alias_name, $is_alias_referenced, $referenced_by);
    return $is_alias_referenced;
}

// Strip special characters and replace whitespace with underscore
function sanitize_str($string) {
    $string = str_replace(' ', '_', $string);    // Replace whitespace with underscore
    $string = preg_replace('/[^A-Za-z0-9\-_.]/', '', $string);    // Remove special characters
    return $string;
}

// Sort our state table
function sort_state_table() {
    // Local variables
    $states = shell_exec("/sbin/pfctl -s state");
    $state_table = array();
    // Parse our output
    $state_rows = explode("\n", $states);
    foreach ($state_rows as $id => $data) {
        $data = preg_replace('!\s+!', ' ', $data);    // Remove extra whitespace
        $data_fields = explode(" ", $data);    // Split on whitespace
        if (count($data_fields) > 1) {
            $state_table[$id] = array();
            $state_table[$id]["interface"] = $data_fields[0];
            $state_table[$id]["protocol"] = $data_fields[1];
            $state_table[$id]["source"] = ($data_fields[3] === "->") ? $data_fields[2] : $data_fields[4];
            $state_table[$id]["destination"] = ($data_fields[3] === "->") ? $data_fields[4] : $data_fields[2];
            $state_table[$id]["status"] = $data_fields[5];
        }
    }
    return $state_table;
}

// Restarts the pfSense webConfigurator
function restart_webconfigurator() {
    ob_flush();
    flush();
    log_error(gettext("webConfigurator configuration has changed. Restarting webConfigurator."));
    send_event("service restart webgui");
}

// Parse our ARP table into an array
function get_arp_table() {
    // Local variables
    $arp_cmd = "arp -an";    // Assign the command which reads our ARP table
    exec($arp_cmd, $arp_data);    // Output our ARP table into a string
    $arp_table = array();    // Init our ARP table array
    // Loop through each line of our ARP data and parse into our array
    foreach ($arp_data as $arp_line) {
        $elements = explode(' ', $arp_line, 7);
        $arp_entry = array();
        $arp_entry['ip'] = trim(str_replace(array('(', ')'), '', $elements[1]));
        $arp_entry['mac'] = trim($elements[3]);
        $arp_entry['interface'] = trim($elements[5]);
        $arp_entry['status'] = trim(substr($elements[6], 0, strrpos($elements[6], ' ')));
        $arp_entry['linktype'] = trim(str_replace(array('[', ']'), '', strrchr($elements[6], ' ')));
        $arp_table[] = $arp_entry;
    }
    return $arp_table;
}

// Pull a single ARP entry value from our ARP table
function get_arp_entry($search, $value) {
    // Local variables
    $arp_table = get_arp_table();   // Pull our ARP table
    $arp_match = [];    // Init our match array
    // Loop through ARP table and look for matches
    foreach ($arp_table as $arp_ent) {
        if ($arp_ent[$search] === $value) {
            $arp_match = $arp_ent;
        }
    }
    return $arp_match;
}

// Checks if an authentication server exists by name
function is_authentication_server($name) {
    global $err_lib, $config;
    foreach ($config["system"]["authserver"] as $as) {
        $reserved_names = [$as["name"], "Local_Database", "local", "LOCAL", "Local"];
        if (in_array($name, $reserved_names)) {
            return true;
        }
    }
    return false;
}

// Reload our unbound configuration, restart associated services and clear config locks
function unbound_reload_config() {
    $reload_unbound = 0;
    $reload_unbound |= services_unbound_configure();
    // Check if application was successful
    if ($reload_unbound === 0) {
        system_resolvconf_generate();    // Update resolveconf
        system_dhcpleases_configure();    // Update DHCPD
        clear_subsystem_dirty("unbound");
        return true;
    }
}

// Check if a DNS Resolver (Unbound) host override already exists
function is_unbound_fqdn($hostname, $domain, $instance_id=null) {
    # Local variables
    global $config;
    $curr_hosts = (array_key_exists("hosts", $config["unbound"])) ? $config["unbound"]["hosts"] : [];
    $host_exists = false;
    $index = 0;

    # Loop through each host override and check if the FQDN already exists
    foreach ($curr_hosts as $host_ent) {
        # Check the FQDN matches this entry
        if ($host_ent["host"] === $hostname and $host_ent["domain"] === $domain) {
            # If we are working with an existing instance, allow existing FQDN if ID matches
            if ($index !== $instance_id) {
                return true;
            }
        }

        # Check FQDN within host override aliases as well
        if (is_array($host_ent["aliases"])) {
            foreach ($host_ent["aliases"]["item"] as $alias_ent) {
                if ($alias_ent["host"] === $hostname and $alias_ent["domain"] === $domain) {
                    return true;
                }
            }
        }
        $index++;
    }
    return $host_exists;
}

// Get a complete config list of ALL interfaces. Based off interfaces_assign.php
function get_all_avail_interfaces() {
    // Local variables
    global $err_lib, $config;
    $base_ifs = get_interface_list();    // Get our base interface list, this will be populated with all ifs
    // Add wireless ifs to our array
    if (is_array($config['wireless']['clone']) && count($config['wireless']['clone'])) {
        foreach ($config['wireless']['clone'] as $clone) {
            $base_ifs[$clone['cloneif']] = $clone;
            $base_ifs[$clone['cloneif']]['is_wlclone'] = true;
        }
    }
    // Add VLAN ifs to our array
    if (is_array($config['vlans']['vlan']) && count($config['vlans']['vlan'])) {
        //$timea = microtime(true);
        foreach ($config['vlans']['vlan'] as $vlan) {
            $base_ifs[$vlan['vlanif']] = $vlan;
            $base_ifs[$vlan['vlanif']]['is_vlan'] = true;
        }
    }
    // Add bridge ifs to our array
    if (is_array($config['bridges']['bridged']) && count($config['bridges']['bridged'])) {
        foreach ($config['bridges']['bridged'] as $bridge) {
            $base_ifs[$bridge['bridgeif']] = $bridge;
            $base_ifs[$bridge['bridgeif']]['is_bridge'] = true;
        }
    }
    // Add GIF ifs to our array
    if (is_array($config['gifs']['gif']) && count($config['gifs']['gif'])) {
        foreach ($config['gifs']['gif'] as $gif) {
            $base_ifs[$gif['gifif']] = $gif;
            $base_ifs[$gif['gifif']]['is_gif'] = true;
        }
    }
    // Add GRE ifs to our array
    if (is_array($config['gres']['gre']) && count($config['gres']['gre'])) {
        foreach ($config['gres']['gre'] as $gre) {
            $base_ifs[$gre['greif']] = $gre;
            $base_ifs[$gre['greif']]['is_gre'] = true;
        }
    }
    // Add LAGG ifs to our array
    if (is_array($config['laggs']['lagg']) && count($config['laggs']['lagg'])) {
        foreach ($config['laggs']['lagg'] as $lagg) {
            $base_ifs[$lagg['laggif']] = $lagg;
            $base_ifs[$lagg['laggif']]['is_lagg'] = true;
            /* LAGG members cannot be assigned */
            $lagifs = explode(',', $lagg['members']);
            foreach ($lagifs as $lagif) {
                if (isset($base_ifs[$lagif])) {
                    unset($base_ifs[$lagif]);
                }
            }
        }
    }
    // Add QinQ ifs to our array
    if (is_array($config['qinqs']['qinqentry']) && count($config['qinqs']['qinqentry'])) {
        foreach ($config['qinqs']['qinqentry'] as $qinq) {
            $base_ifs["{$qinq['vlanif']}"]['descr'] = "VLAN {$qinq['tag']} on {$qinq['if']}";
            $base_ifs["{$qinq['vlanif']}"]['is_qinq'] = true;
            /* QinQ members */
            $qinqifs = explode(' ', $qinq['members']);
            foreach ($qinqifs as $qinqif) {
                $base_ifs["{$qinq['vlanif']}.{$qinqif}"]['descr'] = "QinQ {$qinqif} on VLAN {$qinq['tag']} on {$qinq['if']}";
                $base_ifs["{$qinq['vlanif']}.{$qinqif}"]['is_qinq'] = true;
            }
        }
    }
    // Add PPP ifs to our array
    if (is_array($config['ppps']['ppp']) && count($config['ppps']['ppp'])) {
        foreach ($config['ppps']['ppp'] as $pppid => $ppp) {
            $if_name = $ppp['if'];
            $base_ifs[$if_name] = $ppp;
            $base_ifs[$if_name]['is_ppp'] = true;
            $ports_base = basename($ppp['ports']);
            if (isset($ppp['descr'])) {
                $base_ifs[$if_name]['descr'] = strtoupper($ppp['if']). "({$ports_base}) - {$ppp['descr']}";
            } else if (isset($ppp['username'])) {
                $base_ifs[$if_name]['descr'] = strtoupper($ppp['if']). "({$ports_base}) - {$ppp['username']}";
            } else {
                $base_ifs[$if_name]['descr'] = strtoupper($ppp['if']). "({$ports_base})";
            }
        }
    }
    // Add OpenVPN descriptions to our array
    $ovpn_descrs = array();
    if (is_array($config['openvpn'])) {
        if (is_array($config['openvpn']['openvpn-server'])) {
            foreach ($config['openvpn']['openvpn-server'] as $s) {
                $if_name = "ovpns{$s['vpnid']}";
                $base_ifs[$if_name] = $s;
                $ovpn_descrs[$s['vpnid']] = $s['description'];
            }
        }
        if (is_array($config['openvpn']['openvpn-client'])) {
            foreach ($config['openvpn']['openvpn-client'] as $c) {
                $if_name = "ovpnc{$c['vpnid']}";
                $base_ifs[$if_name] = $c;
                $ovpn_descrs[$c['vpnid']] = $c['description'];
            }
        }
    }
    // Add IPsec descriptions to our array
    global $err_lib, $ipsec_descrs;
    $ipsec_descrs = interface_ipsec_vti_list_all();
    foreach ($ipsec_descrs as $ifname => $ifdescr) {
        $base_ifs[$ifname] = array('descr' => $ifdescr);
    }
    // Loop through our array and check if interface is in use
    foreach ($base_ifs as $pid => $conf) {
        $pf_id = get_pfsense_if_id($pid);    // Try to convert to pfSense interface ID
        // Check if our pfSense ID was found
        if (get_pfsense_if_id($pid)) {
            $base_ifs[$pid]["in_use"] = $pf_id;
        }
    }
    return $base_ifs;
}

// Get available media options for a given interface. Modified from interfaces.php
function get_if_media_options($interface, $physical_if=false) {
    // Local variables
    global $err_lib, $config;
    $interface = ($physical_if !== true) ? get_pfsense_if_id($interface) : $interface;
    $interface = ($physical_if !== true) ? $config['interfaces'][$interface]['if'] : $interface;
    $mediaopts_list = array();
    exec("/sbin/ifconfig -m $interface | grep \"media \"", $mediaopts);
    foreach ($mediaopts as $mediaopt) {
        preg_match("/media (.*)/", $mediaopt, $matches);
        if (preg_match("/(.*) mediaopt (.*)/", $matches[1], $matches1)) {
            // there is media + mediaopt like "media 1000baseT mediaopt full-duplex"
            array_push($mediaopts_list, $matches1[1] . " " . $matches1[2]);
        } else {
            // there is only media like "media 1000baseT"
            array_push($mediaopts_list, $matches[1]);
        }
    }
    return $mediaopts_list;
}

// Get our next available pfSense interface ID
function get_next_pfsense_if_id() {
    // Local variables
    global $err_lib, $config;
    $curr_ifs = $config["interfaces"];
    // Check if we have our `wan` or `lan` pf IDs in use, if so, find the next OPTx id
    if (!array_key_exists("wan", $curr_ifs)) {
        return "wan";
    } elseif (!array_key_exists("lan", $curr_ifs)) {
        return "lan";
    } else {
        // Loop until we find an unused OPTx interface
        foreach (range(1, 2000) as $count) {
            // Check if this OPTx ID exists
            $optx = "opt".strval($count);
            if (!array_key_exists($optx, $curr_ifs)) {
                return $optx;
            }
        }
    }
}

// Returns a list of dynamically configured IPv6 interfaces. Modified from interfaces.php function.
function get_ipv6_if_list() {
    global $err_lib, $config, $section;
    $list = array('' => '');
    $interfaces = get_configured_interface_with_descr(true);
    $dyn_v6_ifs = array();
    foreach ($interfaces as $iface => $ifacename) {
        switch ($config['interfaces'][$iface]['ipaddrv6']) {
            case "6to4":
            case "6rd":
            case "dhcp6":
                $dyn_v6_ifs[$iface] = array(
                    'name' => $ifacename,
                    'ipv6_num_prefix_ids' => pow(2, (int) calculate_ipv6_delegation_length($iface)) - 1);
                break;
            default:
                continue 2;
        }
    }
    return($dyn_v6_ifs);
}

// Apply a new interface configuration
function apply_interface_config($if_id, $if_conf) {
    // Local variables
    global $config;
    $vlan_redo = false;
    // Check that our if configuration is an array
    if (is_array($if_conf)) {
        if (isset($config['interfaces'][$if_id]['enable'])) {
            interface_bring_down($if_id, false, $if_conf);
            interface_configure($if_id, true);
            if ($config['interfaces'][$if_id]['ipaddrv6'] == "track6") {
                // Configure IPv6 track6 type if present
                $wan_cfg = $config['interfaces'][$if_id];
                interface_track6_configure($if_id, $wan_cfg, true);
            }
        } else {
            interface_bring_down($if_id, true, $if_conf);
            // Restart DHCPD if enabled
            if (isset($config['dhcpd'][$if_id]['enable']) ||
                isset($config['dhcpdv6'][$if_id]['enable'])) {
                services_dhcpd_configure();
            }
        }
        // Check if VLANs are configured for this interface, if so, reapply them
        if (interface_has_clones(get_real_interface($if_id))) {
            $vlan_redo = true;
        }
    }
    // Check if DHCP needs to be reloaded
    if ($if_conf['ipaddr'] == "dhcp") {
        kill_dhclient_process($if_conf['if']);
    }
    if ($if_conf['ipaddrv6'] == "dhcp6") {
        kill_dhcp6client_process($if_conf['if'],true);
    }
    // Reapply VLANs if necessary
    if ($vlan_redo) {
        interfaces_vlan_configure();
    }
    // Restart services that may reference IP
    services_snmpd_configure();
    setup_gateways_monitor();
    filter_configure();
    enable_rrd_graphing();
    system_routing_configure();
    // Return true or false if configuration was successful
    return true;
}

function delete_static_route($id) {
    global $config;
    $a_routes = &$config['staticroutes']['route'];
    $a_gateways = return_gateways_array(true, true, true);

    if (!isset($a_routes[$id])) {
        return;
    }

    $targets = array();
    if (is_alias($a_routes[$id]['network'])) {
        foreach (filter_expand_alias_array($a_routes[$id]['network']) as $tgt) {
            if (is_ipaddrv4($tgt)) {
                $tgt .= "/32";
            } else if (is_ipaddrv6($tgt)) {
                $tgt .= "/128";
            }
            if (!is_subnet($tgt)) {
                continue;
            }
            $targets[] = $tgt;
        }
    } else {
        $targets[] = $a_routes[$id]['network'];
    }

    foreach ($targets as $tgt) {
        $family = (is_subnetv6($tgt) ? "-inet6" : "-inet");
        $gateway = $a_gateways[$a_routes[$id]['gateway']]['gateway'];
        mwexec("/sbin/route delete {$family} " . escapeshellarg($tgt) . " " . escapeshellarg($gateway));
    }

    unset($targets);
}

# Converts an integer-of-100 percent to a decimal percentage
function float_percent($value) {
    # Only return converted value if it is numeric
    if (is_numeric($value)) {
        # Add a leading 0 if number is single digit
        if (floatval($value) > 0 and floatval($value) < 10) {
            $value = "0" . strval($value);
        }
        return floatval("0." . $value);
    }
}

# Checks if an alias exists with a specified name, and returns the alias type. Returns null if not found
function is_alias($name) {
    global $config;
    if (is_array($config['aliases']['alias'])) {
        foreach ($config['aliases']['alias'] as $alias) {
            if (strcasecmp($alias['name'], $name) == 0) {
                return $alias["type"];
            }
        }
    }
}

# Checks if an interface group with a specified name already exists. Returns true or false.
function is_ifgroup($name) {
    global $config;
    if (is_array($config['ifgroups']['ifgroupentry'])) {
        foreach ($config['ifgroups']['ifgroupentry'] as $if_group_ent) {
            if ($if_group_ent["ifname"] === $name) {
                return true;
            }
        }
    }
    return false;
}

# Checks if an interface has DHCPD enabled. Returns true or false.
function is_dhcpd_enabled($if) {
    global $config;

    # Ensure our interface is in it's pfSense ID format
    $if = get_pfsense_if_id($if);

    # Check if this interface is enabled
    if (isset($config['dhcpd'][$if]['enable'])) {
        return true;
    }

    return false;
}

# Checks if an interface has DHCPDv6 enabled. Returns true or false.
function is_dhcpdv6_enabled($if) {
    global $config;

    # Ensure our interface is in it's pfSense ID format
    $if = get_pfsense_if_id($if);

    # Check if this interface is enabled
    if (isset($config['dhcpdv6'][$if]['enable'])) {
        return true;
    }

    return false;
}