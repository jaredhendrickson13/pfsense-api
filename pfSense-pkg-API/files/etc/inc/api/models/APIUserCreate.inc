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

class APIUserCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-usermanager"];
        $this->change_note = "Added user via API";
    }

    public function action() {
        $this->config['system']['user'][] = $this->validated_data;
        $this->config["system"]["nextuid"] = strval(intval($this->validated_data["uid"]) + 1);   // Increase our next UID
        local_user_set_password($this->validated_data, $this->validated_data["password"]);  // Set our new user's password
        local_user_set($this->validated_data);
        $this->write_config();
        $userindex = index_users();    // Update our user index
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->validated_data["uid"] = $this->config["system"]["nextuid"];    // Save our next UID
        if (isset($this->initial_data['username'])) {
            // Check that our user already exists
            if (array_key_exists($this->initial_data['username'], index_users())) {
                $this->errors[] = APIResponse\get(5002);
            } else {
                $this->validated_data["name"] = trim($this->initial_data['username']);
            }
        } else {
            $this->errors[] = APIResponse\get(5000);
        }

        if (isset($this->initial_data['password'])) {
            $this->validated_data["password"] = trim($this->initial_data['password']);
        } else {
            $this->errors[] = APIResponse\get(5003);
        }

        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = "";    // Update our user's disabled value if not false
        } elseif ($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);    // Unset our disabled value if not requested
        }

        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = trim($this->initial_data['descr']);    // Update our user's full name
        }

        if (isset($this->initial_data['expires'])) {
            $this->validated_data["expires"] = trim($this->initial_data['expires']);    // Update our user's expiration date
        }

        if (isset($this->initial_data['authorizedkeys'])) {
            $this->validated_data["authorizedkeys"] = trim($this->initial_data['authorizedkeys']);    // Update our user's authorized keys
        }

        if (isset($this->initial_data['ipsecpsk'])) {
            $this->validated_data["ipsecpsk"] = trim($this->initial_data['ipsecpsk']);    // Update our user's IPsec pre-shared key
        }

        $this->validated_data["scope"] = "user";    // Set our new user's system scope
        $this->validated_data["priv"] = [];    // Default our privs to empty array
    }
}