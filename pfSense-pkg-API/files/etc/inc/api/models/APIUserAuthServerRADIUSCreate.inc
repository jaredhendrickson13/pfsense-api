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

class APIUserAuthServerRADIUSCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-authserver"];
        $this->change_note = "Added RADIUS authentication server via API";
    }

    public function action() {
        # Add our new authentication server to the configuration
        $this->config["system"]["authserver"][] = $this->validated_data;

        # Check if clients wants to set this as default auth server
        if ($this->initial_data["active"] === true) {
            $this->config['system']['webgui']['authmode'] = $this->validated_data["name"];
        }

        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Set static configuration attributes
        $this->validated_data["type"] = "radius";
        $this->validated_data["refid"] = uniqid();

        # Validate input fields
        $this->__validate_name();
        $this->__validate_host();
        $this->__validate_radius_secret();
        $this->__validate_radius_auth_port();
        $this->__validate_radius_acct_port();
        $this->__validate_radius_protocol();
        $this->__validate_radius_timeout();
        $this->__validate_radius_nasip_attribute();
    }

    private function __validate_name() {
        # Validate our required `name` payload value
        if (isset($this->initial_data["name"])) {
            # Ensure the name is not already in use
            if (!APITools\is_authentication_server($this->initial_data["name"])) {
                $this->validated_data["name"] = $this->initial_data["name"];
            } else {
                $this->errors[] = APIResponse\get(5026);
            }
        } else {
            $this->errors[] = APIResponse\get(5035);
        }
    }

    private function __validate_host() {
        # Validate our required `host` payload value
        if (isset($this->initial_data["host"])) {
            # Ensure the `host` value is a valid IP or hostname
            if (is_ipaddr($this->initial_data["host"]) or is_hostname($this->initial_data["host"])) {
                $this->validated_data["host"] = $this->initial_data["host"];
            } else {
                $this->errors[] = APIResponse\get(5012);
            }
        } else {
            $this->errors[] = APIResponse\get(5011);
        }
    }

    private function __validate_radius_secret() {
        # Validate our required `radius_secret` payload value
        if (isset($this->initial_data["radius_secret"])) {
            # Ensure the `radius_secret` is a string
            if (is_string($this->initial_data["radius_secret"])) {
                $this->validated_data["radius_secret"] = $this->initial_data["radius_secret"];
            } else {
                $this->errors[] = APIResponse\get(5029);
            }
        } else {
            $this->errors[] = APIResponse\get(5028);
        }
    }

    private function __validate_radius_auth_port() {
        # Validate our required `radius_auth_port` payload value
        if (isset($this->initial_data["radius_auth_port"])) {
            # Ensure the `radius_auth_port` is a port
            if (is_port(strval($this->initial_data["radius_auth_port"]))) {
                $this->validated_data["radius_auth_port"] = $this->initial_data["radius_auth_port"];
            } else {
                $this->errors[] = APIResponse\get(5030);
            }
        }
    }

    private function __validate_radius_acct_port() {
        # Validate our required `radius_acct_port` payload value
        if (isset($this->initial_data["radius_acct_port"])) {
            # Ensure the `radius_acct_port` is a port
            if (is_port(strval($this->initial_data["radius_acct_port"]))) {
                $this->validated_data["radius_acct_port"] = $this->initial_data["radius_acct_port"];
            } else {
                $this->errors[] = APIResponse\get(5031);
            }
        }
        # Throw an error if neither an authentication nor accounting port was provided
        elseif (!isset($this->validated_data["radius_auth_port"])) {
            $this->errors[] = APIResponse\get(5032);
        }
    }

    private function __validate_radius_protocol() {
        # Validate our optional `radius_protocol` payload value
        if (isset($this->initial_data["radius_protocol"])) {
            # Ensure the value is a valid RADIUS protocol option
            if (in_array($this->initial_data["radius_protocol"], ["PAP", "CHAP_MD5", "MSCHAPv1", "MSCHAPv2"])) {
                $this->validated_data["radius_protocol"] = $this->initial_data["radius_protocol"];
            } else {
                $this->errors[] = APIResponse\get(5027);
            }
        }
        # Assume default if none was specified
        else {
            $this->validated_data["radius_protocol"] = "MSCHAPv2";
        }
    }

    private function __validate_radius_timeout() {
        # Validate our required `radius_timeout` payload value
        if (isset($this->initial_data["radius_timeout"])) {
            # Ensure the `radius_timeout` is an integer of 1 or greater
            if (is_integer($this->initial_data["radius_timeout"]) and $this->initial_data["radius_timeout"] >= 1) {
                $this->validated_data["radius_timeout"] = $this->initial_data["radius_timeout"];
            } else {
                $this->errors[] = APIResponse\get(5033);
            }
        }
        # Otherwise assume default value
        else {
            $this->validated_data["radius_timeout"] = 5;
        }
    }

    private function __validate_radius_nasip_attribute() {
        # Validate our required `radius_nasip_attribute` payload value
        if (isset($this->initial_data["radius_nasip_attribute"])) {
            # Try to locate the interface associated with the client's input
            $if = APITools\get_pfsense_if_id($this->initial_data["radius_nasip_attribute"]);

            # Ensure the client's input matched an existing interface
            if ($if) {
                $this->validated_data["radius_nasip_attribute"] = $if;
            } else {
                $this->errors[] = APIResponse\get(5034);
            }
        }
        # Otherwise assume default value
        else {
            $this->validated_data["radius_nasip_attribute"] = "wan";
        }
    }
}
