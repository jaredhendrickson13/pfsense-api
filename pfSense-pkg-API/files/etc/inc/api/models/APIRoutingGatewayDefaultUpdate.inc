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


class APIRoutingGatewayDefaultUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-gateways"];
        $this->change_note = "Set default gateway via API";
    }

    public function action() {
        # Set the default gateways based on the validated input
        $this->set_config("gateways/defaultgw4", $this->validated_data["defaultgw4"]);
        $this->set_config("gateways/defaultgw6", $this->validated_data["defaultgw6"]);

        # Write these changes to config and apply if the client requested
        $this->write_config();

        # Mark the routing subsystem as changed
        mark_subsystem_dirty("staticroutes");
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_defaultgw4() {
        # Optionally allow clients to update the 'defaultgw4' value
        if (isset($this->initial_data["defaultgw4"])) {
            # Ensure this is a valid IPv4 gateway
            if (APITools\is_gateway($this->initial_data["defaultgw4"], true) === "inet") {
                $this->validated_data["defaultgw4"] = $this->initial_data["defaultgw4"];
            }
            # Allow client to set automatic gateway selection
            elseif (in_array($this->initial_data["defaultgw4"], ["", "automatic"])) {
                $this->validated_data["defaultgw4"] = "";
            }
            # Allow client to set no gateway
            elseif (in_array($this->initial_data["defaultgw4"], ["-", "none"])) {
                $this->validated_data["defaultgw4"] = "-";
            }
            else {
                $this->errors[] = APIResponse\get(6028);
            }
        }
    }

    private function __validate_defaultgw6() {
        # Optionally allow clients to update the 'defaultgw6' value
        if (isset($this->initial_data["defaultgw6"])) {
            # Ensure this is a valid IPv6 gateway
            if (APITools\is_gateway($this->initial_data["defaultgw6"], true) === "inet6") {
                $this->validated_data["defaultgw6"] = $this->initial_data["defaultgw6"];
            }
            # Allow client to set automatic gateway selection
            elseif (in_array($this->initial_data["defaultgw6"], ["", "automatic"])) {
                $this->validated_data["defaultgw6"] = "";
            }
            # Allow client to set no gateway
            elseif (in_array($this->initial_data["defaultgw6"], ["-", "none"])) {
                $this->validated_data["defaultgw6"] = "-";
            }
            else {
                $this->errors[] = APIResponse\get(6028);
            }
        }
    }

    public function validate_payload() {
        # Fetch existing default gateway values
        $this->validated_data = [
            "defaultgw4"=>$this->get_config("gateways/defaultgw4"),
            "defaultgw6"=>$this->get_config("gateways/defaultgw6")
        ];

        # Validate client input
        $this->__validate_defaultgw4();
        $this->__validate_defaultgw6();
    }

    public function apply() {
        # Apply these changes immediately if requested
        if ($this->initial_data["apply"] === true) {
            APIRoutingApplyCreate::apply();
        }
    }
}
