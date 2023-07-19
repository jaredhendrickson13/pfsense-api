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


class APIStatusCARPUpdate extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
    }

    public function validate_payload() {
        // Check if user specified enable value
        if (isset($this->initial_data['enable'])) {
            // Check if value is true or false
            if (boolval($this->initial_data['enable'])) {
                $enable = true;
            } else {
                $enable = false;
            }
            $this->validated_data["enable"] = $enable;
        }
        // Check if user specified maintenance mode value
        if (isset($this->initial_data['maintenance_mode'])) {
            // Check if value is true or false
            if (boolval($this->initial_data['maintenance_mode'])) {
                $mm_enable = true;
            } else {
                $mm_enable = false;
            }
            $this->validated_data["maintenance_mode"] = $mm_enable;
        }
    }

    public function action() {
        // Add our CARP settings
        $_SESSION["Username"] = $this->client->username;
        interfaces_carp_set_maintenancemode($this->validated_data["maintenance_mode"]);
        APITools\enable_carp($this->validated_data["enable"]);
        return APIResponse\get(0, $this->validated_data);
    }
}
