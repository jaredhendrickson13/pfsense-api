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

class APIServicesIPsecPhase1Delete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-ipsec-editphase1"];
        $this->change_note = "Deleted IPsec phase 1 via API";
    }

    public function action() {
        # Capture the entry being delete before deleting it
        $this->validated_data = $this->config["ipsec"]["phase1"][$this->id];

        # Remote the entry and mark the changes as pending
        unset($this->config["ipsec"]["phase1"][$this->id]);
        $this->write_config();
        mark_subsystem_dirty('ipsec');

        # Apply these changes if requested
        if ($this->initial_data["apply"] === true) {
            APIServicesIPsecApplyCreate::apply();
        }

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_ikeid();
    }

    private function __validate_ikeid()
    {
        # Validate the required 'ikeid' field
        if (isset($this->initial_data["ikeid"])) {
            # Attempt to get the config ID from this IKE ID
            $this->id = $this->get_config_id_from_ikeid($this->initial_data["ikeid"]);

            # Return an error if this IKE ID was not found in any existing IPsec phase1 entry
            if ($this->id === false) {
                $this->errors[] = APIResponse\get(2208);
            }
        } else {
            $this->errors[] = APIResponse\get(2207);
        }
    }


    public function get_config_id_from_ikeid($ikeid) {
        # Loop through each IPsec phase1 entry and check for a match
        foreach ($this->config["ipsec"]["phase1"] as $index=>$p1) {
            # Check if this IKE ID matches
            if ((int)$ikeid === (int)$p1["ikeid"]) {
                return $index;
            }
        }

        return false;
    }
}
