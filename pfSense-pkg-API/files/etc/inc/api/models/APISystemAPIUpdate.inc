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

        # Backup and sync changes to HA nodes if enabled
        $this->backup();
        $this->sync();

        # Remove sensitive values
        unset($this->validated_data["server_key"]);
        unset($this->validated_data["hasync_password"]);

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

    private function __validate_enable_login_protection() {
        # Check for our optional 'enable_login_protection' payload value
        if ($this->initial_data['enable_login_protection'] === true) {
            $this->validated_data["enable_login_protection"] = "";
        } elseif ($this->initial_data['enable_login_protection'] === false) {
            unset($this->validated_data["enable_login_protection"]);
        }
    }

    private function __validate_log_successful_auth() {
        # Check for our optional 'log_successful_auth' payload value
        if ($this->initial_data['log_successful_auth'] === true) {
            $this->validated_data["log_successful_auth"] = "";
        } elseif ($this->initial_data['log_successful_auth'] === false) {
            unset($this->validated_data["log_successful_auth"]);
        }
    }

    private function __validate_hasync() {
        # Check for our optional 'hasync' payload value
        if ($this->initial_data['hasync'] === true) {
            $this->validated_data["hasync"] = "";
        } elseif ($this->initial_data['hasync'] === false) {
            unset($this->validated_data["hasync"]);
        }
    }

    private function __validate_allow_options() {
        # Check for our optional 'allow_options' payload value
        if ($this->initial_data['allow_options'] === true) {
            $this->validated_data["allow_options"] = "";
        } elseif ($this->initial_data['allow_options'] === false) {
            unset($this->validated_data["allow_options"]);
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

    private function __validate_hasync_hosts() {
        # Only validate this field if HA sync is enabled
        if ($this->validated_data["hasync"]) {
            # Validate our required 'hasync_hosts' payload value
            if (isset($this->initial_data["hasync_hosts"]) and count($this->initial_data["hasync_hosts"]) > 0) {
                # Loop through each host and ensure it is valid
                foreach ($this->initial_data["hasync_hosts"] as $hasync_host) {
                    if (!is_ipaddr($hasync_host) and !is_fqdn($hasync_host)) {
                        $this->errors[] = APIResponse\get(1028);
                    }
                }
                $this->validated_data["hasync_hosts"] = implode(" ", $this->initial_data["hasync_hosts"]);
            } elseif (empty($this->validated_data["hasync_hosts"])) {
                $this->errors[] = APIResponse\get(1027);
            }
        }
    }

    private function __validate_hasync_username() {
        # Only validate this field if HA sync is enabled
        if ($this->validated_data["hasync"]) {
            # Validate our required 'hasync_username' payload value
            if (isset($this->initial_data["hasync_username"])) {
                $this->validated_data["hasync_username"] = strval($this->initial_data["hasync_username"]);
            } elseif (empty($this->validated_data["hasync_username"])) {
                $this->errors[] = APIResponse\get(1029);
            }
        }
    }

    private function __validate_hasync_password() {
        # Only validate this field if HA sync is enabled
        if ($this->validated_data["hasync"]) {
            # Validate our required 'hasync_password' payload value
            if (isset($this->initial_data["hasync_password"])) {
                $this->validated_data["hasync_password"] = strval($this->initial_data["hasync_password"]);
            } elseif (empty($this->validated_data["hasync_password"])) {
                $this->errors[] = APIResponse\get(1029);
            }
        }
    }

    private function __validate_custom_headers() {
        # Check for our optional 'custom_headers' payload value
        if (isset($this->initial_data["custom_headers"]) and !empty($this->initial_data["custom_headers"])) {
            if (is_array($this->initial_data["custom_headers"])) {
                # Loop through each requested header and ensure types are valid
                foreach ($this->initial_data["custom_headers"] as $key=>$value) {
                    if (!is_string($key) or !is_string($value)) {
                        $this->errors[] = APIResponse\get(1026);
                        break;
                    }
                }
                $this->validated_data["custom_headers"] = $this->initial_data["custom_headers"];
            } else {
                $this->errors[] = APIResponse\get(1025);
            }
        }
        # When the custom_headers was passed in but is empty, unset custom headers
        elseif (isset($this->initial_data["custom_headers"]) and empty($this->initial_data["custom_headers"])) {
            unset($this->validated_data["custom_headers"]);
        }
    }

    private function __validate_access_list() {
        # Validate subnets within the specified access list
        if (!empty($this->initial_data["access_list"])) {
            # Check each subnet within the access list to ensure it is valid
            foreach ($this->initial_data["access_list"] as $subnet) {
                if (!is_subnet($subnet)) {
                    $this->errors[] = APIResponse\get(1072);
                    break;
                }
            }
            $this->validated_data["access_list"] = implode(" ", $this->initial_data["access_list"]);
        }
        elseif ($this->initial_data["access_list"] === []) {
            $this->validated_data["access_list"]["access_list"] = "";
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

    public function backup() {
        if (isset($this->validated_data["persist"])) {
            shell_exec("/usr/local/share/pfSense-pkg-API/manage.php backup");
        }
    }

    public function sync() {
        # Use ob_start()/ob_end_clean() to prevent sync() from printing output
        ob_start();
        APITools\sync();
        ob_end_clean();
    }

    public function validate_payload() {
        $this->__validate_enable();
        $this->__validate_persist();
        $this->__validate_readonly();
        $this->__validate_allow_options();
        $this->__validate_allowed_interfaces();
        $this->__validate_authmode();
        $this->__validate_jwt_exp();
        $this->__validate_keyhash();
        $this->__validate_keybytes();
        $this->__validate_custom_headers();
        $this->__validate_access_list();
        $this->__validate_enable_login_protection();
        $this->__validate_log_successful_auth();
        $this->__validate_hasync();
        $this->__validate_hasync_hosts();
        $this->__validate_hasync_username();
        $this->__validate_hasync_password();
    }
}
