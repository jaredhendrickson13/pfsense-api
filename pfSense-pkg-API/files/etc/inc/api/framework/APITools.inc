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
require_once("priv.defs.inc");
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

// Sorts nat rules by specified criteria and reloads the filter
function sort_nat_rules($mode=null, $data=null) {
    // Variables
    global $config;
    $sort_arr = [];
    $master_arr = [];
    foreach ($config["nat"]["rule"] as $idx => $fre) {
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
    $config["nat"]["rule"] = $master_arr;
}

// Input a physical interface ID, or a descriptive interface name, and return the pfSense interface ID (lan,wan,optx)
function get_pfsense_if_id($interface)
{
    // Variables
    global $err_lib, $config;
    // Loop through our config and check each interface for a physical ID match
    foreach ($config["interfaces"] as $ifid => $ife) {
        // Check that we have a configured interface ID first
        if (array_key_exists("if", $ife)) {
            // Check if the interface id matches
            if ($interface === $ife["if"]) {
                return $ifid;    // Return our pfSense interface ID
                break;    // Break loop just in case
            }
        }
        // Check that we have a configured interface descr first
        if (array_key_exists("descr", $ife)) {
            // Check if the interface descr matches
            if ($interface === $ife["descr"]) {
                return $ifid;    // Return our pfSense interface ID
                break;    // Break loop just in case
            }
        } else {
            // Check if the pfSense interface ID matches
            if (strtolower($interface) === $ifid) {
                return $ifid;    // Return our pfSense interface ID
                break;    // Break loop just in case
            }
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
function bring_down_virtual_ip($vip_ent, $id) {
    global $err_lib, $config;
    if ($vip_ent['mode'] == "proxyarp") {
        $viface = $vip_ent['interface'];
        unset($config["virtualip"]["vip"][$id]);
        interface_proxyarp_configure($viface);
    } else {
        interface_vip_bring_down($vip_ent);
        unset($config["virtualip"]["vip"][$id]);
    }
    if (count($config['virtualip']['vip']) == 0) {
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
    global $config;
    $gw_config = $config["gateways"]["gateway_item"];
    $gw_exists = false;
    // Check that we have gateways configured
    if (is_array($gw_config)) {
        // Loop through each gateway and see if name matches
        foreach ($gw_config as $gw_item) {
            if ($gw === $gw_item["name"]) {
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