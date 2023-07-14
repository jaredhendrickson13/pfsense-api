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

class APIServicesSSHdUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified sshd configuration via API";
    }

    public function action() {
        # Write the changes to configuration
        $this->set_config("system/ssh", $this->validated_data);
        $this->write_config();

        # Stop SSHD and log that action
        killbyname("sshd");
        log_error(gettext("secure shell configuration has changed. Stopping sshd."));

        # Restart SSHD if it is still enabled
        if ($this->is_config_enabled("system/ssh")) {
            log_error(gettext("secure shell configuration has changed. Restarting sshd."));
            send_event("service restart sshd");
        }

        return APIResponse\get(0, $this->get_config("system/ssh", []));
    }

    public function validate_payload() {
        # Obtain the current ssh value
        $this->validated_data = $this->get_config("system/ssh", []);

        # Validate fields
        $this->__validate_enable();
        $this->__validate_port();
        $this->__validate_sshdkeyonly();
        $this->__validate_sshdagentforwarding();
    }

    private function __validate_enable() {
        # Check for the optional 'enable' field
        if (isset($this->initial_data['enable'])) {
            if ($this->initial_data['enable'] === true) {
                $this->validated_data["enable"] ="enabled";
            }
            elseif ($this->initial_data['enable'] === false) {
                unset($this->validated_data["enable"]);
            }
            else {
                $this->errors[] = APIResponse\get(2000);
            }
        }
    }

    private function __validate_port() {
        # Check for the optional 'port' field
        if (isset($this->initial_data['port'])) {
            # Ensure port is a valid TCP port
            if (is_port((string)$this->initial_data["port"])) {
                $this->validated_data["port"] = (int)$this->initial_data["port"];
            }
            else {
                $this->errors[] = APIResponse\get(2003);
            }
        }

        # Assume a default if this is not set
        if (!isset($this->validated_data["port"])) {
            $this->validated_data["port"] = 22;
        }
    }

    private function __validate_sshdkeyonly() {
        # Check for the optional 'sshdkeyonly' field
        if (isset($this->initial_data["sshdkeyonly"])) {
            # Ensure it is a valid option
            if (in_array($this->initial_data["sshdkeyonly"], ["disabled", "enabled", "both"])) {
                # If disabled, just remove it from the data
                if ($this->initial_data["sshdkeyonly"] === "disabled") {
                    unset($this->validated_data["sshdkeyonly"]);
                }
                # Otherwise, save the data as valid
                else {
                    $this->validated_data["sshdkeyonly"] = $this->initial_data["sshdkeyonly"];
                }
            }
            else {
                $this->errors[] = APIResponse\get(2001);
            }
        }
    }

    private function __validate_sshdagentforwarding() {
        # Check for the optional 'sshdagentforwarding' field
        if (isset($this->initial_data['sshdagentforwarding'])) {
            # Enable the field if set to true
            if ($this->initial_data["sshdagentforwarding"] === true) {
                $this->validated_data["sshdagentforwarding"] = "enabled";
            }
            # Remove the field if set to false
            elseif ($this->initial_data["sshdagentforwarding"] === false) {
                unset($this->validated_data["sshdagentforwarding"]);
            }
            else {
                $this->errors[] = APIResponse\get(2002);
            }
        }
    }
}
