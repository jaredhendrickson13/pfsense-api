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

require_once("api/auto_loader.inc");

global $priv_list;

$priv_list['page-system-api'] = array();
$priv_list['page-system-api']['name'] = "WebCfg - System: API package";
$priv_list['page-system-api']['descr'] = "Allow access to API package UI";
$priv_list['page-system-api']['match'] = array();
$priv_list['page-system-api']['match'][] = "api*";

# Populate privileges for each Endpoint class in \API\Endpoints
foreach(glob("/etc/inc/api/endpoints/*.inc") as $file) {
    # Import classes files and create object
    require_once($file);
    $endpoint_class = "\\API\\Endpoints\\".str_replace(".inc", "", basename($file));
    $endpoint_obj = new $endpoint_class();
    $priv_list += $endpoint_obj->generate_pfsense_privs();
}
