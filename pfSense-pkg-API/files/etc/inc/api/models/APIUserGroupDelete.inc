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

class APIUserGroupDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Deleted group via API";
    }

    public function action() {
        # Add this group to configuration and apply OS changes.
        $this->del_config("system/group/{$this->id}");
        $this->__apply();
        $this->write_config();
        return APIResponse\get(0, APIUserGroupRead::to_representation($this->validated_data));
    }

    public function validate_payload() {
        # Validate input fields
        $this->__validate_id();
    }

    private function __validate_id() {
        # Validate the required 'id' field
        if (isset($this->initial_data["id"])) {
            # Get group ID by name
            $id = $this->is_existing_group($this->initial_data["id"], true);
            # Check if a group with this name (ID) exists
            if ($id !== false) {
                $this->id = $id;
                $this->validated_data = $this->get_config("system/group/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(5051);
            }
        } else {
            $this->errors[] = APIResponse\get(5050);
        }
    }

    # Applies this user/group configuration to the OS user
    private function __apply() {
        # Resort groups to correct IDs
        $this->set_config("system/group", array_values($this->get_config("system/group")));

        # Sort groups alphabetically
        $groups = $this->get_config("system/group");
        usort($groups, function($a, $b) {
            return strcasecmp($a['name'], $b['name']);
        });

        local_group_del($this->validated_data);
    }

    public function is_existing_group($name, $ret_id=false) {
        # Loop through each existing group and ensure this group name is not already used
        foreach ($this->get_config("system/group", []) as $id=>$group) {
            # Check if names match. Allow names to match if the name matches this object's current name.
            if ($name === $group["name"] and $name !== $this->validated_data["name"]) {
                return ($ret_id) ? $id : true;
            }
        }

        return false;
    }
}


