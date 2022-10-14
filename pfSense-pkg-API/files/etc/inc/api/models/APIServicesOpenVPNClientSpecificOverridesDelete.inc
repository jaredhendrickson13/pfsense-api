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

class APIServicesOpenVPNClientSpecificOverridesDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-openvpn-server"];
        $this->change_note = "Deleted OpenVPN client specific override via API";
        $this->init_config("openvpn", "openvpn-csc");
    }

    public function action() {
        # Remove user from backend and remove from config
        openvpn_delete_csc($this->config["openvpn"]["openvpn-csc"][$this->id]);
        unset($this->config["openvpn"]["openvpn-csc"][$this->id]);
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Originally referred to this field as 'refid'. Still allow that value for backwards compatibility.
        if (isset($this->initial_data["refid"])) {
            $this->initial_data["id"] = $this->initial_data["refid"];
        }

        if (isset($this->initial_data['id'])) {
            if (array_key_exists((int) $this->initial_data['id'], $this->config['openvpn']['openvpn-csc'])) {
                $this->id = (int) $this->initial_data['id'];
                $this->validated_data['id'] = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(2078);
            }
        } else {
            $this->errors[] = APIResponse\get(2077);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
    }
}
