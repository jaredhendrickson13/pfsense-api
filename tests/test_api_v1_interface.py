# Copyright 2022 Jared Hendrickson
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
"""Script used to test the /api/v1/interface endpoint."""
import e2e_test_framework


class APIE2ETestInterface(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/interface endpoint."""
    uri = "/api/v1/interface"
    get_tests = [
        {"name": "Read all configured interfaces"}
    ]
    post_tests = [
        {
            "name": "Create firewall alias for testing",
            "uri": "/api/v1/firewall/alias",
            "req_data": {
                "name": "TEST_ALIAS",
                "type": "host",
                "address": "1.1.1.1"
            }
        },
        {
            "name": "Check physical interface requirement",
            "status": 400,
            "return": 3002
        },
        {
            "name": "Check physical interface exists constraint",
            "status": 400,
            "return": 3000,
            "req_data": {
                "if": "INVALID"
            }
        },
        {
            "name": "Check physical interface in use constraint",
            "status": 400,
            "return": 3001,
            "req_data": {
                "if": "em0"
            }
        },
        {
            "name": "Check IPv4 type option constraint",
            "status": 400,
            "return": 3025,
            "req_data": {
                "if": "em2",
                "type": "INVALID"
            }
        },
        {
            "name": "Check IPv4 type option constraint",
            "status": 400,
            "return": 3041,
            "req_data": {
                "if": "em2",
                "type6": "INVALID"
            }
        },
        {
            "name": "Check spoof MAC address validation",
            "status": 400,
            "return": 3003,
            "req_data": {
                "if": "em2",
                "spoofmac": "INVALID"
            }
        },
        {
            "name": "Check MTU minimum constraint",
            "status": 400,
            "return": 3004,
            "req_data": {
                "if": "em2",
                "mtu": 1279
            }
        },
        {
            "name": "Check MTU maximum constraint",
            "status": 400,
            "return": 3004,
            "req_data": {
                "if": "em2",
                "mtu": 8193
            }
        },
        {
            "name": "Check MSS minimum constraint",
            "status": 400,
            "return": 3005,
            "req_data": {
                "if": "em2",
                "mss": 575
            }
        },
        {
            "name": "Check MSS maximum constraint",
            "status": 400,
            "return": 3005,
            "req_data": {
                "if": "em2",
                "mss": 65536
            }
        },
        {
            "name": "Check media choices constraint",
            "status": 400,
            "return": 3007,
            "req_data": {
                "if": "em2",
                "media": "INVALID"
            }
        },
        {
            "name": "Check description cannot start with pkg_",
            "status": 400,
            "return": 3059,
            "req_data": {
                "if": "em2",
                "descr": "pkg_INVALID"
            }
        },
        {
            "name": "Check description cannot match existing alias name",
            "status": 400,
            "return": 3060,
            "req_data": {
                "if": "em2",
                "descr": "TEST_ALIAS"
            }
        },
        {
            "name": "Check description cannot be numeric",
            "status": 400,
            "return": 3061,
            "req_data": {
                "if": "em2",
                "descr": "10051"
            }
        },
        {
            "name": "Check description cannot be in use",
            "status": 400,
            "return": 3008,
            "req_data": {
                "if": "em2",
                "descr": "WAN"
            }
        },
        {
            "name": "Check static IPv4 address requirement",
            "status": 400,
            "return": 3011,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "staticv4"
            }
        },
        {
            "name": "Check static IPv4 address validation",
            "status": 400,
            "return": 3010,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "staticv4",
                "ipaddr": "INVALID"
            }
        },
        {
            "name": "Check static IPv4 address in use constraint",
            "status": 400,
            "return": 3009,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "staticv4",
                "ipaddr": "192.168.1.1"
            }
        },
        {
            "name": "Check static IPv4 address subnet requirement",
            "status": 400,
            "return": 3013,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "staticv4",
                "ipaddr": "10.90.1.1"
            }
        },
        {
            "name": "Check static IPv4 gateway exists constraint",
            "status": 400,
            "return": 3014,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "staticv4",
                "ipaddr": "10.90.1.1",
                "subnet": 24,
                "gateway": "INVALID"
            }
        },
        {
            "name": "Check DHCP IPv4 alias address validation",
            "status": 400,
            "return": 3015,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "alias-address": "INVALID"
            }
        },
        {
            "name": "Check DHCP IPv4 alias subnet validation",
            "status": 400,
            "return": 3015,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "alias-address": "192.168.54.1",
                "alias-subnet": "INVALID"
            }
        },
        {
            "name": "Check DHCP reject from address validation",
            "status": 400,
            "return": 3016,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "dhcprejectfrom": ["INVALID"]
            }
        },
        {
            "name": "Check DHCP protocol timing timeout minimum constraint",
            "status": 400,
            "return": 3017,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "adv_dhcp_pt_timeout": 0
            }
        },
        {
            "name": "Check DHCP protocol timing retry minimum constraint",
            "status": 400,
            "return": 3018,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "adv_dhcp_pt_retry": 0
            }
        },
        {
            "name": "Check DHCP protocol timing select timeout minimum constraint",
            "status": 400,
            "return": 3019,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "adv_dhcp_pt_select_timeout": -1
            }
        },
        {
            "name": "Check DHCP protocol timing reboot minimum constraint",
            "status": 400,
            "return": 3020,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "adv_dhcp_pt_reboot": 0
            }
        },
        {
            "name": "Check DHCP protocol timing backoff cutoff minimum constraint",
            "status": 400,
            "return": 3021,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "adv_dhcp_pt_backoff_cutoff": 0
            }
        },
        {
            "name": "Check DHCP protocol timing initial interval minimum constraint",
            "status": 400,
            "return": 3022,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "adv_dhcp_pt_initial_interval": 0
            }
        },
        {
            "name": "Check DHCP config file override exists constraint",
            "status": 400,
            "return": 3023,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "adv_dhcp_config_file_override": True,
                "adv_dhcp_config_file_override_path": "/path/does/not/exist"
            }
        },
        {
            "name": "Check DHCP VLAN priority choice constraint",
            "status": 400,
            "return": 3024,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type": "dhcp",
                "dhcpvlanenable": True,
                "dhcpcvpt": 8
            }
        },
        {
            "name": "Check static IPv6 address requirement",
            "status": 400,
            "return": 3028,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "staticv6"
            }
        },
        {
            "name": "Check static IPv6 address validation",
            "status": 400,
            "return": 3026,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "staticv6",
                "ipaddrv6": "INVALID"
            }
        },
        {
            "name": "Check static IPv6 address subnet requirement",
            "status": 400,
            "return": 3030,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "staticv6",
                "ipaddrv6": "0::"
            }
        },
        {
            "name": "Check static IPv6 address subnet requirement",
            "status": 400,
            "return": 3029,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "staticv6",
                "ipaddrv6": "0::",
                "subnetv6": "INVALID"
            }
        },
        {
            "name": "Check static IPv6 gateway exists constraint",
            "status": 400,
            "return": 3031,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "staticv6",
                "ipaddrv6": "0::",
                "subnetv6": 24,
                "gatewayv6": "INVALID"
            }
        },
        {
            "name": "Check DHCP IPv6 prefix delegation size choice constraint",
            "status": 400,
            "return": 3032,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "dhcp6",
                "dhcp6-ia-pd-len": "INVALID"
            }
        },
        {
            "name": "Check DHCP IPv6 VLAN priority choice constraint",
            "status": 400,
            "return": 3033,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "dhcp6",
                "dhcp6vlanenable": True,
                "dhcp6cvpt": "INVALID"
            }
        },
        {
            "name": "Check DHCP IPv6 VLAN priority choice constraint",
            "status": 400,
            "return": 3034,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "dhcp6",
                "adv_dhcp6_config_file_override": True,
                "adv_dhcp6_config_file_override_path": "INVALID PATH"
            }
        },
        {
            "name": "Check 6RD IPv6 gateway requirement",
            "status": 400,
            "return": 3036,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "6rd"
            }
        },
        {
            "name": "Check 6RD IPv6 gateway validation",
            "status": 400,
            "return": 3035,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "6rd",
                "gateway-6rd": "INVALID"
            }
        },
        {
            "name": "Check 6RD IPv6 prefix length minimum constraint",
            "status": 400,
            "return": 3037,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "6rd",
                "gateway-6rd": "1.2.3.4",
                "prefix-6rd-v4plen": -1
            }
        },
        {
            "name": "Check 6RD IPv6 prefix length maximum constraint",
            "status": 400,
            "return": 3037,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "6rd",
                "gateway-6rd": "1.2.3.4",
                "prefix-6rd-v4plen": 33
            }
        },
        {
            "name": "Check Track6 IPv6 interface requirement",
            "status": 400,
            "return": 3039,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "track6"
            }
        },
        {
            "name": "Check Track6 IPv6 interface exists constraint",
            "status": 400,
            "return": 3038,
            "req_data": {
                "if": "em2",
                "descr": "TEST",
                "type6": "track6",
                "track6-interface": "INVALID"
            }
        },
        {
            "name": "Create interface bridge for testing",
            "uri": "/api/v1/interface/bridge",
            "req_data": {
                "members": "em1"
            }
        },
        {
            "name": "Create interface VLAN for testing",
            "uri": "/api/v1/interface/vlan",
            "resp_time": 5,
            "req_data": {
                "if": "em2",
                "tag": 2
            }
        },
        {
            "name": "Create a staticv4/staticv6 interface",
            "req_data": {
                "if": "em2",
                "descr": "e2e_test",
                "enable": True,
                "type": "staticv4",
                "type6": "staticv6",
                "ipaddr": "10.250.0.1",
                "ipaddrv6": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "subnet": "24",
                "subnetv6": "120",
                "blockbogons": True
            }
        },
        {
            "name": "Create a static interface on the bridge",
            "req_data": {
                "if": "bridge0",
                "descr": "BRIDGE_TEST",
                "enable": True,
                "type": "staticv4",
                "ipaddr": "10.251.0.1",
                "subnet": 24,
                "blockbogons": True
            }
        },
        {
            "name": "Check MTU less than VLAN parent interface constraint",
            "status": 400,
            "return": 3006,
            "req_data": {
                "if": "em2.2",
                "mtu": 8192
            }
        },

    ]
    put_tests = [
        {
            "name": "Update staticv4/staticv6 interface to dhcp/dhcp6 and apply",
            "req_data": {
                "id": "em2",
                "descr": "e2e_test_UPDATED",
                "enable": False,
                "type": "dhcp",
                "type6": "dhcp6",
                "blockbogons": False,
                "apply": True
            },
            "resp_time": 12    # Allow a few seconds to bounce the interface when applying
        }
    ]
    delete_tests = [
        {
            "name": "Delete interface",
            "resp_time": 3,
            "req_data": {
                "if": "em2"
            }
        },
        {
            "name": "Check cannot delete interface bridge in use constraint",
            "uri": "/api/v1/interface/bridge",
            "status": 400,
            "return": 3073,
            "req_data": {
                "id": "bridge0"
            }
        },
        {
            "name": "Delete bridged interface",
            "req_data": {
                "if": "bridge0"
            }
        },
        {
            "name": "Delete interface bridge",
            "uri": "/api/v1/interface/bridge",
            "req_data": {
                "id": "bridge0"
            }
        },
        {
            "name": "Delete interface VLAN",
            "uri": "/api/v1/interface/vlan",
            "req_data": {
                "vlanif": "em2.2"
            }
        },
        {
            "name": "Delete test firewall alias",
            "uri": "/api/v1/firewall/alias",
            "req_data": {
                "id": "TEST_ALIAS"
            }
        }
    ]


APIE2ETestInterface()
