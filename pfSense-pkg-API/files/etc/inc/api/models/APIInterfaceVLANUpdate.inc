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


class APIInterfaceVLANUpdate extends APIModel {
    public $prev_vlanif;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified VLAN interface via API";
    }

    public function action() {
        # Write these changes to the config and reconfigure the VLAN
        pfSense_interface_destroy($this->prev_vlanif);
        interface_vlan_configure($this->validated_data);
        $this->set_config("vlans/vlan/{$this->id}", $this->validated_data);

        # Check if any interface assignments are using the original VLAN interface
        $reload_if = $this->__get_if_to_reassign();

        # Reconfigure the interface assignment if an interface was using the previous VLAN
        if ($reload_if) {
            $this->set_config("interfaces/{$reload_if}/if", $this->validated_data["vlanif"]);
            interface_configure($reload_if);
        }

        # Write the changes to config and respond to the API call
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Check if the given ID exists. Also allow vlanif for backward compatibility
        if (isset($this->initial_data["id"])) {
            # Try to locate our ID by configuration index ID
            if ($this->get_config("vlans/vlan/{$this->initial_data['id']}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("vlans/vlan/{$this->id}");
            }
            else {
                $this->errors[] = APIResponse\get(3050);
            }
        }
        elseif (isset($this->initial_data["vlanif"])) {
            # Allow user to locate the VLAN by vlanif
            if (!is_null($this->__get_vlan_id_from_vlanif($this->initial_data["vlanif"]))) {
                $this->id = $this->__get_vlan_id_from_vlanif($this->initial_data["vlanif"]);
                $this->validated_data = $this->get_config("vlans/vlan/{$this->id}");
            }
            else {
                $this->errors[] = APIResponse\get(3050);
            }
        }
        else {
            $this->errors[] = APIResponse\get(3056);
        }
    }

    private function __validate_if() {
        # Optionally allow clients to update the parent if value
        if (isset($this->initial_data['if'])) {
            # Only update the value if it has changed
            if ($this->initial_data["if"] !== $this->validated_data["if"]) {
                if (does_interface_exist($this->initial_data['if'])) {
                    $this->validated_data["if"] = $this->initial_data['if'];
                } else {
                    $this->errors[] = APIResponse\get(3051);
                }
            }
        }
    }

    private function __validate_tag() {
        # Always revalidate the tag
        if (!isset($this->initial_data["tag"])) {
            $this->initial_data["tag"] = $this->validated_data["tag"];
        }

        # Optionally allow clients to update the VLAN tag value
        if (isset($this->initial_data['tag'])) {
            # Return an error if VLAN tag is not numeric, or is not between 1 and 4096
            if (!is_numeric($this->initial_data['tag']) or (1 > intval($this->initial_data['tag']) or intval($this->initial_data['tag']) > 4094)) {
                $this->errors[] = APIResponse\get(3052);
            }
            # Return an error if this VLAN already exists on this interface and it does not match the current value
            elseif ($this->__is_vlan($this->validated_data["if"], $this->initial_data["tag"])) {
                $this->errors[] = APIResponse\get(3054);
            }
            # Return an error if this VLAN already exists as a QinQ
            elseif ($this->__is_qinq($this->validated_data["if"], $this->initial_data["tag"])) {
                $this->errors[] = APIResponse\get(3081);
            }
            # Otherwise, store our tag value as validated
            else {
                $this->validated_data["tag"] = $this->initial_data['tag'];
            }
        }
    }

    private function __validate_pcp() {
        # Optionally allow client to update tge VLAN PCP value.
        if (isset($this->initial_data['pcp'])) {
            # Return error if PCP value is not between 0 and 7
            if (0 > $this->initial_data['pcp'] or $this->initial_data['pcp'] > 7) {
                $this->errors[] = APIResponse\get(3053);
            } else {
                $this->validated_data["pcp"] = intval(trim($this->initial_data['pcp']));
            }
        }
    }

    private function __validate__descr() {
        # Optionally allow client to update the VLAN description.
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
    }

    public function validate_payload() {
        # Locate this VLAN by ID and retain the original vlanif value, so we can reconfigure it
        $this->__validate_id();
        $this->prev_vlanif = $this->validated_data["vlanif"];

        # Validate fields
        $this->__validate_if();
        $this->__validate_tag();
        $this->__validate_pcp();
        $this->__validate__descr();
        $this->validated_data["vlanif"] = $this->validated_data["if"].".".$this->validated_data['tag'];
    }

    private function __is_vlan($if, $tag) {
        # Check if our VLAN already exists
        foreach ($this->get_config("vlans/vlan", []) as $id => $vlan) {
            # Check if we found a match
            if ($if === $vlan["if"] and (int)$tag === (int)$vlan["tag"]) {
                # Check if the interface we found is this VLAN
                if ($id !== $this->id)
                    return true;
            }
        }
        return false;
    }

    private function __is_qinq($if, $tag) {
        foreach ($this->get_config('qinqs/qinqentry', []) as $qinq) {
            if ($qinq['tag'] == $tag && $qinq['if'] == $if) {
                return true;
            }
        }

        return false;
    }

    private function __get_vlan_id_from_vlanif($vlanif) {
        # Loop through each VLAN and return it's ID if it matches
        foreach ($this->get_config("vlans/vlan", []) as $id => $vlan) {
            if ($vlanif === $vlan["vlanif"]) {
                return $id;
            }
        }
    }

    private function __get_if_to_reassign() {
        # Check if we need to reassign an interface
        $assigned_if = APITools\get_pfsense_if_id($this->initial_data["vlanif"]);
        if (!empty($assigned_if)) {
            return $assigned_if;
        }
    }
}
