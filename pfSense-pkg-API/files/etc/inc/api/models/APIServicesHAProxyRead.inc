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

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIServicesHAProxyRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->privileges = ["page-all", "page-service-haproxy"];
    }

    public function action() {
	$haproxy_all = $this->get_config("installedpackages/haproxy", []);
        $haproxy_main = [];
	// We only want global settings returned here
	foreach ($haproxy_all as $setting => $value) {
	    if ($setting != "ha_backends" and $setting != "ha_pools") {
		$haproxy_main[$setting] = $value;
	    }
	}
        return APIResponse\get(0, $haproxy_main);
    }
}
