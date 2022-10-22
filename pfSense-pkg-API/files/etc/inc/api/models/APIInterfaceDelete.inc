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


class APIInterfaceDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-assignnetworkports"];
        $this->change_note = "Deleted interface via API";
        $this->init_config('interfaces');
    }

    public function action() {
        $curr_config = [$this->validated_data["if"] => $this->config["interfaces"][$this->validated_data["if"]]];
        $del_stat = $this->destroy_interface($this->validated_data["if"]);
        $this->write_config();
        return APIResponse\get($del_stat, ($del_stat === 0) ? $curr_config : []);
    }

    public function validate_payload() {
        if (isset($this->initial_data['if'])) {
            if (empty(APITools\get_pfsense_if_id($this->initial_data['if']))) {
                $this->errors[] = APIResponse\get(3000);
            } else {
                $this->validated_data["if"] = APITools\get_pfsense_if_id($this->initial_data['if']);
            }
        } else {
            $this->errors[] = APIResponse\get(3002);
        }
    }

    # Delete an interface given it's interface ID
    private function destroy_interface($id) {
        if ($id === "wan") {
            $err_msg = 3042;
        } elseif (link_interface_to_group($id)) {
            $err_msg = 3043;
        } elseif (link_interface_to_bridge($id)) {
            $err_msg = 3044;
        } elseif (link_interface_to_tunnelif($id, 'gre')) {
            $err_msg = 3045;
        } elseif (link_interface_to_tunnelif($id, 'gif')) {
            $err_msg = 3046;
        } elseif (interface_has_queue($id)) {
            $err_msg = 3047;
        } else {
            unset($this->config['interfaces'][$id]['enable']);
            $realid = get_real_interface($id);
            interface_bring_down($id);   // Bring down interface
            unset($this->config['interfaces'][$id]);	// Delete our interface from configuration
            // Remove DHCP config for interface
            if (is_array($this->config['dhcpd']) && is_array($this->config['dhcpd'][$id])) {
                unset($this->config['dhcpd'][$id]);
                services_dhcpd_configure('inet');
            }
            // Removed interface config for dhcp6
            if (is_array($this->config['dhcpdv6']) && is_array($this->config['dhcpdv6'][$id])) {
                unset($this->config['dhcpdv6'][$id]);
                services_dhcpd_configure('inet6');
            }
            // Remove ACL for interface
            if (count($this->config['filter']['rule']) > 0) {
                foreach ($this->config['filter']['rule'] as $x => $rule) {
                    if ($rule['interface'] == $id) {
                        unset($this->config['filter']['rule'][$x]);
                    }
                }
            }
            // Remove NAT config for interface
            if (is_array($this->config['nat']['rule']) && count($this->config['nat']['rule']) > 0) {
                foreach ($this->config['nat']['rule'] as $x => $rule) {
                    if ($rule['interface'] == $id) {
                        unset($this->config['nat']['rule'][$x]['interface']);
                    }
                }
            }

            // Disable DHCP if last interface
            if ($this->config['interfaces']['lan'] && $this->config['dhcpd']['wan']) {
                unset($this->config['dhcpd']['wan']);
            }
            // Update VLAN assignments
            link_interface_to_vlans($realid, "update");
            $err_msg = 0;
        }
        return $err_msg;
    }
}
