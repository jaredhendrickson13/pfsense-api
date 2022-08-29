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

class APIServicesIPsecPhase2Delete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-ipsec-editphase2"];
        $this->change_note = "Deleted IPsec phase 2 via API";
    }

    public function action() {
        # Capture the entry being delete before deleting it
        $this->validated_data = $this->config["ipsec"]["phase2"][$this->id];

        # Remove this phase2 entry and mark the changes as pending
        unset($this->config["ipsec"]["phase2"][$this->id]);
        $this->write_config();
        mark_subsystem_dirty('ipsec');

        # Apply these changes if requested
        if ($this->initial_data["apply"] === true) {
            APIServicesIPsecApplyCreate::apply();
        }

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_uniqid();
    }

    private function __validate_uniqid() {
        # Validate the required 'uniqid' field
        if (isset($this->initial_data["uniqid"])) {
            # Attempt to get the config ID from this phase 2 unique ID
            $this->id = APITools\get_phase2_config_id_from_uniqid($this->initial_data["uniqid"]);

            # Return an error if this phase 2 unique was not found in any existing IPsec phase1 entry
            if ($this->id === false) {
                $this->errors[] = APIResponse\get(2258);
            }
        } else {
            $this->errors[] = APIResponse\get(2257);
        }
    }
}
