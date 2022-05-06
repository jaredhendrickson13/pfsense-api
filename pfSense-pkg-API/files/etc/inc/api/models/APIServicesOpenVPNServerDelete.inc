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

class APIServicesOpenVPNServerDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-openvpn-server"];
        $this->change_note = "Deleted OpenVPN server via API";
    }

    public function action() {
        # Remove user from backend and remove from config
        openvpn_delete("server",$this->config["openvpn"]["openvpn-server"][$this->id]);
        unset($this->config["openvpn"]["openvpn-server"][$this->id]);
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_vpnid() {
        if (isset($this->initial_data['vpnid'])) {
            $counter = 0;
            foreach ($this->config["openvpn"]["openvpn-server"] as $server) {
                if ((string) $this->initial_data['vpnid'] === (string) $server["vpnid"]) {
                    $this->id = $counter;
                    $this->validated_data['vpnid'] = (string) $this->initial_data['vpnid'];
                    break;
                } 
                
                $counter += 1;
            }

            if (empty($this->validated_data["vpnid"])) {
                $this->errors[] = APIResponse\get(6075);
            }
        } else {
            $this->errors[] = APIResponse\get(6076);
        }
    }

    public function validate_payload() {
        $this->__validate_vpnid();
    }
}
