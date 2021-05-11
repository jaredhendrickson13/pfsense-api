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
    public $cert_method;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-camanager"];
        $this->change_note = "Added CA certificate via API";
        $this->cert_method = null;
    }

    public function action() {
        # Add our new CA to the config
        $this->config["ca"][] = $this->validated_data;
        $this->write_config();

        # Run setup actions
        $this->import_ca();
        ca_setup_trust_store();

        // Print our JSON response
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Create a unique reference ID for this CA and validate globally available fields
        $this->validated_data["refid"] = uniqid();
        $this->__validate_method();
        $this->__validate_descr();
        $this->__validate_trust();
        $this->__validate_randomserial();

        # Validate method specific fields
        if ($this->cert_method === "import") {
            $this->__validate_crt();
            $this->__validate_prv();
            $this->__validate_serial();
        }
    }

    public function import_ca() {
        # Only import the CA if no errors were found and the method was import
        if (empty($this->errors) and $this->cert_method === "import") {
            ca_import($this->validated_data, $this->validated_data["crt"], $this->validated_data["prv"], $this->validated_data["serial"]);
        }
    }

    private function __validate_method() {
        if (isset($this->initial_data['method'])) {
            # Ensure the method is a supported one
            if (in_array($this->initial_data["method"], ["import"])) {
                $this->cert_method = $this->initial_data["method"];
            } else {
                $this->errors[] = APIResponse\get(1032);
            }
        } else {
            $this->errors[] = APIResponse\get(1031);
        }
    }

    private function __validate_descr() {
        # Validate the required 'descr' field
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        } else {
            $this->errors[] = APIResponse\get(1002);
        }
    }

    private function __validate_crt() {
        $crt = base64_decode($this->initial_data["crt"]);

        # Check if our CA certificate is valid
        if (!strstr($crt, "BEGIN CERTIFICATE") || !strstr($crt, "END CERTIFICATE")) {
            $this->errors[] = APIResponse\get(1003);
        } else {
            $this->validated_data["crt"] = $this->initial_data["crt"];
        }
    }

    private function __validate_prv() {
        # Check for our optional 'prv' field
        if (isset($this->initial_data["prv"])) {
            $crt = base64_decode($this->initial_data["crt"]);
            $key = base64_decode($this->initial_data["prv"]);

            # Check if our certificate and key matches
            if (cert_get_publickey($crt, false) != cert_get_publickey($key, false, 'prv')) {
                $this->errors[] = APIResponse\get(1004);
            } else {
                $this->validated_data["prv"] = $this->initial_data["prv"];
            }
        }
    }

    private function __validate_serial() {
        # Check for our optional 'serial' field
        if (isset($this->initial_data["serial"])) {
            # Ensure the serial number is 1 or greater
            if (is_numeric($this->initial_data["serial"]) and intval($this->initial_data["serial"]) >= 1) {
                $this->validated_data["serial"] = intval($this->initial_data["serial"]);
            } else {
                $this->errors[] = APIResponse\get(1033);
            }
        } else {
            $this->validated_data["serial"] = 1;
        }
    }

    private function __validate_trust() {
        # Check our optional trust field
        if ($this->initial_data["trust"] === true) {
            $this->validated_data["trust"] = "enabled";
        }
    }

    private function __validate_randomserial() {
        # Check our optional randomserial field
        if ($this->initial_data["randomserial"] === true) {
            $this->validated_data["randomserial"] = "enabled";
        }
    }
}
