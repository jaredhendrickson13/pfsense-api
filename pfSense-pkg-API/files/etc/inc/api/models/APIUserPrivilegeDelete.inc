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

class APIUserPrivilegeDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-usermanager-addprivs"];
        $this->change_note = "Deleted privileges for user via API";
    }
    public function action() {
        $this->config["system"]["user"][$this->id]["priv"] = $this->validated_data["priv"];
        $this->write_config();
        local_user_set($this->validated_data);
        return APIResponse\get(0, $this->validated_data["priv"]);
    }

    public function validate_payload() {
        $this->__validate_username();
        $this->__validate_priv();
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
        } else {
            $this->errors[] = APIResponse\get(5000);
        }
    }

    private function __validate_priv() {
        # Check for our optional `priv` payload value
        if ($this->initial_data["priv"]) {
            # Ensure value is an array
            if (!is_array($this->initial_data["priv"])) {
                $this->initial_data["priv"] = array($this->initial_data["priv"]);
            }

            # Loop through each of the user's stored privileges and remove it if matched
            foreach ($this->validated_data["priv"] as $id=>$priv) {
                # Check if this privilege is one that is being requested to remove
                if (in_array($priv, $this->initial_data["priv"])) {
                    unset($this->validated_data["priv"][$id]);
                }
            }
        }
    }
}
