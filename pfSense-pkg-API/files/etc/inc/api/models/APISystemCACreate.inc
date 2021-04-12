<?php
//   Copyright 2021 Jared Hendrickson
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


class APISystemCACreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-certmanager"];
        $this->change_note = "Added CA certificate via API";
    }

    public function action() {
        $this->config["ca"][] = $this->validated_data;    // Add our configuration
        $this->write_config();    // Apply our configuration change
        // Print our JSON response
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
        } else {
            $this->errors[] = APIResponse\get(1002);
        }
        if (isset($this->initial_data['method'])) {
            // Check what method was picked
            if ($this->initial_data['method'] === "import") {
                $ca = base64_decode($this->initial_data["ca"]);
                // Check if our CA certificate is valid
                if (!strstr($ca, "BEGIN CERTIFICATE") || !strstr($ca, "END CERTIFICATE")) {
                    $this->errors[] = APIResponse\get(1003);
                }
                // Populate our configuration array
                $this->validated_data["refid"] = uniqid();
                $this->validated_data["descr"] = $descr;
                ca_import($this->validated_data, $ca);
            }
        }
    }
}
