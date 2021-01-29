<?php
//   Copyright 2021 Jared Hendrickson
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

# Variables
global $config;    # Define our config globally
$pgtitle = array(gettext('System'), gettext('API'), gettext('Settings'));    # Save ui path array
include_once("head.inc");    # Write our header, this must be done after defining pgtitle
$tab_array   = array();    # Init our tabs
$tab_array[] = array(gettext("Settings"), true, "/api/");    # Define our page tabs
$tab_array[] = array(gettext("Documentation"), false, "/api/documentation/");    # Define our page tabs
display_top_tabs($tab_array, true);    # Ensure the tabs are written to the top of page
$user = $_SESSION["Username"];    # Save our username
$user_privs = get_user_privileges(getUserEntry($user));
$sec_client_id = bin2hex($user);    # Save our secure username client ID (token mode)
$pkg_config = APITools\get_api_config();    # Save our entire pkg config
$pkg_index = $pkg_config[0];    # Save our pkg configurations index value
$api_config = $pkg_config[1];    # Save our api configuration from our pkg config
$available_auth_modes = array("local" => "Local Database", "token" => "API Token", "jwt" => "JWT");
$available_hash_algos = array("sha256" => "SHA256", "sha384" => "SHA384", "sha512" => "SHA512", "md5" => "MD5");
$available_key_bytes = array("16", "32", "64");    # Save our allowed key bitlengths
$non_config_ifs = array("any" => "Any", "localhost" => "Link-local");    # Save non-configurable interface ids
$availabe_api_if = array_merge($non_config_ifs, get_configured_interface_with_descr(true));    # Combine if arrays

# Redirect user if they do not have privilege to access this page
if (!in_array("page-system-api", $user_privs) and !in_array("page-all", $user_privs)) {
    header("Location: /");
    exit();
}

# UPON POST
if ($_POST["gen"] === "1") {
    $new_key = APITools\generate_token($user);
    print_apply_result_box(0, "\nSave this API key somewhere safe, it cannot be viewed again: \n".$new_key);
}
# Rotate JWT server key requested
if ($_POST["rotate_server_key"] === "1") {
    $config["installedpackages"]["package"][$pkg_index]["conf"]["keys"] = [];
    APITools\create_jwt_server_key(true);
    print_apply_result_box(0, "\nRotated server key.\n");
}

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
        $api_config["enable"] = "";
    } else {
        unset($api_config["enable"]);
    }
    # Save allowed interface value to config
    if (isset($_POST["allowed_interfaces"])) {
        $api_config["allowed_interfaces"] = implode(",", $_POST["allowed_interfaces"]);
    }
    # Save authentication mode to config
    if (isset($_POST["authmode"])) {
        $api_config["authmode"] = $_POST["authmode"];
    }
    # Save JWT expiration value to coonfig
    if (isset($_POST["jwt_exp"])) {
        $api_config["jwt_exp"] = $_POST["jwt_exp"];
    }
    # Save key hash algos to config
    if (isset($_POST["keyhash"])) {
        $api_config["keyhash"] = $_POST["keyhash"];
    }
    # Save key bit strength to config
    if (isset($_POST["keybytes"])) {
        $api_config["keybytes"] = $_POST["keybytes"];
    }
    # Save persist value to config
    if (isset($_POST["persist"])) {
        $api_config["persist"] = "";
    } else {
        unlink("/usr/local/share/pfSense-pkg-API/backup.json");
        unset($api_config["persist"]);
    }
    # Save our read only value
    if (isset($_POST["readonly"])) {
        $api_config["readonly"] = "";
    } else {
        unset($api_config["readonly"]);
    }
    # Write and apply our changes, leave a session variable indicating save, then reload the page
    $config["installedpackages"]["package"][$pkg_index]["conf"] = $api_config;
    $change_note = " Updated API settings";
    write_config(sprintf(gettext($change_note)));
    APITools\create_jwt_server_key();
    print_apply_result_box(0);
}

# Backup our configuration is persist is enabled and the request is a POST request
if(isset($api_config["persist"]) and $_SERVER["REQUEST_METHOD"] === "POST") {
    shell_exec("/usr/local/share/pfSense-pkg-API/manage.php backup");
}

?>
    <div>
        <form class="form-horizontal" method="post" action="/api/">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h2 class="panel-title">API Settings</h2>
                </div>
                <div class="panel-body">
                    <div class="form-group">
                        <label class="col-sm-2 control-label">
                            <span>Enable</span>
                        </label>
                        <div class="checkbox col-sm-10">
                            <?
                            if (isset($api_config["enable"])) {
                                echo "<label class=\"chkboxlbl\"><input name=\"enable\" id=\"enable\" type=\"checkbox\" value=\"yes\" checked=\"checked\"> Enable API</label>";
                            } else {
                                echo "<label class=\"chkboxlbl\"><input name=\"enable\" id=\"enable\" type=\"checkbox\" value=\"yes\"> Enable API</label>";
                            }
                            ?>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label">
                            <span class="element-required">Network Interfaces</span>
                        </label>
                        <div class="col-sm-10">
                            <select class="form-control general" name="allowed_interfaces[]" id="allowed_interfaces[]" multiple="multiple">
                                <?
                                # Pull our current allowed interfaces and select those values
                                $current_api_if = explode(",", $api_config["allowed_interfaces"]);
                                foreach ($availabe_api_if as $aif => $descr) {
                                    if (in_array($aif, $current_api_if)) {
                                        echo "<option value=\"".$aif."\" selected>".$descr."</option>";
                                    } else {
                                        echo "<option value=\"".$aif."\">".$descr."</option>";
                                    }
                                }
                                ?>
                            </select>
                            <span class="help-block">Interface IPs that are allowed to respond to API calls</span>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label">
                            <span class="element-required">Authentication Mode</span>
                        </label>
                        <div class="col-sm-10">
                            <select class="form-control" name="authmode" id="authmode">
                                <?
                                foreach ($available_auth_modes as $cty => $aty) {
                                    if ($api_config["authmode"] === $cty) {
                                        echo "<option value=\"".$cty."\" selected>".$aty."</option>";
                                    } else {
                                        echo "<option value=\"".$cty."\">".$aty."</option>";
                                    }
                                }
                                ?>
                            </select>
                            <span class="help-block">Authentication method API uses to authenticate during API calls. API authentication is further covered in the <a href="/api/documentation/">developer documentation</a>.</span>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label">
                            <span>Persistent Configuration</span>
                        </label>
                        <div class="checkbox col-sm-10">
                            <?
                            if (isset($api_config["persist"])) {
                                echo "<label class=\"chkboxlbl\"><input name=\"persist\" id=\"persist\" type=\"checkbox\" value=\"yes\" checked=\"checked\"> Enable persistent configuration</label>";
                            } else {
                                echo "<label class=\"chkboxlbl\"><input name=\"persist\" id=\"persist\" type=\"checkbox\" value=\"yes\"> Enable persistent configuration</label>";
                            }
                            ?>
                            <span class="help-block">Keep existing API configuration when updating or uninstalling the pfSense API package. If checked, a copy of the API configuration will be kept. If unchecked, all API configuration including API tokens and keys will be lost when updating or uninstalling the package.</span>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label">
                            <span>Read-only</span>
                        </label>
                        <div class="checkbox col-sm-10">
                            <?
                            if (isset($api_config["readonly"])) {
                                echo "<label class=\"chkboxlbl\"><input name=\"readonly\" id=\"readonly\" type=\"checkbox\" value=\"yes\" checked=\"checked\"> Enable read-only access</label>";
                            } else {
                                echo "<label class=\"chkboxlbl\"><input name=\"readonly\" id=\"readonly\" type=\"checkbox\" value=\"yes\"> Enable read-only access</label>";
                            }
                            ?>
                            <span class="help-block">Only allow API calls with read access. Leave unchecked for read/write access</span>
                        </div>
                    </div>
                </div>
    <?
        # Print HTML depending on authmode
        if ($api_config["authmode"] === "jwt") {
            $jwt_exp = $api_config["jwt_exp"];
            echo "<div class='form-group'>".PHP_EOL;
            echo "            <label class='col-sm-2 control-label'>";
            echo "                <span class='element-required'>JWT Expiration</span>".PHP_EOL;
            echo "            </label>".PHP_EOL;
            echo "            <div class='col-sm-10'>".PHP_EOL;
            echo "                <input type='number' min='300' max='86400' class='form-control' name='jwt_exp' id='jwt_exp' value='".strval($jwt_exp)."'>".PHP_EOL;
            echo "                <span class='help-block'>How long (in seconds) the JWT is valid for. Allows a minimum is 300 seconds (5 minutes) and maximum of 86400 seconds (1 day).</span>".PHP_EOL;
            echo "            </div>".PHP_EOL;
            echo "        </div>".PHP_EOL;
            echo "      </div>".PHP_EOL;
            echo "    <button type='submit' id='rotate_server_key' name='rotate_server_key' class='btn btn-sm btn-success' value='1' title='Rotate server key' onclick='return confirm(\"Rotating the server key will void any existng API tokens and JWTs. Proceed?\");'><i class='fa fa-level-up icon-embed-btn'></i>Rotate Server Key</button>".PHP_EOL;
        } elseif ($api_config["authmode"] === "token") {
            echo "<div class='form-group'>".PHP_EOL;
            echo "            <label class='col-sm-2 control-label'>".PHP_EOL;
            echo "                <span class='element-required'>API Key Hash Algorithm</span>".PHP_EOL;
            echo "            </label>".PHP_EOL;
            echo "            <div class='col-sm-10'>".PHP_EOL;
            echo "                <select class='form-control' name='keyhash' id='keyhash'>".PHP_EOL;
            foreach ($available_hash_algos as $hty => $dhty) {
                if ($api_config["keyhash"] === $hty) {
                    echo "<option value='" . $hty . "' selected>" . $dhty . "</option>".PHP_EOL;
                } else {
                    echo "<option value='" . $hty . "'>" . $dhty . "</option>".PHP_EOL;
                }
            }
            echo "                </select>".PHP_EOL;
            echo "                <span class='help-block'>Hashing algorithm used to store API keys</span>".PHP_EOL;
            echo "            </div>".PHP_EOL;
            echo "        </div>".PHP_EOL;
            echo "        <div class='form-group'>".PHP_EOL;
            echo "            <label class='col-sm-2 control-label'>".PHP_EOL;
            echo "                <span class='element-required'>API Key Bytes</span>".PHP_EOL;
            echo "            </label>".PHP_EOL;
            echo "            <div class='col-sm-10'>".PHP_EOL;
            echo "                <select class='form-control' name='keybytes' id='keybytes'>".PHP_EOL;
            foreach ($available_key_bytes as $bty) {
                if ($api_config['keybytes'] === $bty) {
                    echo '<option value=\'' . $bty . '\' selected>' . $bty . '</option>'.PHP_EOL;
                } else {
                    echo '<option value=\'' . $bty . '\'>' . $bty . '</option>'.PHP_EOL;
                }
            }
            echo "               </select>".PHP_EOL;
            echo "                <span class='help-block'>Bit strength used when generating API keys</span>".PHP_EOL;
            echo "            </div>".PHP_EOL;
            echo "        </div>".PHP_EOL;
            echo "    </div>".PHP_EOL;
            echo "    <button type='submit' id='rotate_server_key' name='rotate_server_key' class='btn btn-sm btn-success' value='1' title='Rotate server key' onclick='return confirm(\"Rotating the server key will void any existng API tokens and JWTs. Proceed?\");'><i class='fa fa-level-up icon-embed-btn'></i>Rotate Server Key</button>".PHP_EOL;
        } else {
            echo "    </div>".PHP_EOL;
        }
    ?>
        <button type="submit" id="save" name="save" class="btn btn-sm btn-primary" value="Save" title="Save API configuration"><i class="fa fa-save icon-embed-btn"></i>Save</button>
        <a style="float: right;" class="fa fa-question-circle" href='https://github.com/jaredhendrickson13/pfsense-api/issues/new'> <span style="font-family: 'Helvetica'; font-size: 14px;">Report an Issue</span></a>
        </form>
<!--    <nav class="action-buttons">-->
<!--    </nav>-->
<?php
        if ($api_config["authmode"] === "token") {
            # Pull credentials if configured
            $user_creds = APITools\get_existing_tokens($user);
            echo "<div class=\"panel panel-default\">".PHP_EOL;
            echo "    <div class=\"panel-heading\">".PHP_EOL;
            echo "        <h2 class=\"panel-title\">API Credentials</h2>".PHP_EOL;
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
                    echo "                        <td>" . $user . "</td>" . PHP_EOL;
                    echo "                        <td>" . $sec_client_id . "</td>" . PHP_EOL;
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
?>