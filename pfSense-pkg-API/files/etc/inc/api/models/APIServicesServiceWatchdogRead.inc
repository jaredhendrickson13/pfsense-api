<?php
//   Copyright 2022 Jared Hendrickson
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


class APIServicesServiceWatchdogRead extends APIModel {

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-servicewatchdog"];
        $this->packages = ["pfSense-pkg-Service_Watchdog"];
        $this->package_includes = ["servicewatchdog.inc"];
    }

    public function action() {

        return APIResponse\get(0, $this->get_watched_services());
    }

    public function get_watched_services() {
        global $config;
        $services = ($config["installedpackages"]["servicewatchdog"]["item"]) ?: [];

        # Loop through each configured service and ensure data is formatted correctly
        foreach ($services as &$service) {
            $service["enabled"] = isset($service["enabled"]);
            $service["status"] = isset($service["status"]);
            $service["notify"] = isset($service["notify"]);
        }

        return $services;
    }
}
