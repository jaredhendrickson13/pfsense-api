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

class APIFirewallAliasEntryDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-alias-edit"];
        $this->change_note = "Deleted firewall alias address via API";
    }

    public function action() {
        # Write our new alias entry
        $this->config["aliases"]["alias"][$this->id] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty("aliases");

        # Only reload the firewall filter if a false value was not passed in
        # TODO: This condition applies the changes by default to stay backwards compatible with v1.3.0
        # TODO: this should be refactored in a future release to not apply by default
        if ($this->initial_data["apply"] !== false) {
            APIFirewallApplyCreate::apply();
        }
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_name() {
        # Require clients to pass in a name value to locate the alias to add addresses to
        if (isset($this->initial_data['name'])) {
            # Ensure this alias exists
            $this->id = $this->__get_alias_id($this->initial_data["name"]);
            if (isset($this->id)) {
                $this->validated_data = $this->config["aliases"]["alias"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(4055);
            }
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
    }

    private function __validate_address() {
        # Require client to pass in an array of address values to delete
        if (isset($this->initial_data["address"])) {
            # Ensure our input and existing data is in array format
            $this->initial_data["address"] = $this->__value_to_array($this->initial_data["address"]);
            $this->validated_data["address"] = array_filter(explode(" ", $this->validated_data["address"]));
            $this->validated_data["detail"] = array_filter(explode("||", $this->validated_data["detail"]));

            # Loop through each request address value and remove it if it exists
            foreach ($this->validated_data["address"] as $id => $address) {
                if (in_array($address, $this->initial_data["address"])) {
                    unset($this->validated_data["address"][$id]);
                    unset($this->validated_data["detail"][$id]);
                }
            }
            $this->validated_data["address"] = implode(" ", $this->validated_data["address"]);
            $this->validated_data["detail"] = implode("||", $this->validated_data["detail"]);
        }
    }

    public function validate_payload() {
        $this->__validate_name();
        $this->__validate_address();
    }

    private function __get_alias_id($name) {
        # Loop through each alias until we find one that matches
        foreach ($this->config["aliases"]["alias"] as $id => $alias) {
            # Check if the name matches
            if ($name === $alias["name"]) {
                return intval($id);
            }
        }
    }

    # Adds a non-array variable to an array
    private function __value_to_array($value) {
        # Only convert the value if it is not already array
        if (!is_array($value)) {
            return ([$value]);
        } else {
            return $value;
        }
    }
}
