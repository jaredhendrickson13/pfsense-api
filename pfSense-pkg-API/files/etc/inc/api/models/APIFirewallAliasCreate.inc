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

class APIFirewallAliasCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Added firewall alias via API";
    }

    public function action() {
        # Add our new alias and reload configuration as requested
        $this->id = $this->get_next_id("aliases/alias");
        $this->set_config("aliases/alias/{$this->id}", $this->validated_data);
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
        # Require clients to specify a valid name for the alias
        if (isset($this->initial_data['name'])) {
            # Ensure an alias with this name does not already exist
            if (!is_alias($this->initial_data['name'])) {
                # Ensure the requested name is valid and not reserved by the system
                if (is_validaliasname($this->initial_data["name"])) {
                    $this->validated_data["name"] = $this->initial_data['name'];
                } else {
                    $this->errors[] = APIResponse\get(4053);
                }
            } else {
                $this->errors[] = APIResponse\get(4056);
            }
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
    }

    private function __validate_type() {
        # Require clients to specify a valid type for this alias
        if (isset($this->initial_data['type'])) {
            # Require this type to be supported
            if (in_array($this->initial_data['type'], ["host", "network", "port"])) {
                $this->validated_data["type"] = $this->initial_data["type"];
            } else {
                $this->errors[] = APIResponse\get(4057);
            }
        } else {
            $this->errors[] = APIResponse\get(4061);
        }
    }

    private function __validate_address() {
        if (isset($this->initial_data['address'])) {
            # Use APIFirewallAliasEntryCreate model to validate the address field
            $alias_ent = new APIFirewallAliasEntryCreate();
            $alias_ent->validated_data = $this->validated_data;
            $alias_ent->__validate_address();

            # Check if errors were found when validating the address
            if ($alias_ent->errors) {
                $this->errors = $this->errors + $alias_ent->errors;
            } else {
                $this->validated_data["address"] = $alias_ent->validated_data["address"];
            }
        } else {
            $this->errors[] = APIResponse\get(4052);
        }
    }

    private function __validate_descr() {
        # Optionally allow clients to specify an alias description
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        } else {
            $this->validated_data["descr"] = "";
        }
    }

    private function __validate_detail() {
        # Use APIFirewallAliasEntryCreate model to validate the address field
        $alias_ent = new APIFirewallAliasEntryCreate();
        $alias_ent->validated_data = $this->validated_data;
        $alias_ent->__validate_detail();

        # Check if errors were found when validating the address
        if ($alias_ent->errors) {
            $this->errors = $this->errors + $alias_ent->errors;
        } else {
            $this->validated_data["detail"] = $alias_ent->validated_data["detail"];
        }
    }
    
    public function validate_payload() {
        $this->__validate_name();
        $this->__validate_type();
        $this->__validate_descr();
        $this->__validate_address();
        $this->__validate_detail();
    }
}
