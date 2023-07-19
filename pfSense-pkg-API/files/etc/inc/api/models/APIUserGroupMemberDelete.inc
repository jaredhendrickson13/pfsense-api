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

class APIUserGroupMemberDelete extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Deleted group membership for user via API";
    }

    public function action() {
        local_user_set_groups($this->validated_data["user_config"], $this->validated_data["new_groups"]);    // Set our user's groups
        local_user_set($this->validated_data["user_config"]);    // Reset our user
        $this->write_config();    // Write our config
        // Update our current user's groups
        $this->validated_data["user_config"] = getUserEntry($this->validated_data["username"]);
        $this->validated_data["user_groups"] = local_user_get_groups($this->validated_data["user_config"], true);
        return APIResponse\get(0, $this->validated_data["group"]);
    }

    public function validate_payload() {
        $this->__validate_username();
        $this->__validate_group();
    }

    function __validate_username() {
        if (isset($this->initial_data['username'])) {
            if (!array_key_exists($this->initial_data["username"], index_users())) {
                $this->errors[] = APIResponse\get(5001);
            } else {
                $this->validated_data["username"] = $this->initial_data['username'];
                $this->validated_data["username"] = trim($this->validated_data["username"]);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
    }

    function __validate_group() {
        if (isset($this->initial_data['group'])) {
            if (is_string($this->initial_data["group"])) {
                $this->initial_data["group"] = array($this->initial_data["group"]);
            }
            $this->validated_data["group"] = $this->initial_data['group'];
        } else {
            $this->errors[] = APIResponse\get(5007);
        }

        if (is_array($this->validated_data["group"])) {
            // Get our current user's groups
            $this->validated_data["user_config"] = getUserEntry($this->validated_data["username"]);
            $this->validated_data["user_groups"] = local_user_get_groups($this->validated_data["user_config"], true);
            // Loop through our del group list and ensure it is valid
            foreach ($this->validated_data["group"] as $dg) {
                if (!array_key_exists($dg, index_groups())) {
                    $this->errors[] = APIResponse\get(5008);
                }
                if (in_array($dg, $this->validated_data["user_groups"])) {
                    $this->validated_data["new_groups"] = \array_diff($this->validated_data["user_groups"], array($dg));
                }
            }
        } else {
            $this->errors[] = APIResponse\get(5009);
        }
    }
}
