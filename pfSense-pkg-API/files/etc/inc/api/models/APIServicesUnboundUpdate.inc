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


class APIServicesUnboundUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified DNS Resolver settings via API";
    }

    public function action() {
        # Write the changes to config and optionally apply the changes
        $this->set_config("unbound", $this->validated_data);
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Fetch the current Unbound config
        $this->validated_data = $this->get_config("unbound");

        # Validate fields
        $this->__validate_enable();
        $this->__validate_dnssec();
        $this->__validate_custom_options();
    }

    private function __validate_enable() {
        # Optionally allow clients to update the `enable` field
        if (isset($this->initial_data["enable"])) {
            # Set the enable flag if if true
            if ($this->initial_data["enable"] === true) {
                $this->validated_data["enable"] = "";
            }
            # Unset the enable flag if false
            elseif ($this->initial_data["enable"] === false) {
                unset($this->validated_data["enable"]);
            }
        }
    }

    private function __validate_dnssec() {
        # Optionally allow clients to update the `dnssec` field
        if (isset($this->initial_data["dnssec"])) {
            # Set the dnssec flag if if true
            if ($this->initial_data["dnssec"] === true) {
                $this->validated_data["dnssec"] = "";
            }
            # Unset the dnssec flag if false
            elseif ($this->initial_data["dnssec"] === false) {
                unset($this->validated_data["dnssec"]);
            }
        }
    }

    private function __validate_custom_options() {
        # Optionally allow clients to update the `custom_options` field
        if (isset($this->initial_data["custom_options"])) {
            # Base64 encode the custom options
            $this->validated_data["custom_options"] = base64_encode($this->initial_data["custom_options"]);
        }
    }

    public function apply() {
        # Apply the change immediately if requested
        if ($this->initial_data["apply"] === true) {
            APITools\unbound_reload_config();
        }
    }
}
