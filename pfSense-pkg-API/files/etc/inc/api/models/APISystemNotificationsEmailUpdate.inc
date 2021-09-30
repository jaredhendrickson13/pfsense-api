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

class APISystemNotificationsEmailUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-notifications"];
        $this->change_note = "Modified system email notifications via API";
    }

    public function action() {
        # Write our new email notification settings
        $this->__init_config();
        $this->config["notifications"]["smtp"] = $this->validated_data;
        $this->write_config();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
       # Fetch our existing SMTP configuration
        $this->__get_smtp_config();
        $this->__validate_disabled();
        $this->__validate_ipaddress();
        $this->__validate_port();
        $this->__validate_timeout();
        $this->__validate_ssl();
        $this->__validate_sslvalidate();
        $this->__validate_fromaddress();
        $this->__validate_notifyemailaddress();
        $this->__validate_authentication_mechanism();

        # Validate LOGIN specific fields
        if ($this->validated_data["authentication_mechanism"] === "LOGIN") {
            $this->__validate_username();
            $this->__validate_password();
        }
    }

    private function __validate_disabled() {
        # Validate our optional 'disabled' field
        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = "";
        }
        elseif ($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    private function __validate_ipaddress() {
        # Validate our required 'ipaddress' field
        if (isset($this->initial_data["ipaddress"])) {
            # Ensure value is valid IP address or hostname
            if (is_ipaddr($this->initial_data["ipaddress"]) or is_hostname($this->initial_data["ipaddress"])) {
                $this->validated_data["ipaddress"] = $this->initial_data["ipaddress"];
            } else {
                $this->errors[] = APIResponse\get(1062);
            }
        } else {
            $this->errors[] = APIResponse\get(1061);
        }
    }

    private function __validate_port() {
        # Validate our required 'port' field
        if (isset($this->initial_data["port"])) {
            # Ensure value is valid port
            if (is_port(strval($this->initial_data["port"]))) {
                $this->validated_data["port"] = $this->initial_data["port"];
            } else {
                $this->errors[] = APIResponse\get(1064);
            }
        } else {
            $this->errors[] = APIResponse\get(1063);
        }
    }

    private function __validate_timeout() {
        # Validate our optional 'timeout' field
        if (isset($this->initial_data["timeout"])) {
            # Require value to be numeric and greater than 0
            if (is_numeric($this->initial_data["timeout"]) and intval($this->initial_data["timeout"]) > 0) {
                $this->validated_data["timeout"] = intval($this->initial_data["timeout"]);
            } else {
                $this->errors[] = APIResponse\get(1065);
            }
        } else {
            # Default to 20
            $this->validated_data["timeout"] = 20;
        }
    }

    private function __validate_ssl() {
        # Validate our optional 'ssl' field
        if ($this->initial_data["ssl"] === true) {
            $this->validated_data["ssl"] = "";
        }
        elseif ($this->initial_data["ssl"] === false) {
            unset($this->validated_data["ssl"]);
        }
    }

    private function __validate_sslvalidate() {
        # Validate our optional 'sslvalidate' field
        if ($this->initial_data["sslvalidate"] === true) {
            $this->validated_data["sslvalidate"] = "enabled";
        }
        elseif ($this->initial_data["sslvalidate"] === false) {
            $this->validated_data["sslvalidate"] = "disabled";
        }
    }

    private function __validate_fromaddress() {
        # Validate our required 'fromaddress' field
        if (isset($this->initial_data["fromaddress"])) {
            # Ensure value is valid email address
            if (filter_var($this->initial_data["fromaddress"], FILTER_VALIDATE_EMAIL)) {
                $this->validated_data["fromaddress"] = $this->initial_data["fromaddress"];
            } else {
                $this->errors[] = APIResponse\get(1067);
            }
        } else {
            $this->errors[] = APIResponse\get(1066);
        }
    }

    private function __validate_notifyemailaddress() {
        # Validate our required 'notifyemailaddress' field
        if (isset($this->initial_data["notifyemailaddress"])) {
            # Ensure value is valid email address
            if (filter_var($this->initial_data["notifyemailaddress"], FILTER_VALIDATE_EMAIL)) {
                $this->validated_data["notifyemailaddress"] = $this->initial_data["notifyemailaddress"];
            } else {
                $this->errors[] = APIResponse\get(1069);
            }
        } else {
            $this->errors[] = APIResponse\get(1068);
        }
    }

    private function __validate_authentication_mechanism() {
        # Local variables
        $auth_mechanisms = ["PLAIN", "LOGIN"];

        # Validate our required 'authentication_mechanism' field
        if (isset($this->initial_data["authentication_mechanism"])) {
            # Ensure value is valid choice
            if (in_array(strtoupper($this->initial_data["authentication_mechanism"]), $auth_mechanisms)) {
                $this->validated_data["authentication_mechanism"] = strtoupper($this->initial_data["authentication_mechanism"]);

                # Remove login specific fields if set to PLAIN
                if ($this->validated_data["authentication_mechanism"] === "PLAIN") {
                    unset($this->validated_data["username"]);
                    unset($this->validated_data["password"]);
                }
            } else {
                $this->errors[] = APIResponse\get(1071);
            }
        } else {
            $this->errors[] = APIResponse\get(1070);
        }
    }

    private function __validate_username() {
        # Check for the optional 'username' field
        if (isset($this->initial_data["username"])) {
            $this->validated_data["username"] = strval($this->initial_data["username"]);
        }
    }

    private function __validate_password() {
        # Check for the optional 'password' field
        if (isset($this->initial_data["password"])) {
            $this->validated_data["password"] = strval($this->initial_data["password"]);
        }
    }

    private function __init_config() {
        # Check if we have a notifications config
        if (empty($this->config["notifications"])) {
            $this->config["notifications"] = [];
        }
    }

    private function __get_smtp_config() {
        # Check if the configuration exists
        if (!empty($this->config["notifications"]["smtp"])) {
            $this->validated_data = $this->config["notifications"]["smtp"];
        } else {
            $this->validated_data = [];
        }
    }
}
