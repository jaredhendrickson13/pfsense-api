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


class APIServicesServiceWatchdogUpdate extends APIModel {
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->packages = ["pfSense-pkg-Service_Watchdog"];
        $this->package_includes = ["servicewatchdog.inc"];
        $this->change_note = "Modified Service Watchdog configuration via API";
    }

    public function action() {
        # Write the updated configuration and reload service watchdog if changes were made
        if ($this->validated_data) {
            $this->set_config("installedpackages/servicewatchdog/item", $this->validated_data);
            $this->write_config();
            servicewatchdog_cron_job();
        }

        return APIResponse\get(0, APIServicesServiceWatchdogRead::get_watched_services());
    }

    public function validate_payload() {
        $this->__validate_services();
    }

    private function __validate_services() {
        # Validate the optional 'services' field
        if (isset($this->initial_data["services"])) {
            # Variables
            $watched_services = [];

            # Revert validated data so it can be repopulated with the updated values
            $this->validated_data = [];

            # Loop through each requested service and configure it accordingly
            foreach ($this->initial_data["services"] as $service) {
                # Ensure this item is an array
                $service = (is_array($service)) ? $service : [];

                # Require the 'name' subfield
                if (!isset($service["name"])) {
                    $this->errors[] = APIResponse\get(2259);
                }
                # Require this service to be watchable
                elseif (!$this->is_service_watchable($service["name"])) {
                    $this->errors[] = APIResponse\get(2260);
                }
                # Do not allow duplicates of the same service
                elseif (in_array($service["name"], $watched_services)) {
                    $this->errors[] = APIResponse\get(2261);
                }
                # Otherwise, this service is valid.
                else {
                    # Mark this service as watched so duplicates will not be allowed
                    $watched_services[] = $service["name"];

                    # Get the configuration for this service
                    $serv_conf = $this->is_service_watchable($service["name"]);

                    # Allow clients to set the notify field
                    if ($service["notify"] === true) {
                        $serv_conf["notify"] = true;
                    }

                    # Add this service to our validated configuration
                    $this->validated_data[] = $serv_conf;
                }
            }
        }
    }

    public function is_service_watchable($name) {
        # Loop through compatible services and check if this service is found
        foreach (get_services() as $service) {
            if ($service["name"] === $name) {
                return $service;
            }
        }
        return false;
    }
}
