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


class APIInterfaceVLANCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-vlan-edit"];
        $this->change_note = "Added VLAN interface via API";
    }

    public function action() {
        $this->id = $this->get_next_id("vlans/vlan");
        $this->set_config("vlans/vlan/{$this->id}", $this->validated_data);
        interface_vlan_configure($this->validated_data);
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_if() {
        # Require clients to specify an if value to indicate which interface this VLAN will apply to
        if (isset($this->initial_data['if'])) {
            # Return an error if the requested parent interface does not exist
            if (!does_interface_exist($this->initial_data['if'])) {
                $this->errors[] = APIResponse\get(3051);
            } else {
                $this->validated_data["if"] = $this->initial_data['if'];
            }
        } else {
            $this->errors[] = APIResponse\get(3055);
        }
    }

    private function __validate_tag() {
        # Require clients to specify a VLAN tag value
        if (isset($this->initial_data['tag'])) {
            # Return an error if VLAN tag is not numeric, or is not between 1 and 4096
            if (!is_numeric($this->initial_data['tag']) or (1 > intval($this->initial_data['tag']) or intval($this->initial_data['tag']) > 4096)) {
                $this->errors[] = APIResponse\get(3052);
            }
            # Return an error if this VLAN already exists on this interface
            elseif ($this->__is_vlan($this->validated_data["if"], $this->initial_data["tag"])) {
                $this->errors[] = APIResponse\get(3054);
            }
            # Otherwise, store our tag value as validated
            else {
                $this->validated_data["tag"] = $this->initial_data['tag'];
            }
        } else {
            $this->errors[] = APIResponse\get(3048);
        }
    }

    private function __validate_pcp() {
        # Optionally allow client to specify a VLAN PCP value. Default if not set.
        if (isset($this->initial_data['pcp'])) {
            # Return error if PCP value is not between 0 and 7
            if (0 > $this->initial_data['pcp'] or $this->initial_data['pcp'] > 7) {
                $this->errors[] = APIResponse\get(3053);
            } else {
                $this->validated_data["pcp"] = intval(trim($this->initial_data['pcp']));
            }
        } else {
            $this->validated_data["pcp"] = "";
        }
    }

    private function __validate__descr() {
        # Optionally allow client to specify a VLAN description. Default if not set.
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        } else {
            $this->validated_data["descr"] = "";
        }
    }

    public function validate_payload() {
        $this->__validate_if();
        $this->__validate_tag();
        $this->__validate_pcp();
        $this->__validate__descr();
        $this->validated_data["vlanif"] = $this->validated_data["if"].".".$this->initial_data['tag'];
    }

    private function __is_vlan($if, $tag) {
        # Check if our VLAN already exists
        foreach ($this->get_config("vlans/vlan", []) as $vle) {
            if ($if === $vle["if"] and strval($tag) === $vle["tag"]) {
                return true;
            }
        }
        return false;
    }
}
