<?php
//   Copyright 2023 Jared Hendrickson
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


class APIInterfaceBridgeDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Deleted bridge interface via API";
    }

    public function action() {
        $this->del_config("bridges/bridged/{$this->id}");
        $this->write_config();
        pfSense_interface_destroy($this->validated_data["bridgeif"]);
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Check our required 'id' field
        if (isset($this->initial_data["id"])) {
            # Loop through each bridge and check if the ID matches the bridgeif value
            foreach ($this->get_config("bridges/bridged", []) as $id=>$bridge) {
                # Check if this bridge matches
                if (strtolower($this->initial_data["id"]) === $bridge["bridgeif"]) {
                    $this->validated_data = $bridge;
                    # Check that the bridge isn't in use
                    if (!$this->__is_bridge_in_use()) {
                        $this->id = $id;
                    } else {
                        $this->errors[] = APIResponse\get(3073);
                    }
                }
            }
            # If no ID was identified, throw a bridge not found error
            if (!is_numeric($this->id)) {
                $this->errors[] = APIResponse\get(3072);

            }
        } else {
            $this->errors[] = APIResponse\get(3071);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
    }

    private function __is_bridge_in_use() {
        # Loop through each configured interface and see if this bridge is assigned
        foreach ($this->get_config("interfaces", []) as $id=>$interface) {
            # Check if this interface matches the bridgeif to be deleted
            if ($interface["if"] === $this->validated_data["bridgeif"]) {
                return true;
            }
        }

        # When no interface matched the bridge during the loop
        return false;
    }
}
