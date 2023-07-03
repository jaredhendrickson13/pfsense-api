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


class APIInterfaceVLANDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-vlan-edit"];
        $this->change_note = "Deleted interface VLAN via API";
    }

    public function action() {
        pfSense_interface_destroy($this->validated_data["vlanif"]);
        $this->del_config("vlans/vlan/{$this->id}");
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

        # Lastly check that our interface is not in use currently
        if (APITools\get_pfsense_if_id($this->validated_data["vlanif"])) {
            $this->errors[] = APIResponse\get(3049);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
    }

    private function __get_vlan_id_from_vlanif($vlanif) {
        # Loop through each VLAN and return it's ID if it matches
        foreach ($this->get_config("vlans/vlan", []) as $id => $vlan) {
            if ($vlanif === $vlan["vlanif"]) {
                return $id;
            }
        }
    }
}
