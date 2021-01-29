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

class APIUserDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-usermanager"];
        $this->change_note = "Deleted user via API";
    }

    public function action() {
        $index_id = index_users()[$this->validated_data["username"]];    // Save our user's index ID number
        $del_user = $this->config["system"]["user"][$index_id];
        local_user_del($this->config["system"]["user"][$index_id]);    // Delete our user on the backend
        unset($this->config['system']['user'][$index_id]);    // Unset our user from config
        $this->config['system']['user'] = array_values($this->config['system']['user']);    // Reindex our users
        $this->write_config();    // Write our new config
        return APIResponse\get(0, $del_user);
    }

    public function validate_payload() {
        if (isset($this->initial_data["username"])) {
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
}