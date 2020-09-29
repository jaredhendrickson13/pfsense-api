<?php
//   Copyright 2020 Jared Hendrickson
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
        $this->config["vlans"]["vlan"] = [];
        $this->config["vlans"]["vlan"][] = $this->validated_data;    // Write our configuration change
        interface_vlan_configure($this->validated_data);    // Configure our VLAN on the backend
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['if'])) {
            if (!does_interface_exist($this->initial_data['if'])) {
                $this->errors[] = APIResponse\get(3051);
            } else {
                $this->validated_data["if"] = $this->initial_data['if'];
            }
        } else {
            $this->errors[] = APIResponse\get(3055);
        }

        if (isset($this->initial_data['tag'])) {
            if (!is_numeric($this->initial_data['tag']) or (1 > intval($this->initial_data['tag']) or intval($this->initial_data['tag']) > 4096)) {
                $this->errors[] = APIResponse\get(3052);
            } else {
                $this->validated_data["tag"] = intval(trim($this->initial_data['tag']));
                $str_tag = strval($this->validated_data["tag"]);
            }
        } else {
            $this->errors[] = APIResponse\get(3048);
        }

        if (isset($this->initial_data['pcp'])) {
            if (0 > $this->initial_data['pcp'] or $this->initial_data['pcp'] > 7) {
                $this->errors[] = APIResponse\get(3053);
            } else {
                $this->validated_data["pcp"] = intval(trim($this->initial_data['pcp']));
            }
        } else {
            $this->validated_data["pcp"] = "";
        }

        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        } else {
            $this->validated_data["descr"] = "";
        }

        // Check if our VLAN is already in use
        if (is_array($this->config["vlans"]["vlan"])) {
            foreach ($this->config["vlans"]["vlan"] as $vle) {
                if ($this->validated_data["if"] === $vle["if"] and $str_tag === $vle["tag"]) {
                    $this->errors[] = APIResponse\get(3054);
                }
            }
        } else {
            $this->config["vlans"] = [];
            $this->config["vlans"]["vlan"] = [];
        }
        $this->validated_data["vlanif"] = $this->validated_data["if"].".".$this->initial_data['tag'];

    }
}