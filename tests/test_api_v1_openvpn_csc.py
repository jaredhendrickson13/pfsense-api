# Copyright 2021 Jared Hendrickson
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import unit_test_framework

class APIUnitTestOpenVPNClientSpecificOverrides(unit_test_framework.APIUnitTest):
    uri = "/api/v1/openvpn/csc"
    get_tests = [
        {"name": "Read all OpenVPN client specific overrides"}
    ]
    post_tests = [
        {
            "name": "Create a client specific override",
            "payload": {
                "custom_options": "ifconfig-push xxx.xxx.xxx.xxx 255.255.255.0;",
                "common_name": "commonname",
                "description": "An override, that is specific..."
            }
        },
        {
            "name": "Missing Common Name",
            "payload": {
                "custom_options": "ifconfig-push xxx.xxx.xxx.xxx 255.255.255.0;",
                "description": "An override, that is specific..."
            },
            "return": 6030,
            "status": 400
        },
        {
            "name": "Unknown NetBIOS Node Type",
            "payload": {
                "custom_options": "ifconfig-push xxx.xxx.xxx.xxx 255.255.255.0;",
                "common_name": "commonname",
                "description": "An override, that is specific...",
                "netbios_node_type": "q"
            },
            "return": 6031,
            "status": 400
        },
        {
            "name": "All options",
            "payload": {
                "server_list": "1, 2, 3",
                "custom_options": "ifconfig-push xxx.xxx.xxx.xxx 255.255.255.0;",
                "disable": True,
                "common_name": "commonname",
                "block": True,
                "description": "An override, that is specific...",
                "tunnel_network": "10.0.8.5/24",
                "tunnel_networkv6": "2001:db9:1:1::100/64",
                "local_network": "10.0.9.5/24, 10.0.8.5/24, 10.0.10.5/24",
                "local_networkv6": "2001:db9:1:1::100/64, 2002:db9:1:1::100/64, 2003:db9:1:1::100/64",
                "remote_network": "10.0.9.5/24, 10.0.8.5/24, 10.0.10.5/24",
                "remote_networkv6": "2001:db9:1:1::100/64, 2002:db9:1:1::100/64, 2003:db9:1:1::100/64",
                "redirect_gateway": True,
                "prevent_server_definitions": True,
                "remove_server_routes": True,
                "dns_domain": "google.com",
                "dns_servers": "8.8.8.8, 8.8.4.4, 8.8.3.3, 8.8.2.2",
                "ntp_servers": "192.168.56.101, 192.168.56.102",
                "netbios_enable": True,
                "netbios_node_type": "p",
                "netbios_scope": "5",
                "wins_servers": "192.168.56.103, 192.168.56.104",
            },
        },
    ]
    put_tests = [
        {
            "name": "Update a client specific override",
            "payload": {
                "refid": 0,
                "common_name": "commonname2",
            }
        },
        {
            "name": "Missing Reference ID",
            "payload": {
                "description": "An override, that is pretty specific..."
            },
            "return": 6034,
            "status": 400
        },
        {
            "name": "Unknown NetBIOS Node Type",
            "payload": {
                "refid": 0,
                "netbios_node_type": "q"
            },
            "return": 6031,
            "status": 400
        },
        {
            "name": "All options",
            "payload": {
                "refid": 1,
                "server_list": "1, 2",
                "custom_options": "ifconfig-push 10.10.10.1 255.255.255.0;",
                "disable": False,
                "common_name": "commonname3",
                "block": False,
                "description": "An override, that is VERY specific...",
                "tunnel_network": "10.0.8.6/24",
                "tunnel_networkv6": "2001:dc9:1:1::100/64",
                "local_network": "10.0.9.6/24, 10.0.8.5/24, 10.0.10.6/24",
                "local_networkv6": "2001:dc9:1:1::100/64, 2002:dc9:1:1::100/64, 2003:dc9:1:1::100/64",
                "remote_network": "10.0.9.6/24, 10.0.8.5/24, 10.0.10.6/24",
                "remote_networkv6": "2001:dc9:1:1::100/64, 2002:dc9:1:1::100/64, 2003:dc9:1:1::100/64",
                "redirect_gateway": False,
                "prevent_server_definitions": False,
                "remove_server_routes": False,
                "dns_domain": "github.com",
                "dns_servers": "8.8.8.7, 8.8.4.7, 8.8.3.7, 8.8.2.7",
                "ntp_servers": "192.168.56.106, 192.168.56.107",
                "netbios_enable": False,
                "netbios_node_type": "m",
                "netbios_scope": "42",
                "wins_servers": "192.168.56.108, 192.168.56.194",
            },
        },
    ]
    delete_tests = [
        {
            "name": "Delete client specific override",
            "payload": {"refid": 0}
        },
    ]

APIUnitTestOpenVPNClientSpecificOverrides()
