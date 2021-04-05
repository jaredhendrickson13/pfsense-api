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

class APIFirewallAliasUpdate extends APIModel {
    private $type_changed;
    private $original_name;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-alias-edit"];
        $this->change_note = "Modified firewall alias address via API";
        $this->type_changed = false;
    }

    public function action() {
        $this->config["aliases"]["alias"][$this->id] = $this->validated_data;
        $this->__rename_alias_references__();
        $this->write_config();
        send_event("filter reload");
        return APIResponse\get(0, $this->config["aliases"]["alias"][$this->id]);
    }

    private function __validate_id() {
        # Require clients to pass in an ID field containing the alias name or index value
        if (isset($this->initial_data["id"])) {
            # Loop through each alias and check for a match
            foreach ($this->config["aliases"]["alias"] as $id=>$alias) {
                # First check if the ID matches the index value or the alias name
                if ($this->initial_data["id"] === $id or $this->initial_data["id"] === $alias["name"]) {
                    $this->id = $id;
                    $this->validated_data = $alias;
                    $this->original_name = $alias["name"];
                    break;
                }
            }
            # If we did not find an ID in the loop, return a not found error
            if (is_null($this->id)) {
                $this->errors[] = APIResponse\get(4055);
            }
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
    }

    private function __validate_name() {
        # Optionally allow clients to update the alias name
        if (isset($this->initial_data['name'])) {
            # Ensure an alias with this name does not already exist. Allow if the stored name matches the requested.
            if (!is_alias($this->initial_data['name']) or $this->validated_data["name"] === $this->initial_data['name']) {
                # Ensure the requested name is valid and not reserved by the system
                if (is_validaliasname($this->initial_data["name"])) {
                    $this->validated_data["name"] = $this->initial_data['name'];
                } else {
                    $this->errors[] = APIResponse\get(4053);
                }
            } else {
                $this->errors[] = APIResponse\get(4056);
            }
        }
    }

    private function __validate_type() {
        # Optionally allow clients to update the alias type
        if (isset($this->initial_data['type'])) {
            # Require alias to not be in use to change the type
            if (!APITools\alias_in_use($this->original_name)) {
                # Require this type to be supported
                if (in_array($this->initial_data['type'], ["host", "network", "port"])) {
                    $this->type_changed = true;
                    $this->validated_data["type"] = $this->initial_data["type"];
                } else {
                    $this->errors[] = APIResponse\get(4057);
                }
            } elseif ($this->initial_data["type"] !== $this->validated_data["type"]) {
                $this->errors[] = APIResponse\get(4107);
            }
        }
    }

    private function __validate_address() {
        if (isset($this->initial_data['address']) or $this->type_changed) {
            # Revert the address array and detail array if set
            if (isset($this->initial_data["address"])) {
                unset($this->validated_data["address"]);
            }
            # Otherwise, revalidate our validated data
            else {
                $this->initial_data["address"] = $this->validated_data["address"];
                unset($this->validated_data["address"]);
            }

            # Use APIFirewallAliasEntryCreate model to validate the address field
            $alias_ent = new APIFirewallAliasEntryCreate();
            $alias_ent->initial_data = $this->initial_data;
            $alias_ent->validated_data = $this->validated_data;
            $alias_ent->__validate_address();

            # Check if errors were found when validating the address
            if ($alias_ent->errors) {
                $this->errors = $this->errors + $alias_ent->errors;
            } else {
                $this->validated_data["address"] = $alias_ent->validated_data["address"];
            }
        }
    }

    private function __validate_descr() {
        # Optionally allow clients to specify an alias description
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
    }

    private function __validate_detail() {
        # Unset the existing details if new details are specified
        if (isset($this->initial_data["detail"])) {
            unset($this->validated_data["detail"]);
        }

        # Use APIFirewallAliasEntryCreate model to validate the address field
        $alias_ent = new APIFirewallAliasEntryCreate();
        $alias_ent->initial_data = $this->initial_data;
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
        $this->__validate_id();
        $this->__validate_name();
        $this->__validate_type();
        $this->__validate_descr();
        $this->__validate_address();
        $this->__validate_detail();
    }

    private function __rename_alias_references__() {
        # Only update alias references if our name was changed
        if ($this->original_name !== $this->validated_data["name"]) {
            update_alias_name($this->validated_data["name"], $this->original_name);
        }
    }
}
