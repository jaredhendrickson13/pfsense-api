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

class APIServicesRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-status-services"];
    }

    public function action() {
        $service_list = get_services();    // Stop our service
        // Loop through our service list and add our service status
        foreach ($service_list as $key => $srvc) {
            $s_status = get_service_status($srvc);
            // Check if service is started
            if ($s_status === true) {
                $service_list[$key]["status"] = "running";
            } elseif ($s_status === false) {
                $service_list[$key]["status"] = "stopped";
            } else {
                $service_list[$key]["status"] = "unknown";
            }
            // Check if user requested only one service, if so remove unmatched services
            if (isset($this->validated_data["name"]) and $this->validated_data["name"] !== $srvc["name"]) {
                unset($service_list[$key]);
            }
        }
        return APIResponse\get(0, $service_list);
    }

    public function validate_payload() {
        if (isset($this->initial_data['name'])) {
            $this->validated_data["name"] = $this->initial_data['name'];
        }
    }
}
