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
$pgtitle = array(gettext('System'), gettext('API'), gettext('Update'));
include('head.inc');
echo "<link rel='stylesheet' href='/css/api/api.css'/>";
echo "<script type='application/javascript' src='/js/api.js'></script>";
$update_tab = (APISystemAPIVersionRead::is_update_available()) ? "Update (New Release Available)" : "Update";
$tab_array = [[gettext("Settings"), false, "/api/"], [gettext("Documentation"), false, "/api/documentation/"], [gettext($update_tab), true, "/api/update/"]];
display_top_tabs($tab_array, true);    # Ensure the tabs are written to the top of page

# Variables
global $config;
$form = new Form(false);
$pf_ver = APITools\get_pfsense_version()["version"];
$curr_ver = APISystemAPIVersionRead::get_api_version();
$latest_ver = APISystemAPIVersionRead::get_latest_api_version();
$latest_ver_date = date("Y-m-d", strtotime(APISystemAPIVersionRead::get_latest_api_release_date()));
$all_vers = APISystemAPIVersionRead::get_all_available_versions();
$curr_ver_msg = (APISystemAPIVersionRead::is_update_available()) ? " - Update available" : " - Up-to-date";


# On POST, start the update process
if ($_POST["confirm"] and !empty($_POST["version"])) {
    # Start the update process in the background and print notice
    shell_exec("nohup pfsense-api revert ".$_POST["version"]." > /dev/null &");
    print_apply_result_box(0, "\nAPI update process has started and is running in the background. Check back in a few minutes.");
}

# Populate our update status form
$update_status_section = new Form_Section('Update Status');
$update_status_section->addInput(new Form_StaticText('Current Version', $curr_ver.$curr_ver_msg));
$update_status_section->addInput(new Form_StaticText(
    'Latest Version',
    $latest_ver." - <a href='https://github.com/jaredhendrickson13/pfsense-api/releases/tag/v".$latest_ver."'>View Release</a>"." - Released on ".$latest_ver_date
));

# Populate our update settings form
$update_settings_section = new Form_Section('Update Settings');
$update_settings_section->addInput(new Form_Select(
    'version',
    'Select Version',
    $latest_ver,
    $all_vers
))->setHelp(
    "Select the version you'd like to update or rollback to. Only releases supporting pfSense ".$pf_ver." are shown. Use 
    caution when reverting to a previous version of pfSense API as this can remove some features and/or introduce 
    vulnerabilities that have since been patched in a later release."
);


# Display our populated form
$form->addGlobal(new Form_Button('confirm', 'Confirm', null, 'fa-check'))->addClass('btn btn-sm btn-success');
$form->add($update_status_section);
$form->add($update_settings_section);
print $form;

include('foot.inc');
