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

class APIUserAuthServerLDAPDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-user-authserver"];
        $this->change_note = "Deleted LDAP authentication server via API";
    }

    public function action() {
        $del_server = $this->config["system"]["authserver"][$this->validated_data["id"]];
        unset($this->config["system"]["authserver"][$this->validated_data["id"]]);
        if ($this->validated_data["revert"] === true) {
            unset($this->config["system"]["webgui"]["authmode"]);
        }
        $this->write_config();
        return APIResponse\get(0, $del_server);
    }

    public function validate_payload() {
        if (isset($this->initial_data['name'])) {
            $this->validated_data["name"] = $this->initial_data['name'];
        } else {
            $this->errors[] = APIResponse\get(5010);
        }

        // Loop through our servers and check that this server exists
        foreach ($this->config["system"]["authserver"] as $asid => $asc) {
            if ($this->validated_data["name"] === $asc["name"] and $asc["type"] === "ldap") {
                $this->validated_data["id"] = $asid;    // Save our config before deleting
                // Check if this auth server is our default, if so revert to local database
                if ($this->validated_data["name"] === $this->config["system"]["webgui"]["authmode"]) {
                    $this->validated_data["revert"] = True;
                }
            }
        }
        if (is_null($this->validated_data["id"])) {
            $this->errors[] = APIResponse\get(5025);
        }
    }
}