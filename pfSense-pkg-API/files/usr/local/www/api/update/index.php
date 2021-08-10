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

include_once("util.inc");
include_once("guiconfig.inc");
require_once("api/framework/APITools.inc");
require_once("api/models/APISystemAPIVersionRead.inc");

# Initialize the pfSense UI page (note: $pgtitle must be defined before including head.inc)
$pgtitle = array(gettext('System'), gettext('API'), gettext('Update'));
include('head.inc');
echo "<link rel='stylesheet' href='/css/api.css'/>";
echo "<script type='application/javascript' src='/js/api.js'></script>";
$tab_array = [[gettext("Settings"), false, "/api/"], [gettext("Documentation"), false, "/api/documentation/"], [gettext("Update"), true, "/api/update/"]];
display_top_tabs($tab_array, true);    # Ensure the tabs are written to the top of page

# Variables
global $config;
$form = new Form(false);
$curr_ver = APISystemAPIVersionRead::get_api_version();
$latest_ver = APISystemAPIVersionRead::get_latest_api_version();
$latest_ver_date = date("Y-m-d", strtotime(APISystemAPIVersionRead::get_latest_api_release_date()));

# On POST, start the update process
if ($_POST["confirm"]) {
    # Start the update process in the background and print notice
    shell_exec("nohup pfsense-api update > /dev/null &");
    print_apply_result_box(0, "\nAPI update process has started and is running in the background. Check back in a few minutes.");
}

# Populate our form
$update_section = new Form_Section('Update Settings');
$update_section->addInput(new Form_StaticText('Current Version', $curr_ver));
$update_section->addInput(new Form_StaticText(
    'Latest Version',
    $latest_ver." - <a href='https://github.com/jaredhendrickson13/pfsense-api/releases/tag/v".$latest_ver."'>View Release</a>"." - Released on ".$latest_ver_date
));

# Only display the update button if an update is available
if (APISystemAPIVersionRead::is_update_available()) {
    $form->addGlobal(new Form_Button('confirm', 'Confirm Update', null, 'fa-check'))->addClass('btn btn-sm btn-success');
}

# Display our populated form
$form->add($update_section);
print $form;

include('foot.inc');
