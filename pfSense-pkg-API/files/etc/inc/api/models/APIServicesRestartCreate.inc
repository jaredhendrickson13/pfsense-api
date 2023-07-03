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

class APIServicesRestartCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-services"];
    }

    public function action() {
        if (isset($this->validated_data["service"])) {
            service_control_restart($this->validated_data["service"], []);
            $services = ["service"=>$this->validated_data["service"], "status"=>"restarted"];
            return APIResponse\get(0, $services);
        } else {
            $services = get_services();
            // Loop through our service list and add our service status
            foreach ($services as $key => $srvc) {
                service_control_restart($srvc["name"], []);    // Start our service
                $services[$key]["status"] = "restarted";
            }
            return APIResponse\get(0, $services);
        }
    }

    private function __validate_service() {
        # Check if specified service exists
        if (isset($this->initial_data["service"])) {
            $services = get_services();
            # Loop over services to find specified service
            $found = false;
            foreach($services as $key => $srvc) {
                if ($this->initial_data["service"] === $srvc["name"]) {
                    $found = true;
                    break;
                }
            }
            if (!$found) {
                $this->errors[] = APIResponse\get(2158);
            } else {
                $this->validated_data["service"] = $this->initial_data["service"];
            }
        }
    }

    public function validate_payload() {
        $this->__validate_service();
    }
}
