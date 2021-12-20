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
require_once("api/models/APIFirewallStatesSizeRead.inc");


class APIFirewallStatesSizeUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-firewall"];
        $this->change_note = "Modified firewall state table size via API";
    }

    public function action() {

        // Write our state table size
        if ($this->validated_data["maximumstates"] === "default") {
            unset($this->config["system"]["maximumstates"]);
        } else {
            $this->config["system"]["maximumstates"] = $this->validated_data["maximumstates"];
        }
        $this->write_config();    // Apply our configuration change
        filter_configure();    // Update our firewall filter
        return APIResponse\get(0, (new APIFirewallStatesSizeRead())->action()["data"]);
    }

    public function validate_payload() {
        // Check if we set a maximumstates size
        if (isset($this->initial_data["maximumstates"])) {
            // Check that our state size is valid
            if (is_numeric($this->initial_data["maximumstates"]) and (10 <= intval($this->initial_data["maximumstates"]))) {
                $this->validated_data["maximumstates"] = $this->initial_data["maximumstates"];
            } elseif ($this->initial_data["maximumstates"] === "default") {
                $this->validated_data["maximumstates"] = $this->initial_data["maximumstates"];
            } else {
                $this->errors[] = APIResponse\get(4073);
            }
        } else {
            $this->errors[] = APIResponse\get(4072);
        }
    }
}
