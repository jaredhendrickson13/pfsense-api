<?php
include_once("util.inc");
include_once("guiconfig.inc");
require_once("api.inc");

// Variables
global $config;    // Define our config globally
$pgtitle = array(gettext('System'), gettext('API'), gettext('Settings'));    // Save ui path array
include_once("head.inc");    // Write our header, this must be done after defining pgtitle
$tab_array   = array();    // Init our tabs
$tab_array[] = array(gettext("Settings"), true, "/api/");    // Define our page tabs
display_top_tabs($tab_array, true);    // Ensure the tabs are written to the top of page
$user = $_SESSION["Username"];    // Save our username
$sec_client_id = bin2hex($user);    // Save our secure username client ID (token mode)
$b64_client_id = base64_encode($user);    // Save a base64 encoded version of our username
$pkg_config = get_api_configuration();    // Save our entire pkg config
$pkg_index = $pkg_config[0];    // Save our pkg configurations index value
$api_config = $pkg_config[1];    // Save our api configuration from our pkg config
$available_auth_modes = array("local" => "Local Database", "base64" => "Base64", "token" => "API Token");
$available_hash_algos = array("sha256" => "SHA256", "sha384" => "SHA384", "sha512" => "SHA512", "md5" => "MD5");
$available_key_bytes = array("16", "32", "64");    // Save our allowed key bitlengths
$non_config_ifs = array("any" => "Any", "localhost" => "Link-local");    // Save non-configurable interface ids
$availabe_api_if = array_merge($non_config_ifs, get_configured_interface_with_descr(true));    // Combine if arrays

// UPON POST
if ($_POST["gen"] === "1") {
    $new_key = api_generate_token($user);
    print_apply_result_box(0, "\nSave this API key somewhere safe, it cannot be viewed again: \n".$new_key);
}
if (isset($_POST["del"]) and is_numeric($_POST["del"])) {
    $del_key = $_POST["del"];
    unset($config["installedpackages"]["package"][$pkg_index]["conf"]["keys"]["key"][$del_key]);
    $change_note = " Deleted API key";
    write_config(sprintf(gettext($change_note)));
    print_apply_result_box(0);
}
if (isset($_POST["save"])) {
    // Save enable value to config
    if (isset($_POST["enable"])) {
        $api_config["enable"] = "";
    } else {
        unset($api_config["enable"]);
    }
    // Save allowed interface value to config
    if (isset($_POST["allowed_interfaces"])) {
        $api_config["allowed_interfaces"] = implode(",", $_POST["allowed_interfaces"]);
    }
    // Save authentication mode to config
    if (isset($_POST["authmode"])) {
        $api_config["authmode"] = $_POST["authmode"];
    }
    // Save key hash algos to config
    if (isset($_POST["keyhash"])) {
        $api_config["keyhash"] = $_POST["keyhash"];
    }
    // Save key bit strength to config
    if (isset($_POST["keybytes"])) {
        $api_config["keybytes"] = $_POST["keybytes"];
    }
    // Save key bit strength to config
    if (isset($_POST["readonly"])) {
        $api_config["readonly"] = "";
    } else {
        unset($api_config["readonly"]);
    }
    // Write and apply our changes, leave a session variable indicating save, then reload the page
    $config["installedpackages"]["package"][$pkg_index]["conf"] = $api_config;
    $change_note = " Updated API settings";
    write_config(sprintf(gettext($change_note)));
    print_apply_result_box(0);
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
                                // Pull our current allowed interfaces and select those values
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
                            <span class="help-block">Authentication method API uses to authenticate during API calls. `Local Database` uses basic authentication using your pfSense user/password, `Base 64` uses base64 encoded pfSense user/password, `API Token` generates specific API tokens for API access</span>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label">
                            <span class="element-required">API Key Hash Algorithm</span>
                        </label>
                        <div class="col-sm-10">
                            <select class="form-control" name="keyhash" id="keyhash">
                                <?
                                foreach ($available_hash_algos as $hty => $dhty) {
                                    if ($api_config["keyhash"] === $hty) {
                                        echo "<option value=\"".$hty."\" selected>".$dhty."</option>";
                                    } else {
                                        echo "<option value=\"".$hty."\">".$dhty."</option>";
                                    }
                                }
                                ?>
                            </select>
                            <span class="help-block">Hashing algorithm used to store API keys</span>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label">
                            <span class="element-required">API Key Bytes</span>
                        </label>
                        <div class="col-sm-10">
                            <select class="form-control" name="keybytes" id="keybytes">
                                <?
                                foreach ($available_key_bytes as $bty) {
                                    if ($api_config["keybytes"] === $bty) {
                                        echo "<option value=\"".$bty."\" selected>".$bty."</option>";
                                    } else {
                                        echo "<option value=\"".$bty."\">".$bty."</option>";
                                    }
                                }
                                ?>
                            </select>
                            <span class="help-block">Bit strength used when generating API keys</span>
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
                <div class="col-sm-10 col-sm-offset-2"><button class="btn btn-primary" type="submit" value="Save" name="save" id="save"><i class="fa fa-save icon-embed-btn"> </i>Save</button></div>
        </form>
    </div>
    <?
        // Pull credentials if configured
        $user_creds = api_get_existing_tokens($user);
        // Print HTML depending on authmode
        if ($api_config["authmode"] === "token") {
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
        }
    ?>
    <?
        if ($api_config["authmode"] === "token") {
            echo "<nav class=\"action-buttons\">";
            echo "    <a class=\"btn btn-sm btn-success\" href=\"/api/?gen=1\" usepost>";
            echo "        <i class=\"fa fa-plus icon-embed-btn\"></i>";
            echo "        Generate	</a>";
            echo "</nav>";
        }
    ?>
<?php
include('foot.inc');
?>