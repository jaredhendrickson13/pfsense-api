<?php
//    Copyright 2022 Jared Hendrickson
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

include_once("util.inc");
include_once("guiconfig.inc");
require_once("api/framework/APITools.inc");
require_once("api/models/APISystemAPIVersionRead.inc");

# Initialize the pfSense UI page (note: $pgtitle must be defined before including head.inc)
$pgtitle = array(gettext('System'), gettext('API'), gettext('Settings'));
include('head.inc');
echo "<link rel='stylesheet' href='/css/api/api.css'/>";
echo "<script type='application/javascript' src='/js/api.js'></script>";
$update_tab = (APISystemAPIVersionRead::is_update_available()) ? "Update (New Release Available)" : "Update";
$tab_array = [[gettext("Settings"), true, "/api/"], [gettext("Documentation"), false, "/api/documentation/"], [gettext($update_tab), false, "/api/update/"]];
display_top_tabs($tab_array, true);    # Ensure the tabs are written to the top of page

# Variables
global $config;
$form = new Form(false);
$general_section = new Form_Section('General Settings');
$token_section = new Form_Section('API Token Settings');
$jwt_section = new Form_Section('JWT Settings');
$advanced_section = new Form_Section('Advanced Settings', 'api-advanced-settings');
$pkg_index = APITools\get_api_config()[0];
$pkg_config = APITools\get_api_config()[1];
$input_errors = [];

# Generate new API token if requested
if ($_POST["gen"] === "1") {
    $new_key = APITools\generate_token($_SESSION["Username"]);
    print_apply_result_box(0, "\nSave this API key somewhere safe, it cannot be viewed again: \n".$new_key);
}

# Rotate JWT server key if requested
if ($_POST["rotate_server_key"]) {
    $config["installedpackages"]["package"][$pkg_index]["conf"]["keys"] = [];
    APITools\create_jwt_server_key(true);
    print_apply_result_box(0, "\nRotated server key.\n");
}

# Remove the corresponding API token if a token deletion was requested
if (isset($_POST["del"]) and is_numeric($_POST["del"])) {
    $del_key = $_POST["del"];
    unset($config["installedpackages"]["package"][$pkg_index]["conf"]["keys"]["key"][$del_key]);
    $change_note = " Deleted API key";
    write_config(sprintf(gettext($change_note)));
    print_apply_result_box(0);
}

# Upon normal save, update changed values
if (isset($_POST["save"])) {
    # Save enable value to config
    if (isset($_POST["enable"])) {
        $pkg_config["enable"] = "";
    } else {
        unset($pkg_config["enable"]);
    }

    # Save allowed interface value to config
    if (isset($_POST["allowed_interfaces"])) {
        $pkg_config["allowed_interfaces"] = implode(",", $_POST["allowed_interfaces"]);
    }

    # Save authentication mode to config
    if (isset($_POST["authmode"])) {
        $pkg_config["authmode"] = $_POST["authmode"];
    }

    # Save JWT expiration value to config
    if (isset($_POST["jwt_exp"])) {
        $pkg_config["jwt_exp"] = $_POST["jwt_exp"];
    }

    # Save key hash algos to config
    if (isset($_POST["keyhash"])) {
        $pkg_config["keyhash"] = $_POST["keyhash"];
    }

    # Save key bit strength to config
    if (isset($_POST["keybytes"])) {
        $pkg_config["keybytes"] = $_POST["keybytes"];
    }

    # Save persist value to config
    if (isset($_POST["persist"])) {
        $pkg_config["persist"] = "";
    } else {
        unlink("/usr/local/share/pfSense-pkg-API/backup.json");
        unset($pkg_config["persist"]);
    }

    # Save our read only value
    if (isset($_POST["readonly"])) {
        $pkg_config["readonly"] = "";
    } else {
        unset($pkg_config["readonly"]);
    }

    # Save our allow OPTIONS value
    if (isset($_POST["allow_options"])) {
        $pkg_config["allow_options"] = "";
    } else {
        unset($pkg_config["allow_options"]);
    }

    # Save any custom headers specified
    if (!empty($_POST["custom_headers"])) {
        # Decode the JSON string to ensure it is valid
        $headers = json_decode($_POST["custom_headers"], true);

        # Only save the new value if it was a successfully decoded JSON string
        if (is_array($headers)) {
            # Loop through each requested header and ensure types are valid
            foreach ($headers as $key=>$value) {
                if (!is_string($key) or !is_string($value)) {
                    $input_errors[] = "Custom headers key-value pairs must be string types.";
                    $has_errors = true;
                    break;
                }
            }
            $pkg_config["custom_headers"] = $headers;
        } else {
            $input_errors[] = "Custom headers must be a JSON string containing key-value pairs.";
            $has_errors = true;
        }
    } else {
        unset($pkg_config["custom_headers"]);
    }

    # Validate subnets within the specified access list
    if (!empty($_POST["access_list"])) {
        # Convert access list to array and remove line breaks
        $access_list = explode(" ", $_POST["access_list"]);

        # Check each subnet within the access list to ensure it is valid
        foreach ($access_list as $subnet) {
            if (!is_subnet($subnet)) {
                $input_errors[] = "Access list entry '".$subnet."' is not a valid IPv4 or IPv6 CIDR.";
                $has_errors = true;
                break;
            }
        }
        $pkg_config["access_list"] = $_POST["access_list"];
    } else {
        $pkg_config["access_list"] = "";
    }

    # Validate login protection settings
    if (!empty($_POST["enable_login_protection"])) {
        $pkg_config["enable_login_protection"] = "";
    } else {
        unset($pkg_config["enable_login_protection"]);
    }

    # Validate HA Sync settings if enabled
    if (!empty($_POST["hasync"])) {
        $pkg_config["hasync"] = "";

        # Validate HA Sync Hosts
        $hosts = array_filter(explode(" ", $_POST["hasync_hosts"]));
        # Ensure at least one host was set
        if (count($hosts) > 0) {
            foreach ($hosts as $host) {
                if (!is_ipaddr($host) and !is_fqdn($host)) {
                    $input_errors[] = "Invalid HA Sync host ".$host;
                    $has_errors = true;
                }
            }
            # Write our hosts if valid
            if (!$has_errors) {
                $pkg_config["hasync_hosts"] = $_POST["hasync_hosts"];
            }
        } else {
            $input_errors[] = "At least 1 HA Sync host is required when enabled.";
            $has_errors = true;
        }

        # Validate HA Sync username
        if (!empty($_POST["hasync_username"])) {
            $pkg_config["hasync_username"] = $_POST["hasync_username"];
        } else {
            $input_errors[] = "An HA sync username is required when enabled.";
            $has_errors = true;
        }

        # Validate HA Sync password
        if (!empty($_POST["hasync_password"])) {
            # Ensure password confirmation matches
            if ($_POST["hasync_password"] === $_POST["hasync_password_confirm"]) {
                $pkg_config["hasync_password"] = $_POST["hasync_password"];
            } else {
                $input_errors[] = "HA Sync password confirmation does not match.";
                $has_errors = true;
            }
        } elseif (empty($pkg_config["hasync_password"])) {
            $input_errors[] = "An HA sync password is required when enabled.";
            $has_errors = true;
        }
    } else {
        unset($pkg_config["hasync"]);
    }

    # Only write changes if no errors occurred
    if (!$has_errors) {
        # Write and apply our changes, leave a session variable indicating save, then reload the page
        $config["installedpackages"]["package"][$pkg_index]["conf"] = $pkg_config;
        $change_note = " Updated API settings";
        write_config(sprintf(gettext($change_note)));
        APITools\create_jwt_server_key();
        print_apply_result_box(0);
    } else {
        print_input_errors($input_errors);
    }
}

# Backup our configuration is persist is enabled and the request is a POST request
if(isset($pkg_config["persist"]) and $_SERVER["REQUEST_METHOD"] === "POST") {
    shell_exec("/usr/local/share/pfSense-pkg-API/manage.php backup");
}

# Sync our configuration if HA sync is enabled
if(isset($pkg_config["hasync"]) and $_SERVER["REQUEST_METHOD"] === "POST") {
    # Use ob_start()/ob_end_clean() to prevent sync() from printing output
    ob_start();
    APITools\sync();
    ob_end_clean();
}

# Populate the GENERAL section of the UI form
$general_section->addInput(new Form_Checkbox(
    'enable',
    'Enable',
    'Enable API',
    isset($pkg_config["enable"]),
    ''
));

$general_section->addInput(new Form_Checkbox(
    'readonly',
    'Read-only',
    'Enable read-only access',
    isset($pkg_config["readonly"]),
    ''
))->setHelp("Only allow API calls with read access. Leave unchecked for read/write access.");

$general_section->addInput(new Form_Checkbox(
    'persist',
    'Persistent Configuration',
    'Enable persistent configuration',
    isset($pkg_config["persist"]),
    ''
))->setHelp(
    "Keep existing API configuration when updating or uninstalling the pfSense API package. If checked, a copy of the 
    API configuration will be kept. If unchecked, all API configuration including API tokens and keys will be lost when 
    updating or uninstalling the package."
);

$general_section->addInput(new Form_Select(
    'allowed_interfaces',
    'Network Interfaces',
    explode(",", $pkg_config["allowed_interfaces"]),
    array_merge(["any" => "Any", "localhost" => "Link-local"], get_configured_interface_with_descr(true)),
    true
))->setHelp(
    "Select interfaces that are allowed to respond to API requests."
);

$general_section->addInput(new Form_Select(
    'authmode',
    'Authentication Mode',
    $pkg_config["authmode"],
    ["local" => "Local Database", "token" => "API Token", "jwt" => "JWT"]
))->setHelp(
    "Select the mode used to authenticate API requests. See the <a href='/api/documentation/'>developer documentation</a>
    for more information on API authentication."
);

# Add toggle button to show/hide the advanced settings
$show_adv_btn = new Form_Button('display_advanced', 'Display Advanced', null, 'fa-cog');
$show_adv_btn->setAttribute('type','button')->addClass('btn-info btn-sm')->setOnClick("toggle_advanced_settings()");
$general_section->addInput(new Form_StaticText('Advanced Settings', $show_adv_btn));

### Populate the API TOKEN section of the UI form
$token_section->addInput(new Form_Select(
    'keyhash',
    'Token Hash Algorithm',
    $pkg_config["keyhash"],
    ["sha256" => "SHA256", "sha384" => "SHA384", "sha512" => "SHA512", "md5" => "MD5"]
))->setHelp(
    "Hashing algorithm used when generating API tokens."
);

$token_section->addInput(new Form_Select(
    'keybytes',
    'Token Bit Strength',
    $pkg_config["keybytes"],
    ["16"=>"16", "32"=>"32", "64"=>"64"]
))->setHelp(
    "Bit strength used when generating API tokens."
);

### Populate the JWT section of the UI form
$jwt_section->addInput(new Form_Input(
    'jwt_exp',
    'JWT Expiration',
    'number',
    $pkg_config["jwt_exp"],
    ["min"=>300, "max"=>86400]
))->setHelp(
    "How long (in seconds) the JWT is valid for. Allows a minimum of 300 seconds (5 minutes) and maximum of 
    86400 seconds (1 day)."
);

### Populate the ADVANCED section of the UI form
$advanced_section->addClass("hide-api-advanced-settings");
$advanced_section->addInput(new Form_Checkbox(
    'enable_login_protection',
    'Login Protection',
    'Enable API Login Protection',
    isset($pkg_config["enable_login_protection"])
))->setHelp(
    "Include API authentication in pfSense's Login Protection feature. When enabled, all API authentication requests
    will be logged and monitored for brute force attacks. Login Protection can be configured in 
    <a href='/system_advanced_admin.php'>System > Advanced</a>"
);
$advanced_section->addInput(new Form_Checkbox(
    'hasync',
    'Sync API Configuration',
    'Enable HA Sync',
    isset($pkg_config["hasync"])
))->setHelp("Automatically sync API configuration, keys, and tokens to HA peers.");
$advanced_section->addInput(new Form_Input(
    'hasync_hosts',
    'HA Sync Hosts',
    'text',
    $pkg_config['hasync_hosts']
))->setHelp(
    "Enter the host(s) to sync API configurations to. Multiple hosts may be specified separated by a single space. Each
    host must use the same webConfigurator protocol and port."
);
$advanced_section->addInput(new Form_Input(
    'hasync_username',
    'HA Sync Username',
    'text',
    $pkg_config['hasync_username']
))->setHelp(
    "Enter the username of user to authenticate with when syncing. This user must have access to the API settings page."
);
$advanced_section->addPassword(new Form_Input(
    'hasync_password',
    'HA Sync Password',
    'password',
    ''
))->setHelp(
    'Enter the password of the HA sync user listed above. After saving, the password will be hidden and this field will 
    be blank. '
);


$advanced_section->addInput(new Form_Checkbox(
    'allow_options',
    'OPTIONS Method',
    'Allow OPTIONS Request Method',
    isset($pkg_config["allow_options"])
))->setHelp(
    "Allow API to answer OPTIONS requests. This is sometimes required for integration with frontend web applications."
);

$advanced_section->addInput(new Form_Textarea(
    'custom_headers',
    'Custom Headers',
    (is_array($pkg_config["custom_headers"])) ? json_encode($pkg_config["custom_headers"]) : ""
))->setHelp(
    'Specify custom response headers to return with API responses. This must be JSON encoded string containing key-value
     pairs (e.g. <code>{"test-header-name": "test-header-value"}</code>). This may be required by some HTTP clients and frameworks.
     For example, this can be used to set CORS policy headers required by frontend web applications.'
);

$advanced_section->addInput(new Form_Textarea(
    'access_list',
    'Allowed Networks',
    $pkg_config["access_list"]
))->setHelp(
    'Specify IPv4 or IPv6 subnets (in CIDR notation) that are allowed to interact with the API. Subnets must
    use the network address in the CIDR and not any other address within the subnet. Multiple subnets may be specified
    separated by a space. If a client tries to invoke an API call that is not within an allowed subnet, a 403 error will
    be returned. If left blank, all IPs will be allowed and the access list will essentially be bypassed.'
);

# Populate the entire form
$form->add($general_section);
$form->add($advanced_section);
($pkg_config["authmode"] === "token") ? $form->add($token_section) : null;
($pkg_config["authmode"] === "jwt") ? $form->add($jwt_section) : null;

# Add buttons below the form
$rotate_btn = new Form_Button('rotate_server_key', 'Rotate server key', null, 'fa-repeat');
$rotate_btn->addClass('btn btn-sm btn-success');
$rotate_btn->setOnclick("return confirm(\"Rotating the server key will void any existng API tokens and JWTs. Proceed?\");");
$form->addGlobal(new Form_Button('save', 'Save', null, 'fa-save'))->addClass('btn btn-sm btn-primary api-save-btn');
(in_array($pkg_config["authmode"], ["token", "jwt"])) ? $form->addGlobal($rotate_btn) : null;
$form->addGlobal(new Form_Button('report', 'Report an Issue', 'https://github.com/jaredhendrickson13/pfsense-api/issues/new/choose', ''))->addClass('fa fa-question-circle api-report');

# Display the populated configuration form
print $form;

# POPULATE TOKEN TABLE IF TOKEN AUTH MODE IS SET
if ($pkg_config["authmode"] === "token") {
    # Pull credentials if configured
    $user_creds = APITools\get_existing_tokens($_SESSION["Username"]);
    echo "<div class=\"panel panel-default\">".PHP_EOL;
    echo "    <div class=\"panel-heading\">".PHP_EOL;
    echo "        <h2 class=\"panel-title\">API Tokens</h2>".PHP_EOL;
    echo "        </div>".PHP_EOL;
    echo "    <div class=\"panel-body\">".PHP_EOL;
    echo "        <div class=\"table-responsive\">".PHP_EOL;
    echo "            <table class=\"table table-striped table-hover table-compact sortable-theme-bootstrap\" data-sortable>".PHP_EOL;
    echo "                <thead>".PHP_EOL;
    echo "                    <tr>".PHP_EOL;
    echo "                        <th>USERNAME</th>".PHP_EOL;
    echo "                        <th>CLIENT-ID</th>".PHP_EOL;
    echo "                        <th>CLIENT-TOKEN HASH</th>".PHP_EOL;
    echo "                        <th>HASH ALGORITHM</th>".PHP_EOL;
    echo "                    </tr>".PHP_EOL;
    echo "                </thead>".PHP_EOL;
    echo "                <tbody>".PHP_EOL;
    if (!empty($user_creds)) {
        foreach ($user_creds as $id => $key) {
            $formatted_key = strlen($key["client_token"]) > 20 ? substr($key["client_token"],0,20)."..." : $key["client_token"];
            echo "                    <tr>" . PHP_EOL;
            echo "                        <td>" . $_SESSION["Username"] . "</td>" . PHP_EOL;
            echo "                        <td>" . bin2hex($_SESSION["Username"]) . "</td>" . PHP_EOL;
            echo "                        <td>$formatted_key</td>" . PHP_EOL;
            echo "                        <td>".$key["algo"]."</td>" . PHP_EOL;
            echo "                        <td><a class=\"fa fa-trash\"	title=\"Delete API key\" href=\"/api/?del=".$id."\" usepost></a></td>".PHP_EOL;
            echo "                    </tr>" . PHP_EOL;
        }
    }
    echo "                </tbody>".PHP_EOL;
    echo "            </table>".PHP_EOL;
    echo "        </div>".PHP_EOL;
    echo "    </div>".PHP_EOL;
    echo "</div>".PHP_EOL;
    echo "<nav class=\"action-buttons\">";
    echo "    <a class=\"btn btn-sm btn-success\" href=\"/api/?gen=1\" usepost>";
    echo "        <i class=\"fa fa-plus icon-embed-btn\"></i>";
    echo "        Generate	</a>";
    echo "</nav>";
}

include('foot.inc');
