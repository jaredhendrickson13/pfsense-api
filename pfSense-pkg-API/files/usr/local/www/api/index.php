<?php
//    Copyright 2021 Jared Hendrickson
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

# Imports and inits
include_once("util.inc");
include_once("guiconfig.inc");
require_once("api/framework/APITools.inc");

$pgtitle = array(gettext('System'), gettext('API'), gettext('Settings'));
include('head.inc');
$tab_array = [[gettext("Settings"), true, "/api/"], [gettext("Documentation"), false, "/api/documentation/"]];
display_top_tabs($tab_array, true);    # Ensure the tabs are written to the top of page


# Variables
global $config;
$form = new Form(false);
$general_section = new Form_Section('General Settings');
$token_section = new Form_Section('API Token Settings');
$jwt_section = new Form_Section('JWT Settings');
$advanced_section = new Form_Section('Advanced Settings');
$pkg_index = APITools\get_api_config()[0];
$pkg_config = APITools\get_api_config()[1];

# UPON POST
if ($_POST["gen"] === "1") {
    $new_key = APITools\generate_token($_SESSION["Username"]);
    print_apply_result_box(0, "\nSave this API key somewhere safe, it cannot be viewed again: \n".$new_key);
}
# Rotate JWT server key requested
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
    # Save JWT expiration value to coonfig
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
    # Write and apply our changes, leave a session variable indicating save, then reload the page
    $config["installedpackages"]["package"][$pkg_index]["conf"] = $pkg_config;
    $change_note = " Updated API settings";
    write_config(sprintf(gettext($change_note)));
    APITools\create_jwt_server_key();
    print_apply_result_box(0);
}

# Backup our configuration is persist is enabled and the request is a POST request
if(isset($pkg_config["persist"]) and $_SERVER["REQUEST_METHOD"] === "POST") {
    shell_exec("/usr/local/share/pfSense-pkg-API/manage.php backup");
}

# POPULATE THE GENERAL SECTION OF THE UI
$general_section->addInput(new Form_Checkbox(
    'enable',
    'Enable',
    '',
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
    $pkg_config["allowed_interfaces"],
    array_merge(["any" => "Any", "localhost" => "Link-local"], get_configured_interface_with_descr(true)),
    true
));

$general_section->addInput(new Form_Select(
    'authmode',
    'Authentication Mode',
    $pkg_config["authmode"],
    ["local" => "Local Database", "token" => "API Token", "jwt" => "JWT"]
));

# POPULATE THE API TOKEN SECTION OF THE UI
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

# POPULATE THE JWT SECTION OF THE UI
$jwt_section->addInput(new Form_Input(
    'jwt_exp',
    'JWT Expiration',
    'number',
    $pkg_config["jwt_exp"],
    ["min"=>300, "max"=>86400]
))->setHelp(
    "How long (in seconds) the JWT is valid for. Allows a minimum is 300 seconds (5 minutes) and maximum of 
    86400 seconds (1 day)."
);

# POPULATE THE ADVANCED SECTION OF THE UI
$advanced_section->addInput(new Form_Checkbox(
    'allow_options',
    'OPTIONS Method',
    'Allow OPTIONS Request Method',
    $pkg_config["allow_options"]
))->setHelp("Allow API to answer OPTIONS requests. This is sometimes required for integration with frontend web applications.");
$advanced_section->addInput(new Form_Textarea(
    'custom_headers',
    'Custom Response Headers',
    (is_array($pkg_config["custom_headers"])) ? json_encode($pkg_config["custom_headers"]) : ""
))->setHelp(
    'Specify custom response headers to return with API responses. This must be JSON encoded string containing key-value
     pairs (e.g. {"test-header-name": "test-header-value"}). This may be required by some HTTP clients and frameworks.
     For example, this can be used to set CORS policy headers required by frontend web applications.'
);


# POPULATE OUR COMPLETE FORM
$form->add($general_section);
$form->add($advanced_section);

# Only display the Token or JWT sections if they are the selected auth mode
($pkg_config["authmode"] === "token") ? $form->add($token_section) : null;
($pkg_config["authmode"] === "jwt") ? $form->add($jwt_section) : null;
$rotate_btn = new Form_Button('rotate_server_key', 'Rotate server key', null, 'fa-level-up');
$rotate_btn->addClass('btn btn-sm btn-success');
$rotate_btn->setOnclick("return confirm(\"Rotating the server key will void any existng API tokens and JWTs. Proceed?\");");
$form->addGlobal($rotate_btn);
$form->addGlobal(new Form_Button('save', 'Save', null, 'fa-save'))->addClass('btn btn-sm btn-primary api-save-btn');
$form->addGlobal(new Form_Button('report', 'Report an Issue', 'https://github.com/jaredhendrickson13/pfsense-api/issues/new', ''))->addClass('fa fa-question-circle api-report');

# PRINT OUR FORM AND PFSENSE FOOTER
print $form;
//print "<a style=\"float: right;\" class=\"fa fa-question-circle\" href='https://github.com/jaredhendrickson13/pfsense-api/issues/new'> <span style=\"font-family: 'Helvetica'; font-size: 14px;\">Report an Issue</span></a>";

# POPULATE TOKEN TABLE
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