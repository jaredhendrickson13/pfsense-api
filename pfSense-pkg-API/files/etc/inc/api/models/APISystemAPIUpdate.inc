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


class APISystemAPIUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-api"];
        $this->change_note = "Modified API settings via API";
        $this->validated_data = APITools\get_api_config()[1];
    }

    public function action() {
        $this->config["installedpackages"]["package"][APITools\get_api_config()[0]]["conf"] = $this->validated_data;
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_enable() {
        # Check for our optional 'enable' payload value
        if ($this->initial_data['enable'] === true) {
            $this->validated_data["enable"] = "";
        } elseif ($this->initial_data['enable'] === false) {
            unset($this->validated_data["enable"]);
        }
    }

    private function __validate_persist() {
        # Check for our optional 'persist' payload value
        if ($this->initial_data['persist'] === true) {
            $this->validated_data["persist"] = "";
        } elseif ($this->initial_data['persist'] === false) {
            unset($this->validated_data["persist"]);
        }
    }

    private function __validate_readonly() {
        # Check for our optional 'readonly' payload value
        if ($this->initial_data['readonly'] === true) {
            $this->validated_data["readonly"] = "";
        } elseif ($this->initial_data['readonly'] === false) {
            unset($this->validated_data["readonly"]);
        }
    }

    private function __validate_allowed_interfaces() {
        # Local variables
        $non_config_ifs = array("any" => "Any", "localhost" => "Link-local");
        $available_ifs = array_merge($non_config_ifs, get_configured_interface_with_descr(true));

        # Check for our optional 'allowed_interfaces' payload value
        if (isset($this->initial_data['allowed_interfaces'])) {
            $this->validated_data["allowed_interfaces"] = [];
            # Loop through each requested interface and ensure it is valid
            foreach ($this->initial_data["allowed_interfaces"] as $if) {
                # Convert the interface to the pfSense interface ID if it exists, otherwise leave original input.
                $if = (APITools\get_pfsense_if_id($if)) ? APITools\get_pfsense_if_id($if) : $if;

                # Check that this interface exists
                if (array_key_exists($if, $available_ifs)) {
                    $this->validated_data["allowed_interfaces"][] = $if;
                } else {
                    $this->errors[] = APIResponse\get(1020);
                }
            }

            # Convert value to internal XML value
            $this->validated_data["allowed_interfaces"] = implode(",", $this->validated_data["allowed_interfaces"]);
        }
    }

    private function __validate_authmode() {
        # Check for our option 'authmode' payload value
        if (isset($this->initial_data["authmode"])) {
            # Ensure it is an available option
            if (in_array($this->initial_data["authmode"], ["local", "jwt", "token"])) {
                $this->validated_data["authmode"] = $this->initial_data["authmode"];
            } else {
                $this->errors[] = APIResponse\get(1021);
            }
        }
    }

    private function __validate_jwt_exp() {
        # Check for our option 'jwt_exp' payload value 86400
        if (isset($this->initial_data["jwt_exp"])) {
            # Ensure it is within range
            if ($this->initial_data["jwt_exp"] >= 300 and $this->initial_data["jwt_exp"] <= 86400) {
                $this->validated_data["jwt_exp"] = $this->initial_data["jwt_exp"];
            } else {
                $this->errors[] = APIResponse\get(1022);
            }
        }
    }

    private function __validate_keyhash() {
        # Check for our option 'keyhash' payload value
        if (isset($this->initial_data["keyhash"])) {
            # Ensure it is an available option
            if (in_array($this->initial_data["keyhash"], ["sha256", "sha384", "sha512", "md5"])) {
                $this->validated_data["keyhash"] = $this->initial_data["keyhash"];
            } else {
                $this->errors[] = APIResponse\get(1023);
            }
        }
    }

    private function __validate_keybytes() {
        # Check for our option 'keybytes' payload value
        if (isset($this->initial_data["keybytes"])) {
            # Ensure it is an available option
            if (in_array($this->initial_data["keybytes"], ["16", "32", "64", 16, 32, 64])) {
                $this->validated_data["keybytes"] = $this->initial_data["keybytes"];
            } else {
                $this->errors[] = APIResponse\get(1024);
            }
        }
    }

    public function validate_payload() {
        $this->__validate_enable();
        $this->__validate_persist();
        $this->__validate_readonly();
        $this->__validate_allowed_interfaces();
        $this->__validate_authmode();
        $this->__validate_jwt_exp();
        $this->__validate_keyhash();
        $this->__validate_keybytes();
    }
}