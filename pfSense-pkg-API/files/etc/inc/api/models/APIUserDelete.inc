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
        # Remove user from backend and remove from config
        local_user_del($this->config["system"]["user"][$this->id]);
        unset($this->config["system"]["user"][$this->id]);
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_username() {
        # Check for our required `username` payload value
        if (isset($this->initial_data['username'])) {
            # Loop through each configured user and check if this user exists
            foreach ($this->config["system"]["user"] as $id=>$user) {
                if ($this->initial_data["username"] === $user["name"]) {
                    $this->validated_data = $user;
                    $this->id = intval($id);
                }
            }
            # Set an error if no user was found
            if (!isset($this->validated_data["uid"])) {
                $this->errors[] = APIResponse\get(5001);
            }
            # Set an error if this is a system user
            if ($this->validated_data["scope"] !== "user") {
                $this->errors[] = APIResponse\get(5005);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
    }

    public function validate_payload() {
        $this->__validate_username();
    }
}