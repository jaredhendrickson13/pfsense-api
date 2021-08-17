<?php
//   Copyright 2021 Jared Hendrickson
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

class APIOpenVPNClientSpecificOverridesCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-openvpn-server"];
        $this->change_note = "Added OpenVPN client specific override via API";
    }

    public function action() {
        # Add our Client Specific Override to the configuration
        $this->config['openvpn']['openvpn-csc'][] = $this->validated_data;

        # Write the user to configuration and set the user on the backend. Return response with created user object.
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_server_list() {
        # Check if mode is one of these
        $server_list = $this->config['openvpn']['openvpn-server'];
        $server_ids = [];

        foreach ($server_list as $server) {
            $server_ids[] = (int) $server['vpnid'];
        }

        if (isset($this->initial_data['server_list'])) {
            $ini_data_sl = explode(',', $this->initial_data['server_list']);

            foreach ($init_data_sl as $server_id) {
                if (!is_numeric($server_id))
                {
                    $this->errors[] = APIResponse\get(6029);
                    break;
                }
                if (in_array((int) $server_id, $server_ids)) {
                    continue;
                } else {
                    $this->errors[] = APIResponse\get(6029);
                    break;
                }
            }

            $this->validated_data["server_list"] = $this->initial_data['server_list'];
        } else {
            $this->validated_data["server_list"] = "";
        }
    }

    private function __validate_custom_options() {
        if (isset($this->initial_data['custom_options'])) {
            $this->validated_data["custom_options"] = $this->initial_data['custom_options'];
        } else {
            $this->validated_data["custom_options"] = "";
        }
    }

    private function __validate_common_name() {
        if (isset($this->initial_data['common_name'])) {
            $this->validated_data["common_name"] = $this->initial_data['common_name'];
        } else {
            $this->errors[] = APIResponse\get(6030);
        }
    }

    private function __validate_disabled() {
        # Check for our optional `disabled` payload value
        if ($this->initial_data["disable"] === true) {
            $this->validated_data["disable"] = "";
        }
    }

    private function __validate_block() {
        # Check for our optional `disabled` payload value
        if ($this->initial_data["block"] === true) {
            $this->validated_data["block"] = "yes";
        }
        else {
            $this->validated_data["block"] = "";
        }
    }

    private function __validate_description() {
        # Check for our optional `descr` payload value
        if (isset($this->initial_data['description'])) {
            $this->validated_data["description"] = $this->initial_data['description'];
        }
        else {
            $this->validated_data["description"] = "";
        }
    }

    private function __validate_tunnel_network() {
        # Check for our optional `expires` payload value
        if (isset($this->initial_data['tunnel_network'])) {
            $this->validated_data["tunnel_network"] = $this->initial_data['tunnel_network'];
        }
        else {
            $this->validated_data["tunnel_network"] = "";
        }
    }

    private function __validate_tunnel_networkv6() {
        if (isset($this->initial_data['tunnel_networkv6'])) {
            $this->validated_data["tunnel_networkv6"] = $this->initial_data['tunnel_networkv6'];
        }
        else {
            $this->validated_data["tunnel_networkv6"] = "";
        }
    }

    private function __validate_local_network() {
        if (isset($this->initial_data['local_network'])) {
            $this->validated_data["local_network"] = $this->initial_data['local_network'];
        }
        else {
            $this->validated_data["local_network"] = "";
        }
    }

    private function __validate_local_networkv6() {
        if (isset($this->initial_data['local_networkv6'])) {
            $this->validated_data["local_networkv6"] = $this->initial_data['local_networkv6'];
        }
        else {
            $this->validated_data["local_networkv6"] = "";
        }
    }

    private function __validate_remote_network() {
        if (isset($this->initial_data['remote_network'])) {
            $this->validated_data["remote_network"] = $this->initial_data['remote_network'];
        }
        else {
            $this->validated_data["remote_network"] = "";
        }
    }

    private function __validate_remote_networkv6() {
        if (isset($this->initial_data['remote_networkv6'])) {
            $this->validated_data["remote_networkv6"] = $this->initial_data['remote_networkv6'];
        }
        else {
            $this->validated_data["remote_networkv6"] = "";
        }
    }

    private function __validate_redirect_gateway() {
        if ($this->initial_data['redirect_gateway'] === true) {
            $this->validated_data["gwredir"] = "yes";
        }
        else {
            $this->validated_data["gwredir"] = "";
        }
    }

    private function __validate_prevent_server_definitions() {
        if ($this->initial_data['prevent_server_definitions'] === true) {
            $this->validated_data["push_reset"] = "yes";
        }
        else {
            $this->validated_data["push_reset"] = "";
        }
    }

    private function __validate_remove_server_routes() {
        if ($this->initial_data['remove_server_routes'] === true) {
            $this->validated_data["remove_route"] = "yes";
        }
        else {
            $this->validated_data["remove_route"] = "";
        }
    }

    private function __validate_dns_domain() {
        if (isset($this->initial_data['dns_domain'])) {
            $this->validated_data["dns_domain"] = $this->initial_data['dns_domain'];
        }
        else {
            $this->validated_data["dns_domain"] = "";
        }
    }

    private function __validate_netbios_enable() {
        if ($this->initial_data['netbios_enable'] === true) {
            $this->validated_data["netbios_enable"] = "yes";
        }
        else {
            $this->validated_data["netbios_enable"] = "";
        }
    }

    private function __validate_netbios_scope() {
        if (isset($this->initial_data['netbios_scope'])) {
            $this->validated_data["netbios_scope"] = $this->initial_data['netbios_scope'];
        }
        else {
            $this->validated_data["netbios_scope"] = "";
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
        }
    }

    private function __validate_win_servers() {
        if (isset($this->initial_data['win_servers'])) {
            $win_servers = explode(',', $this->initial_data['win_servers']);

            $counter = 1;

            foreach ($win_servers as $server) {
                if ($counter > 2)
                {
                    break;
                }

                $this->validated_data["win_server" . $counter] = str_replace(" ", "", $server);
                $counter += 1;
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
                $this->errors[] = APIResponse\get(6031);
            }
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
