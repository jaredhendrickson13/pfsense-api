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

class APIUserPrivilegeDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-usermanager-addprivs"];
        $this->change_note = "Deleted privileges for user via API";
    }

    public function action() {
        $user_config =& getUserEntry($this->validated_data["username"]);
        $user_id = index_users()[$this->validated_data["username"]];    // Save our user's array index ID
        local_user_set($user_config);    // Set user backend parameters
        $this->config["system"]["user"][$user_id] = $user_config;    // Add our new config
        $this->write_config();    // Write to config
        return APIResponse\get(0, $this->validated_data["priv"]);
    }

    public function validate_payload() {
        global $priv_list;
        if (isset($this->initial_data['username'])) {
            $this->validated_data["username"] = $this->initial_data['username'];
            $this->validated_data["username"] = trim($this->validated_data["username"]);
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
        if (isset($this->initial_data['priv'])) {
            $this->validated_data["priv"] = $this->initial_data['priv'];
        } else {
            $this->errors[] = APIResponse\get(5004);
        }
        // Check if our user already exists, if so exit on non-zero
        $user_config =& getUserEntry($this->validated_data["username"]);
        if (!array_key_exists("uid", $user_config)) {
            $this->errors[] = APIResponse\get(5002);
        }
        // Ensure our new priv is array, if it is a string create an array containing the string
        if (is_string($this->validated_data["priv"])) {
            $this->validated_data["priv"] = array($this->validated_data["priv"]);
        }
        if (is_array($this->validated_data["priv"])) {
            // Loop through our new priv list and check that the privs are valid
            foreach ($this->validated_data["priv"] as $dp) {
                if (!array_key_exists($dp, $priv_list)) {
                    $this->errors[] = APIResponse\get(5006);
                }
                if (in_array($dp, $user_config["priv"])) {
                    $user_config["priv"] = \array_diff($user_config["priv"], array($dp));
                }
            }
        } else {
            $this->errors[] = APIResponse\get(5005);
        }
    }
}