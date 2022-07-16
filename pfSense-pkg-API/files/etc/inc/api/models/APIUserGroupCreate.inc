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

class APIUserGroupCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-groupmanager"];
        $this->change_note = "Added group via API";
    }

    public function action() {
        # Add this group to configuration and apply OS changes.
        $this->config["system"]["group"][] = $this->validated_data;
        $this->__apply();
        $this->write_config();
        return APIResponse\get(0, APIUserGroupRead::to_representation($this->validated_data));
    }

    public function validate_payload() {
        # Validate input fields
        $this->__validate_scope();    # This must be validated before 'name' for conditional char length
        $this->__validate_name();
        $this->__validate_description();
        $this->__validate_member();
        $this->__validate_priv();

        # Set static values
        $this->validated_data["gid"] = $this->config["system"]["nextgid"]++;
    }

    private function __validate_name() {
        # Variables
        $max_length = ($this->validated_data["scope"] === "remote") ? 64 : 16;

        # Validate the required 'name' field
        if (isset($this->initial_data["name"])) {
            # Ensure group name length is within range
            if (strlen($this->initial_data["name"]) >= 1 and strlen($this->initial_data["name"]) <= $max_length) {
                # Ensure group name is not already in use
                if (!$this->is_existing_group($this->initial_data["name"])) {
                    # Ensure group name does not use invalid characters
                    if (preg_match("/[^a-zA-Z0-9\.\-_]/", $this->initial_data["name"])) {
                        $this->errors[] = APIResponse\get(5045);
                    } else {
                        $this->validated_data["name"] = $this->initial_data["name"];
                    }
                } else {
                    $this->errors[] = APIResponse\get(5049);
                }
            } else {
                $this->errors[] = APIResponse\get(5044);
            }
        } else {
            $this->errors[] = APIResponse\get(5042);
        }
    }

    private function __validate_scope() {
        # Validate the optional 'scope' field
        if (isset($this->initial_data["scope"])) {
            # Ensure the scope is a valid option
            if (in_array($this->initial_data["scope"], ["local", "remote"])) {
                $this->validated_data["scope"] = $this->initial_data["scope"];
            } else {
                $this->errors[] = APIResponse\get(5046);
            }
        } else {
            # Default this value to 'local'
            $this->validated_data["scope"] = "local";
        }
    }

    private function __validate_description() {
        # Validate the optional 'description' field
        if (isset($this->initial_data["description"])) {
            $this->validated_data["description"] = strval($this->initial_data["description"]);
        } else {
            # Default this value to empty string
            $this->validated_data["description"] = "";
        }
    }

    private function __validate_member() {
        # Default members to empty array
        $this->validated_data["member"] = [];

        # Validate the optional 'member' field
        if (isset($this->initial_data["member"])) {
            # Convert non-array values to an array
            if (!is_array($this->initial_data["member"])) {
                $this->initial_data["member"] = array($this->initial_data["member"]);
            }

            # Loop through each array item and ensure it is an existing user ID
            foreach ($this->initial_data["member"] as $uid) {
                # Ensure value is an existing user's UID
                if ($this->is_existing_uid($uid)) {
                    $this->validated_data["member"][] = $uid;
                } else {
                    $this->errors[] = APIResponse\get(5047);
                    break;
                }
            }
        }
    }

    private function __validate_priv() {
        global $priv_list;
        $this->validated_data["priv"] = [];

        # Validate the optional `priv` field
        if ($this->initial_data["priv"]) {
            # Ensure value is an array
            if (!is_array($this->initial_data["priv"])) {
                $this->initial_data["priv"] = array($this->initial_data["priv"]);
            }

            # Loop through each requested privilege and ensure it exists
            foreach ($this->initial_data["priv"] as $priv) {
                if (array_key_exists($priv, $priv_list)) {
                    $this->validated_data["priv"][] = $priv;
                    $this->validated_data["priv"] = array_unique($this->validated_data["priv"]);
                } else {
                    $this->errors[] = APIResponse\get(5048);
                    break;
                }
            }
        }
    }

    # Applies this user/group configuration to the OS user
    private function __apply() {
        # Sort groups alphabetically
        usort($this->config['system']['group'], function($a, $b) {
            return strcasecmp($a['name'], $b['name']);
        });

        # Add this group to the OS
        local_group_set($this->validated_data);

        # Sync group member's OS users
        if (is_array($this->validated_data['member'])) {
            init_config_arr(array('system', 'user'));

            foreach ($this->config["system"]["user"] as $user) {
                if (in_array($user['uid'], $this->validated_data['member'])) {
                    local_user_set($user);
                }
            }
        }
    }

    public function is_existing_uid($uid) {
        # Loop through each existing user and change if the UID matches
        foreach ($this->config["system"]["user"] as $user) {
            if ($user["uid"] == $uid) {
                return true;
            }
        }
        return false;
    }

    public function is_existing_group($name) {
        # Loop through each existing group and ensure this group name is not already used
        foreach ($this->config["system"]["group"] as $group) {
            if ($name === $group["name"]) {
                return true;
            }
        }

        return false;
    }
}


