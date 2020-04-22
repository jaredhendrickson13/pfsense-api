<?php
require_once("apiresp.inc");
require_once("config.inc");
require_once("util.inc");
require_once("interfaces.inc");
require_once("interfaces_fast.inc");
require_once("service-utils.inc");
require_once("filter.inc");
require_once("shaper.inc");
require_once("auth.inc");
require_once("functions.inc");

// HEADERS--------------------------------------------------------------------------------------------------------------
header("Content-Type: application/json", true);
header("Referer: no-referrer");

// VARIABLES------------------------------------------------------------------------------------------------------------
error_reporting(E_ERROR);    // Suppress warnings and notices for the time being
global $g, $config, $userindex;
$client_params = $_GET;    // Accept HTTP requests in URL encoded format
$read_priv = array("user-config-readonly");    // Read only privileges
$err_lib = api_error_lib();    // Pull our error library from apiresp.inc
$api_resp = array("status" => "unauthorized", "code" => 401, "return" => 3, "message" => $err_lib[3]);  // JSON array

// Check if our URL encoded parameters are empty, if so try JSON encoded parameters
if (empty($client_params)) {
    $client_params = json_decode(file_get_contents('php://input'), true);    // Accept HTTP requests in JSON format
}

// Check for our client-id and client-token
if (isset($client_params['client-id'])) {
    $client_id = $client_params['client-id'];
}
if (isset($client_params['client-token'])) {
    $client_token = $client_params['client-token'];
}

// FUNCTIONS------------------------------------------------------------------------------------------------------------
// Strip special characters and replace whitespace with underscore
function sanitize_str($string) {
    $string = str_replace(' ', '_', $string);    // Replace whitespace with underscore
    $string = preg_replace('/[^A-Za-z0-9\-_.]/', '', $string);    // Remove special characters
    return $string;
}

// Checks if a given string starts with another given string
function str_starts_with($needle, $haystack) {
    $length = strlen($needle);
    return (substr($haystack, 0, $length) === $needle);
}

// Restarts the pfSense webConfigurator
function restart_webconfigurator() {
    ob_flush();
    flush();
    log_error(gettext("webConfigurator configuration has changed. Restarting webConfigurator."));
    send_event("service restart webgui");
}

// API authentication debug function
function api_auth_debug($req_privs=array()) {
    global $err_lib, $client_id, $client_params, $read_priv;
    // Before doing anything, check that debug mode is set
    if (array_key_exists("debug", $client_params)) {
        // Local variables
        $client_config =& getUserEntry($client_id);;
        $client_privs = get_user_privileges($client_config);
        $read_only_priv = true;
        if (array_diff($read_priv, $client_privs)) {
            $read_only_priv = false;
        }
        // Print our debug data
        echo "API CLIENT:" . PHP_EOL;
        echo var_dump($client_id) . PHP_EOL;
        echo "API AUTHENTICATED:" . PHP_EOL;
        echo var_dump(api_authenticate()) . PHP_EOL;
        echo "API CLIENT PRIVILEGES:" . PHP_EOL;
        echo var_dump($client_privs) . PHP_EOL;
        echo "REQUIRED PRIVILEGES:" . PHP_EOL;
        echo var_dump($req_privs) . PHP_EOL;
        echo "READ ONLY:" . PHP_EOL;
        echo var_dump($read_only_priv) . PHP_EOL;
    }
}

// Generate new API tokens for token auth mode
function api_generate_token($username) {
    // Local variables
    global $err_lib, $config;
    $pkg_index = get_api_configuration()[0];    // Save our current API configs pkg index
    $api_config = get_api_configuration()[1];    // Save our current API config
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

// Get our API token ID for a given username
function api_get_existing_tokens($username) {
    // Local variables
    $api_config = get_api_configuration()[1];
    $key_user = bin2hex($username);    // Save our user's dedicated API client-ID
    $user_keys = [];
    foreach ($api_config["keys"]["key"] as $id => $key) {
        if ($key["client_id"] === $key_user) {
            $user_keys[$id] = array("client_token" => $key["client_token"], "algo" => $key["algo"]);
        }
    }
    return $user_keys;
}

// Authenticate using an API token
function api_authenticate_token($cid, $ctoken) {
    $authenticated = false;
    $hex_to_user = pack("H*", $cid);
    // First check if our hex decoded user exists
    if (in_array($hex_to_user, index_users())) {
        // Loop through each of our users API keys and check if key matches
        foreach (api_get_existing_tokens($hex_to_user) as $id => $data) {
            $hash_input_key = hash($data["algo"], $ctoken);    // Hash our key using our configured algos
            if ($hash_input_key === $data["client_token"]) {
                $authenticated = true;
                break;
            }
        }
    }
    return $authenticated;
}

// Check if user authenticates successfully
function api_authenticate() {
    // Local variables
    global $err_lib, $config, $client_id, $client_token;
    $authenticated = false;
    $api_config = get_api_configuration()[1];
    $users = index_users();
    // Format our client ID and token based on our configured auth mode
    if ($api_config["authmode"] === "base64") {
        $client_id = base64_decode($client_id);
        $client_token = base64_decode($client_token);
    }
    // Check if client ID and token are valid, and if user has necessary privileges
    $client_id = trim($client_id);
    $client_token = trim($client_token);
    if ($api_config["authmode"] === "token") {
        if (api_authenticate_token($client_id, $client_token) === true) {
            $client_id = pack("H*", $client_id);
            unset($_SESSION["Username"]);
            $_SESSION["Username"] = $client_id;
            $authenticated = true;
        }
    } else {
        // Check local auth
        $local_auth = authenticate_user($client_id, $client_token);    // Test auth locally
        if ($local_auth === true) {
            unset($_SESSION["Username"]);
            $_SESSION["Username"] = $client_id;
            $authenticated = true;
        }
    }
    // Check if user is disabled
    if (array_key_exists("disabled", $config["system"]["user"][$users[$client_id]])) {
        $authenticated = false;
    }
    return $authenticated;
}

// Check if user is authorized to run API function
function api_authorized($req_privs, $read_only=false) {
    // Local variables
    global $err_lib, $read_priv, $client_id, $api_resp;
    $authorized = false;
    api_auth_debug($req_privs);
    // Check that we can successfully authenticate
    if (api_authenticate() === true) {
        $client_config =& getUserEntry($client_id);;
        $client_privs = get_user_privileges($client_config);
        // Check if API is in read-only mode
        if (is_api_read_only() === $read_only or $read_only) {
            // Loop through each of our req privs and ensure the client has them, also check if access is read only
            foreach ($req_privs as &$priv) {
                // Check if action is not read only
                if ($read_only === false) {
                    if (array_diff($read_priv, $client_privs) and in_array($priv, $client_privs, true)) {
                        $authorized = true;
                        break;
                    }
                } else {
                    if (in_array($priv, $client_privs)) {
                        $authorized = true;
                        break;
                    }
                }
            }
        }
    } else {
        http_response_code(401);
        echo json_encode($api_resp) . PHP_EOL;
        die();
    }
    // If authorization failed, return forbidden
    if ($authorized !== true) {
        http_response_code(401);
        $api_resp["return"] = 4;
        $api_resp["message"] = $err_lib[$api_resp["return"]];
        echo json_encode($api_resp) . PHP_EOL;
        die();
    }
    return $authorized;
}

// Run our pre-runtime checks
function api_runtime_allowed() {
    api_version_check();    // Die if incompatible pfSense version
    api_enabled();    // Die if API is disabled in configuration
    api_whitelist_check();    // Die if server address is not whitelisted
}

// Read our API configuration
function get_api_configuration() {
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

// Check if the API is enabled before answering calls, if not, redirect to wc login
function api_enabled() {
    $api_config = get_api_configuration()[1];
    if (!isset($api_config["enable"])) {
        header("Location: /");
        die();
    }
}

// Check if the API is in read-only mode
function is_api_read_only() {
    // Local variables
    $api_config = get_api_configuration()[1];    // Save our current API config
    if (array_key_exists("readonly", $api_config)) {
        return true;
    } else {
        return false;
    }
}

// Check if our pfSense version is allowed to run this version of API
function api_version_check() {
    # Local variables
    global $err_lib;
    $curr_ver = str_replace(".", "", explode("-", get_pfsense_version()["version"])[0]);
    $min_ver = 244;
    $curr_ver = is_numeric($curr_ver) ? intval($curr_ver) : 0;
    $valid_ver = false;
    if ($curr_ver < $min_ver) {
        http_response_code(501);
        $api_resp = array("status" => "not implemented", "code" => 501, "return" => 5);
        $api_resp["message"] = $err_lib[$api_resp["return"]];
        echo json_encode($api_resp) . PHP_EOL;
        die();
    }
}

// Check if server IP is allowed to answer API calls. Redirects to login if not
function api_whitelist_check() {
    global $err_lib, $config;
    $allowed = false;
    $pkg_data = get_api_configuration();
    $pkg_id = $pkg_data[0];
    $pkg_conf = $pkg_data[1];
    $if_conf = $config["interfaces"];
    $whitelist = array("any");
    $srv_ip = $_SERVER["SERVER_ADDR"];
    // Check that we have a package configuration
    if (is_numeric($pkg_id)) {
        // Override defaults with user specified config
        if (isset($pkg_conf["allowed_interfaces"])) {
            $allow_ifs = $pkg_conf["allowed_interfaces"];
            if (strpos($allow_ifs, ",") !== false) {
                $whitelist = explode(",", $allow_ifs);
            } else {
                $whitelist = array($allow_ifs);
            }
        } else {
            $config["installedpackages"]["package"][$pkg_id]["allowed_interfaces"] = "any";
            write_config();
        }
        // Check if our server IP is in our whitelist
        foreach ($whitelist as $wif) {
            $if_info = get_interface_info($wif);
            // Check if our server IP is a valid if address, localhost, or any
            if ($srv_ip === $if_info["ipaddr"]) {
                $allowed = true;
                break;
            } elseif ($srv_ip === $if_info["ipaddrv6"]) {
                $allowed = true;
                break;
            } elseif ($srv_ip === "127.0.0.1" and $wif === "localhost") {
                $allowed = true;
                break;
            }elseif ($wif === "any") {
                $allowed = true;
                break;
            }
        }
    }
    if (!$allowed) {
        unset($_SESSION);
        $api_resp = array("status" => "forbidden", "code" => 403, "return" => 6);
        $api_resp["message"] = $err_lib[$api_resp["return"]];
        http_response_code(403);
        echo json_encode($api_resp);
        die;
    }
}

// Search JSON response for specific fields
function api_extended_search($base_data, $search_data) {
    global $err_lib;
    $search_failed = false;
    // If client requests an extended search
    if (is_array($base_data) and is_array($search_data)) {
        $target_item = end($search_data);    // Save our last array item
        // Loop through each attribute in the search list and check if it exists
        foreach ($search_data as &$df) {
            if (array_key_exists($df, $base_data)) {
                // If this attribute is our target item, format and return the value
                if ($df === $target_item) {
                    if (!is_array($base_data[$df])) {
                        $base_data = array($df => $base_data[$df]);
                    } else {
                        $base_data = $base_data[$df];
                    }
                    break;
                } else {
                    // Check if this item is an array, if so search the nested array. Otherwise failed
                    if (is_array($base_data[$df])) {
                        $base_data = $base_data[$df];
                    } else {
                        $search_failed = true;
                        break;
                    }
                }
            } else {
                $search_failed = true;
                break;
            }
        }
    } else {
        $search_failed = true;
    }
    // Check if we failed to find our search attributes
    if ($search_failed === true) {
        $api_resp = array("status" => "not found", "code" => 404, "return" => 7);
        $api_resp["message"] = $err_lib[$api_resp["return"]];
        http_response_code(404);
        echo json_encode($api_resp) . PHP_EOL;
        die();
    }
    return $base_data;
}

// Check our local pfSense version
function get_pfsense_version() {
    # VARIABLES
    $ver_path = "/etc/version";    // Assign the path to our version file
    $ver_patch_path = "/etc/version.patch";    // Assign the path to our version patch file
    $ver_bt_path = "/etc/version.buildtime";    // Assign the path to our version build time file
    $ver_lc_path = "/etc/version.lastcommit";    // Assign the path to our version last commit file
    $ver_data = array();    // Init an empty array for our version data
    # RUN TIME
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
function unbound_host_override_exists($hostname, $domain) {
    // Local variables
    global $err_lib, $config;
    $curr_hosts = array();
    $host_exists = false;
    // Check if host override already exists
    if (array_key_exists("hosts", $config["unbound"])) {
        $curr_hosts = $config["unbound"]["hosts"];
    }
    foreach ($curr_hosts as $host_ent) {
        if ($host_ent["host"] === $hostname and $host_ent["domain"] === $domain) {
            $host_exists = true;
            break;
        }
        if (is_array($host_ent["aliases"])) {
            foreach ($host_ent["aliases"]["item"] as $alias_ent) {
                if ($alias_ent["host"] === $hostname and $alias_ent["domain"] === $domain) {
                    $host_exists = true;
                    break;
                }
            }
        }
    }
    return $host_exists;
}

// Check user's DNS Resolver (Unbound) host override alias input for errors
function unbound_parse_aliases($aliases) {
    global $err_lib;
    $aliases_fin = "";    // Default our alias value to blank string
    if (is_array($aliases)) {
        $a_count = 0;
        $aliases_fin = array();
        foreach ($aliases as $alias_ent) {
            // Check that each alias has a valid hostname
            if (array_key_exists("host", $alias_ent) and is_string($alias_ent["host"])) {
                $aliases_fin[$a_count] = array("host" => trim($alias_ent["host"]));
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2007);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                http_response_code($api_resp["code"]);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            }
            // Check that each alias has a valid domain name
            if (array_key_exists("domain", $alias_ent) and is_string($alias_ent["domain"])) {
                $aliases_fin[$a_count]["domain"] = trim($alias_ent["domain"]);
            } else {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2008);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                http_response_code($api_resp["code"]);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            }
            // Check for alias description under descr key
            if (array_key_exists("descr", $alias_ent) and is_string($alias_ent["descr"])) {
                if (is_string($alias_ent["descr"])) {
                    $aliases_fin[$a_count]["description"] = $alias_ent["descr"];
                }
            }
            // Check for alias description under description key
            if (array_key_exists("description", $alias_ent) and is_string($alias_ent["description"])) {
                if (is_string($alias_ent["description"])) {
                    $aliases_fin[$a_count]["description"] = $alias_ent["description"];
                }
            }
            // Increase our counter
            $a_count++;
        }
        // Check if our aliases already exist
        foreach ($aliases_fin as $af) {
            if (unbound_host_override_exists($af["host"], $af["domain"])) {
                $api_resp = array("status" => "bad request", "code" => 400, "return" => 2009);
                $api_resp["message"] = $err_lib[$api_resp["return"]];
                http_response_code($api_resp["code"]);
                echo json_encode($api_resp) . PHP_EOL;
                die();
            }
        }
        // Nest our aliases under the item key
        $aliases_fin = array("item" => $aliases_fin);
    }
    // Return our parsed aliases
    return $aliases_fin;
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

// Input a physical interface ID, or a descriptive interface name, and return the pfSense interface ID (lan,wan,optx)
function get_pfsense_if_id($interface) {
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

// Sorts filter rules by specified criteria and reloads the filter
function sort_firewall_rules($mode=null, $data=null) {
    // Variables
    global $err_lib, $config;
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

// Sorts nat rules by specified criteria and reloads the filter
function sort_nat_rules($mode=null, $data=null) {
    // Variables
    global $err_lib, $config;
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

// Checks if inputted routing gateway exists
function is_gateway($gw) {
    // Local variables
    global $err_lib, $config;
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

// Check if a VHID already exists
function vhid_exists($vhid) {
    global $err_lib, $config;
    $vhid_config = $config["virtualip"]["vip"];
    $vhid_exists = false;
    # Loop through our virutal IPs and check if this VHID already exists
    foreach ($vhid_config as $vip) {
        if (intval($vip["vhid"]) === intval($vhid)) {
            $vhid_exists = true;
        }
    }
    return $vhid_exists;
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
function apply_interface_config($if_conf) {
    // Local variables
    global $err_lib, $config;
    $vlan_redo = false;
    // Check that our if configuration is an array
    if (is_array($if_conf)) {
        foreach ($if_conf as $if_apply => $if_go) {
            if (isset($config['interfaces'][$if_apply]['enable'])) {
                interface_bring_down($if_apply, false, $if_go);
                interface_configure($if_apply, true);
                if ($config['interfaces'][$if_apply]['ipaddrv6'] == "track6") {
                    // Configure IPv6 track6 type if present
                    $wan_cfg = $config['interfaces'][$if_apply];
                    interface_track6_configure($if_apply, $wan_cfg, true);
                }
            } else {
                interface_bring_down($if_apply, true, $if_go);
                // Restart DHCPD if enabled
                if (isset($config['dhcpd'][$if_apply]['enable']) ||
                    isset($config['dhcpdv6'][$if_apply]['enable'])) {
                    services_dhcpd_configure();
                }
            }
            // Check if VLANs are configured for this interface, if so, reapply them
            if (interface_has_clones(get_real_interface($if_apply))) {
                $vlan_redo = true;
            }
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

// Delete an interface
function destroy_interface($id) {
    // Local variables
    global $err_lib, $config;
    $err_msg = "";
    $success = false;
    if ($id === "wan") {
        $err_msg = 3042;
    } elseif (link_interface_to_group($id)) {
        $err_msg = 3043;
    } elseif (link_interface_to_bridge($id)) {
        $err_msg = 3044;
    } elseif (link_interface_to_gre($id)) {
        $err_msg = 3045;
    } elseif (link_interface_to_gif($id)) {
        $err_msg = 3046;
    } elseif (interface_has_queue($id)) {
        $err_msg = 3047;
    } else {
        unset($config['interfaces'][$id]['enable']);
        $realid = get_real_interface($id);
        interface_bring_down($id);   // Bring down interface
        unset($config['interfaces'][$id]);	// Delete our interface from configuration
        // Remove DHCP config for interface
        if (is_array($config['dhcpd']) && is_array($config['dhcpd'][$id])) {
            unset($config['dhcpd'][$id]);
            services_dhcpd_configure('inet');
        }
        // Removed interface config for dhcp6
        if (is_array($config['dhcpdv6']) && is_array($config['dhcpdv6'][$id])) {
            unset($config['dhcpdv6'][$id]);
            services_dhcpd_configure('inet6');
        }
        // Remove ACL for interface
        if (count($config['filter']['rule']) > 0) {
            foreach ($config['filter']['rule'] as $x => $rule) {
                if ($rule['interface'] == $id) {
                    unset($config['filter']['rule'][$x]);
                }
            }
        }
        // Remove NAT config for interface
        if (is_array($config['nat']['rule']) && count($config['nat']['rule']) > 0) {
            foreach ($config['nat']['rule'] as $x => $rule) {
                if ($rule['interface'] == $id) {
                    unset($config['nat']['rule'][$x]['interface']);
                }
            }
        }
        $change_note = " Deleted interface via API";    // Add a change note
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        // Disable DHCP if last interface
        if ($config['interfaces']['lan'] && $config['dhcpd']['wan']) {
            unset($config['dhcpd']['wan']);
        }
        // Update VLAN assignments
        link_interface_to_vlans($realid, "update");
        // Write success return
        $success = true;
    }
    return array("status" => $success, "msg" => $err_msg);
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
    global $err_lib, $config;
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
