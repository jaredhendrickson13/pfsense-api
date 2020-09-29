<?php
//   Copyright 2020 Jared Hendrickson
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

class APIUserPrivilegeCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-usermanager-addprivs"];
        $this->change_note = "Added privileges for user via API";
    }

    public function action() {
        local_user_set($this->validated_data["user_config"]);    // Set user backend parameters
        $this->write_config();
        return APIResponse\get(0, $this->validated_data["user_config"]["priv"]);
    }

    public function validate_payload() {
        global $priv_list;
        if (isset($this->initial_data['username'])) {
            $this->validated_data["username"] = trim($this->initial_data['username']);
            $this->validated_data["user_config"] =& getUserEntry($this->validated_data["username"]);
            if (!array_key_exists("uid", $this->validated_data["user_config"])) {
                $this->errors[] = APIResponse\get(5001);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
        if (isset($this->initial_data['priv'])) {
            // Ensure our new priv is array, if it is a string create an array containing the string
            if (is_string($this->initial_data["priv"])) {
                $this->initial_data["priv"] = array($this->initial_data["priv"]);
            }
            if (is_array($this->initial_data["priv"])) {
                // Loop through our new priv list and check that the privs are valid
                foreach ($this->initial_data["priv"] as $np) {
                    if (!array_key_exists($np, $priv_list)) {
                        $this->errors[] = APIResponse\get(5006);
                    }
                    if (!in_array($np, $this->validated_data["user_config"]["priv"])) {
                        $this->validated_data["user_config"]["priv"][] = $np;
                    }
                }
            } else {
                $this->errors[] = APIResponse\get(5005);
            }
        } else {
            $this->errors[] = APIResponse\get(5004);
        }
    }
}