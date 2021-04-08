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


class APISystemAPISyncUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Synced API configuration from ".$this->client->ip_address;
        $this->privileges = ["page-all", "page-system-api"];
        $this->set_auth_mode = "local";
        $this->retain_read_mode = false;
        $this->ignore_enabled = true;
        $this->ignore_ifs = true;
    }

    public function action() {
        $pkg_index = APITools\get_api_config()[0];
        $this->config["installedpackages"]["package"][$pkg_index]["conf"] = $this->validated_data;
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Do not sync API HA sync settings or authentication data
        unset($this->initial_data["hasync"]);
        unset($this->initial_data["hasync_hosts"]);
        unset($this->initial_data["hasync_username"]);
        unset($this->initial_data["hasync_password"]);
        unset($this->initial_data["client-id"]);
        unset($this->initial_data["client-token"]);

        # Require data to include at least a server key and auth mode
        # NOTE: This model is not intended for public use, input is created by the master node and is generally trusted
        if (isset($this->initial_data["server_key"]) and isset($this->initial_data["authmode"])) {
            $this->validated_data = $this->initial_data;
        } else {
            $this->errors[] = APIResponse\get(1);
        }
    }
}
