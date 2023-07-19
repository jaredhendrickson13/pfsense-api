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


class APISystemCRLCreate extends APIModel {
    public $cert_method;

    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Added certificate revocation list via API";
        $this->cert_method = null;
    }

    public function action() {
        # Add the new certificate
        $this->id = $this->get_next_id("crl");
        $this->set_config("crl/{$this->id}", $this->validated_data);

        # Write changes to configuration
        $this->write_config();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_caref() {
        # Validate the required 'caref' field
        if (isset($this->initial_data['caref'])) {
            # Ensure CA ref ID exists
            if (in_array($this->initial_data['caref'], $this->get_all_ca())) {
                $this->validated_data["caref"] = $this->initial_data["caref"];
            } else {
                $this->errors[] = APIResponse\get(1048);
            }
        } else {
            $this->errors[] = APIResponse\get(1047);
        }
    }

    private function __validate_method() {
        # Local variables
        $methods = ["existing", "internal"];

        if (isset($this->initial_data['method'])) {
            # Ensure the method is a supported one
            if (in_array($this->initial_data["method"], $methods)) {
                $this->cert_method = $this->initial_data["method"];
                $this->validated_data['method'] = $this->initial_data["method"];
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
            # Ensure description does not contain invalid characters
            if (preg_match("/[\?\>\<\&\/\\\"\']/", $this->initial_data['descr'])) {
                $this->errors[] = APIResponse\get(1037);
            } else {
                $this->validated_data["descr"] = $this->initial_data['descr'];
            }
        } else {
            $this->errors[] = APIResponse\get(1002);
        }
    }

    private function __validate_crl_data() {
        # Validate the required 'crl_data' field
        if (isset($this->initial_data['crl_data'])) {
            $this->validated_data['text'] = base64_encode($this->initial_data['crl_data']);
        } else {
            $this->errors[] = APIResponse\get(1080);
        }
    }

    private function __validate_lifetime() {
        # Validate the optional 'lifetime' field
        if (isset($this->initial_data['lifetime'])) {
            # Ensure lifetime is within range
            if (is_numeric($this->initial_data["lifetime"]) and intval($this->initial_data["lifetime"]) < cert_get_max_lifetime()) {
                $this->validated_data["lifetime"] = strval($this->initial_data["lifetime"]);
            } else {
                $this->errors[] = APIResponse\get(1046);
            }
        } else {
            $this->validated_data["lifetime"] = strval(9999);
        }
    }

    private function __validate_serial() {
        # Check for our optional 'serial' field
        if (isset($this->initial_data["serial"])) {
            # Ensure the serial number is 1 or greater
            if (is_numeric($this->initial_data["serial"]) and intval($this->initial_data["serial"]) >= 0) {
                $this->validated_data["serial"] = strval($this->initial_data["serial"]);
            } else {
                $this->errors[] = APIResponse\get(1033);
            }
        } else {
            $this->validated_data["serial"] = strval(0);
        }
    }

    public function get_all_ca() {
        # Local variables
        $ca_enties = [];

        # Loop through each CA and add all CAs that have a valid key
        foreach ($this->get_config('ca', []) as $ca) {
            if ($ca['prv']) {
                $ca_enties[] = $ca['refid'];
            }
        }

        return $ca_enties;
    }

    public function validate_payload() {
        # Validate shared fields
        $this->validated_data["refid"] = uniqid();
        $this->__validate_method();
        $this->__validate_caref();
        $this->__validate_descr();

        # Validate 'existing' method fields
        if ($this->cert_method === "existing") {
            $this->__validate_crl_data();
        }

        # Validate 'internal' method fields
        if ($this->cert_method === "internal") {
            $this->__validate_serial();
            $this->__validate_lifetime();
        }
    }
}
