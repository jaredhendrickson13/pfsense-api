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


class APISystemARPDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-arptable"];
    }

    public function action() {
        $del_ent = APITools\get_arp_entry("ip", $this->validated_data["ip"]);    // Save our deleted ARP config
        exec("arp -d ".$this->validated_data["ip"], $arp_del);    // Delete our ARP address
        return APIResponse\get(0, $del_ent);
    }
    
    public function validate_payload() {
        if (isset($this->initial_data['ip'])) {
            $this->validated_data["ip"] = $this->initial_data['ip'];
            $this->validated_data["ip"] = trim($this->validated_data["ip"]);
        } else {
            $this->errors[] = APIResponse\get(1006);
        }
    }
}
