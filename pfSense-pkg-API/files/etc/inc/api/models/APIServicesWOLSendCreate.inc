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

class APIServicesWOLSendCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-wakeonlan"];
        $this->change_note = "Sent magic WOL packet via API";
    }

    public function action() {
        $if = $this->validated_data["interface"];
        $if_ip = $this->validated_data["interface_ip"];
        $mac = $this->validated_data["mac"];

        $bc_ip = $this->__get_broadcast_ip($if, $if_ip);
        $response_data = array_merge($this->validated_data, ["broadcast_ip" => $bc_ip]);

        if (!mwexec("/usr/local/bin/wol -i {$bc_ip} {$mac}")) {
            return APIResponse\get(0, $response_data);
        } else {
            return APIResponse\get(2103, $response_data);
        }
    }

    private function __get_broadcast_ip($if, $if_ip) {
        return gen_subnet_max($if_ip, get_interface_subnet($if));
    }

    public function validate_payload() {
        $this->__validate_interface();
        $this->__validate_mac();
    }

    private function __validate_interface() {
        if (isset($this->initial_data["interface"])) {
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface']);
            $interface_ip = get_interface_ip($interface);
            if (is_ipaddr($interface_ip)) {
                $this->validated_data["interface"] = $interface;
                $this->validated_data["interface_ip"] = $interface_ip;
            } else {
                $this->errors[] = APIResponse\get(2100);
            }
        } else {
            $this->errors[] = APIResponse\get(2099);
        }
    }

    private function __validate_mac() {
        if (isset($this->initial_data["mac"])) {
            if (is_macaddr($this->initial_data["mac"])) {
                $this->validated_data["mac"] = $this->initial_data["mac"];
            } else {
                $this->errors[] = APIResponse\get(2102);
            }
        } else {
            $this->errors[] = APIResponse\get(2101);
        }
    }
}
