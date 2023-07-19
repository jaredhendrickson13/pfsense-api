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

class APIServicesOpenVPNClientSpecificOverridesUpdate extends APIModel {
    public $original_csc;

    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Modified OpenVPN client specific override via API";
    }

    public function action() {
        # Update our new user in the config and set the user on the backend
        $this->set_config("openvpn/openvpn-csc/{$this->id}", $this->validated_data);
        $this->write_config();

        # Remove the old csc values and sync the new ones
        openvpn_delete_csc($this->original_csc);
        openvpn_resync_csc($this->validated_data);
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Originally referred to this field as 'refid'. Still allow that value for backwards compatibility.
        if (isset($this->initial_data["refid"])) {
            $this->initial_data["id"] = $this->initial_data["refid"];
        }

        if (isset($this->initial_data['id'])) {
            if ($this->get_config("openvpn/openvpn-csc/{$this->initial_data['id']}", false)) {
                $this->id = (int) $this->initial_data['id'];
                $this->validated_data = $this->get_config("openvpn/openvpn-csc/{$this->id}", []);
                $this->original_csc = $this->get_config("openvpn/openvpn-csc/{$this->id}", []);

            } else {
                $this->errors[] = APIResponse\get(2078);
            }
        } else {
            $this->errors[] = APIResponse\get(2079);
        }
    }

    private function __validate_server_list() {
        # Check if mode is one of these
        $server_list = $this->get_config('openvpn/openvpn-server', []);
        $server_ids = [];

        foreach ($server_list as $server) {
            $server_ids[] = (int) $server['vpnid'];
        }

        if (isset($this->initial_data['server_list'])) {
            $this->validated_data["server_list"] = (string) $this->initial_data['server_list'];
        } else {
            $this->validated_data["server_list"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/server_list", []);
        }
    }

    private function __validate_custom_options() {
        if (isset($this->initial_data['custom_options'])) {
            $this->validated_data["custom_options"] = $this->initial_data['custom_options'];
        } else {
            $this->validated_data["custom_options"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/custom_options");
        }
    }

    private function __validate_common_name() {
        if (isset($this->initial_data['common_name'])) {
            $this->validated_data["common_name"] = $this->initial_data['common_name'];
        } else {
            $this->validated_data["common_name"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/common_name");
        }
    }

    private function __validate_disabled() {
        # Check for our optional `disabled` payload value
        if (isset($this->initial_data["disable"])) {
            if ($this->initial_data["disable"] === true) {
                $this->validated_data["disable"] = "";
            }
        } else {
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "disable")) {
                $this->validated_data["disable"] = "";
            }
        }
    }

    private function __validate_block() {
        # Check for our optional `disabled` payload value
        if (isset($this->initial_data["block"])) {
            if ($this->initial_data["block"] === true) {
                $this->validated_data["block"] = "yes";
            }
            else {
                $this->validated_data["block"] = "";
            }
        } else {
            $this->validated_data["block"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/block");
        }
    }

    private function __validate_description() {
        # Check for our optional `descr` payload value
        if (isset($this->initial_data['description'])) {
            $this->validated_data["description"] = $this->initial_data['description'];
        }
        else {
            $this->validated_data["description"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/description");
        }
    }

    private function __validate_tunnel_network() {
        # Check for our optional `expires` payload value
        if (isset($this->initial_data['tunnel_network'])) {
            $this->validated_data["tunnel_network"] = $this->initial_data['tunnel_network'];
        }
        else {
            $this->validated_data["tunnel_network"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/tunnel_network");
        }
    }

    private function __validate_tunnel_networkv6() {
        if (isset($this->initial_data['tunnel_networkv6'])) {
            $this->validated_data["tunnel_networkv6"] = $this->initial_data['tunnel_networkv6'];
        }
        else {
            $this->validated_data["tunnel_networkv6"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/tunnel_networkv6");
        }
    }

    private function __validate_local_network() {
        if (isset($this->initial_data['local_network'])) {
            $this->validated_data["local_network"] = $this->initial_data['local_network'];
        }
        else {
            $this->validated_data["local_network"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/local_network");
        }
    }

    private function __validate_local_networkv6() {
        if (isset($this->initial_data['local_networkv6'])) {
            $this->validated_data["local_networkv6"] = $this->initial_data['local_networkv6'];
        }
        else {
            $this->validated_data["local_networkv6"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/local_networkv6");
        }
    }

    private function __validate_remote_network() {
        if (isset($this->initial_data['remote_network'])) {
            $this->validated_data["remote_network"] = $this->initial_data['remote_network'];
        }
        else {
            $this->validated_data["remote_network"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/remote_network");
        }
    }

    private function __validate_remote_networkv6() {
        if (isset($this->initial_data['remote_networkv6'])) {
            $this->validated_data["remote_networkv6"] = $this->initial_data['remote_networkv6'];
        }
        else {
            $this->validated_data["remote_networkv6"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/remote_networkv6");
        }
    }

    private function __validate_redirect_gateway() {
        if (isset($this->initial_data['redirect_gateway'])) {
            if ($this->initial_data['redirect_gateway'] === true) {
                $this->validated_data["gwredir"] = "yes";
            }
            else {
                $this->validated_data["gwredir"] = "";
            }
        } else {
            $this->validated_data["gwredir"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/gwredir");
        }
    }

    private function __validate_prevent_server_definitions() {
        if (isset($this->initial_data['prevent_server_definitions'])) {
            if ($this->initial_data['prevent_server_definitions'] === true) {
                $this->validated_data["push_reset"] = "yes";
            }
            else {
                $this->validated_data["push_reset"] = "";
            }
        } else {
            $this->validated_data["push_reset"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/push_reset");
        }
    }

    private function __validate_remove_server_routes() {
        if (isset($this->initial_data['remove_server_routes'])) {
            if ($this->initial_data['remove_server_routes'] === true) {
                $this->validated_data["remove_route"] = "yes";
            }
            else {
                $this->validated_data["remove_route"] = "";
            }
        } else {
            $this->validated_data["remote_route"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/remove_route");
        }
    }

    private function __validate_dns_domain() {
        if (isset($this->initial_data['dns_domain'])) {
            $this->validated_data["dns_domain"] = $this->initial_data['dns_domain'];
        }
        else {
            $this->validated_data["dns_domain"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/dns_domain");
        }
    }

    private function __validate_netbios_enable() {
        if (isset($this->initial_data['remove_server_routes'])) {
            if ($this->initial_data['netbios_enable'] === true) {
                $this->validated_data["netbios_enable"] = "yes";
            }
            else {
                $this->validated_data["netbios_enable"] = "";
            }
        } else {
            $this->validated_data["netbios_enable"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/netbios_enable");
        }
    }

    private function __validate_netbios_scope() {
        if (isset($this->initial_data['netbios_scope'])) {
            $this->validated_data["netbios_scope"] = $this->initial_data['netbios_scope'];
        }
        else {
            $this->validated_data["netbios_scope"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/netbios_scope");
        }
    }

    private function __validate_dns_servers() {
        if (isset($this->initial_data['dns_servers'])) {
            $dns_servers = explode(',', $this->initial_data['dns_servers']);

            $counter = 1;

            foreach ($dns_servers as $server) {
                if ($counter > 4)
                {
                    break;
                }

                $this->validated_data["dns_server" . $counter] = str_replace(" ", "", $server);
                $counter += 1;
            }
        } else {
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "dns_server1"))
            {
                $this->validated_data["dns_server1"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/dns_server1");
            }
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "dns_server2"))
            {
                $this->validated_data["dns_server2"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/dns_server2");
            }
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "dns_server3"))
            {
                $this->validated_data["dns_server3"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/dns_server3");
            }
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "dns_server4"))
            {
                $this->validated_data["dns_server4"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/dns_server4");
            }
        }
    }

    private function __validate_ntp_servers() {
        if (isset($this->initial_data['ntp_servers'])) {
            $ntp_servers = explode(',', $this->initial_data['ntp_servers']);

            $counter = 1;

            foreach ($ntp_servers as $server) {
                if ($counter > 2)
                {
                    break;
                }

                $this->validated_data["ntp_server" . $counter] = str_replace(" ", "", $server);
                $counter += 1;
            }
        } else {
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "ntp_server1"))
            {
                $this->validated_data["ntp_server1"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/ntp_server1");
            }
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "ntp_server2"))
            {
                $this->validated_data["ntp_server2"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/ntp_server2");
            }
        }
    }

    private function __validate_win_servers() {
        if (isset($this->initial_data['wins_servers'])) {
            $win_servers = explode(',', $this->initial_data['wins_servers']);

            $counter = 1;

            foreach ($win_servers as $server) {
                if ($counter > 2)
                {
                    break;
                }

                $this->validated_data["wins_server" . $counter] = str_replace(" ", "", $server);
                $counter += 1;
            }
        } else {
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "wins_server1"))
            {
                $this->validated_data["wins_server1"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/wins_server1");
            }
            if ($this->is_config_enabled("openvpn/openvpn-csc/{$this->id}", "wins_server2"))
            {
                $this->validated_data["wins_server2"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/wins_server2");
            }
        }
    }

    private function __validate_netbios_ntype() {
        if (isset($this->initial_data['netbios_node_type'])){
            if ($this->initial_data['netbios_node_type'] == 'b') {
                $this->validated_data["netbios_ntype"] = 1;
            } else if ($this->initial_data['netbios_node_type'] == 'p') {
                $this->validated_data["netbios_ntype"] = 2;
            } else if ($this->initial_data['netbios_node_type'] == 'm') {
                $this->validated_data["netbios_ntype"] = 3;
            } else if ($this->initial_data['netbios_node_type'] == 'h') {
                $this->validated_data["netbios_ntype"] = 4;
            } else {
                $this->errors[] = APIResponse\get(2076);
            }
        } elseif (!empty($this->get_config("openvpn/openvpn-csc/{$this->id}/netbios_ntype"))) {
            $this->validated_data["netbios_ntype"] = $this->get_config("openvpn/openvpn-csc/{$this->id}/netbios_ntype");
        } else {
            if (isset($this->initial_data['netbios_enable'])) {
                $this->validated_data["netbios_ntype"] = 1;
            } else {
                $this->validated_data["netbios_ntype"] = 0;
            }
        }
    }

    public function validate_payload() {
        # Run each validation method
        $this->__validate_id();
        $this->__validate_server_list();
        $this->__validate_custom_options();
        $this->__validate_disabled();
        $this->__validate_common_name();
        $this->__validate_block();
        $this->__validate_description();
        $this->__validate_tunnel_network();
        $this->__validate_tunnel_networkv6();
        $this->__validate_local_network();
        $this->__validate_local_networkv6();
        $this->__validate_remote_network();
        $this->__validate_remote_networkv6();
        $this->__validate_redirect_gateway();
        $this->__validate_remove_server_routes();
        $this->__validate_prevent_server_definitions();
        $this->__validate_dns_domain();
        $this->__validate_dns_servers();
        $this->__validate_ntp_servers();
        $this->__validate_netbios_enable();
        $this->__validate_netbios_ntype();
        $this->__validate_netbios_scope();
        $this->__validate_win_servers();

        $this->validated_data["nbdd_server1"] = "";
    }
}
