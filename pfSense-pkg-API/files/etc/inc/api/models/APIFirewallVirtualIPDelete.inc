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

class APIFirewallVirtualIPDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Deleted virtual IP via API";
    }

    public function action() {
        # Capture the vip to be deleted, remove it, write the changes to config, and reload the firewall filter
        $del_vip = $this->get_config("virtualip/vip/{$this->id}");
        $this->delete_virtual_ip($del_vip, $this->id);
        $this->write_config();
        send_event("filter reload");
        return APIResponse\get(0, $del_vip);
    }
    
    public function validate_payload() {
        if (isset($this->initial_data['id'])) {
            # Check that our rule ID exists
            if ($this->get_config("virtualip/vip/{$this->initial_data['id']}", false)) {
                $this->id = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4018);
            }
        } else {
            $this->errors[] = APIResponse\get(4017);
        }
    }

    public static function delete_virtual_ip($vip_ent, $id, $delete=true) {
        # Safely remove and bring proxyarp virtual IPs
        if ($vip_ent['mode'] == "proxyarp") {
            if ($delete) {
                APIModel::del_config("virtualip/vip/{$id}");
            }
            $viface = $vip_ent['interface'];
            interface_proxyarp_configure($viface);
        }
        # Safely remove and bring down all other virtual IP types
        else {
            interface_vip_bring_down($vip_ent);
            if ($delete) {
                APIModel::del_config("virtualip/vip/{$id}");
            }
        }

        # Remove leftover configuration arrays if there are no virtual IPs left
        if ($delete and count(APIModel::get_config('virtualip/vip')) == 0) {
            APIModel::del_config("virtualip/vip");
        }
    }
}
