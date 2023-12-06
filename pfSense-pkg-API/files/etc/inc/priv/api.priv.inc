<?php
//   Copyright 2023 Jared Hendrickson
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

global $priv_list;

$priv_list['page-system-api'] = array();
$priv_list['page-system-api']['name'] = "WebCfg - System: API package";
$priv_list['page-system-api']['descr'] = "Allow access to API package UI";
$priv_list['page-system-api']['match'] = array();
$priv_list['page-system-api']['match'][] = "api*";

# Include the privileges that are auto-generated during endpoint build
if (is_file("/usr/local/share/pfSense-pkg-API/privs.json")) {
    $endpoint_privs_json = file_get_contents("/usr/local/share/pfSense-pkg-API/privs.json");
    $endpoint_privs = json_decode($endpoint_privs_json, associative: true);
    $priv_list += $endpoint_privs;
}
