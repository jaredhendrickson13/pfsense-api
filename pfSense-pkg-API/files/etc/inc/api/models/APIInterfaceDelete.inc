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


class APIInterfaceDelete extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Deleted interface via API";
    }

    public function action() {
        $curr_config = $this->get_config("interfaces/{$this->validated_data["if"]}");
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
        # Do not allow deletion of WAN interface
        if ($id === "wan") {
            $err_msg = 3042;
        }
        # Do not allow deletion if interface is in group
        elseif (link_interface_to_group($id)) {
            $err_msg = 3043;
        }
        # Do not allow deletion if interface is in bridge
        elseif (link_interface_to_bridge($id)) {
            $err_msg = 3044;
        }
        # Do not allow interface to be deleted if it is assigned GRE interface
        elseif (link_interface_to_tunnelif($id, 'gre')) {
            $err_msg = 3045;
        }
        # Do not allow interface to be deleted if it is assigned GIF interface
        elseif (link_interface_to_tunnelif($id, 'gif')) {
            $err_msg = 3046;
        }
        # Do not allow interface to be deleted if it has a traffic queue assigned
        elseif (interface_has_queue($id)) {
            $err_msg = 3047;
        }
        # Otherwise, run through the steps to delete the interface
        else {
            # Remove the 'enable' value first, then bring down the interface and delete it from config
            $this->del_config("interfaces/{$id}/enable");
            $realid = get_real_interface($id);
            interface_bring_down($id);
            $this->del_config("interfaces/{$id}");

            # Remove DHCP config for interface
            if (is_array($this->get_config("dhcpd/{$id}"))) {
                $this->del_config("dhcpd/{$id}");
                services_dhcpd_configure('inet');
            }
            # Remove interface config for dhcp6
            if (is_array($this->get_config("dhcpdv6/{$id}"))) {
                $this->del_config("dhcpdv6/{$id}");
                services_dhcpd_configure('inet6');
            }
            # Remove ACL for interface
            foreach ($this->get_config('filter/rule', []) as $x => $rule) {
                if ($rule['interface'] == $id) {
                    $this->del_config("filter/rule/{$x}");
                }
            }
            # Remove NAT config for interface
            foreach ($this->get_config('nat/rule', []) as $x => $rule) {
                if ($rule['interface'] == $id) {
                    $this->del_config("nat/rule/{$x}");
                }
            }

            # Disable DHCP if last interface
            if (count($this->get_config('interfaces', [])) <= 1 && $this->get_config('dhcpd/wan')) {
                $this->del_config('dhcpd/wan');
            }

            # Update VLAN assignments
            link_interface_to_vlans($realid, "update");
            $err_msg = 0;
        }
        return $err_msg;
    }
}
