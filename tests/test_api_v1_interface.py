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


# Constants
INTERFACE_STATICV4_IPADDR_CREATE = "172.16.100.1"
INTERFACE_STATICV4_SUBNET_CREATE = 24
INTERFACE_STATICV6_IPADDR_CREATE = "2001:db8:abcd:12::1"
INTERFACE_STATICV6_SUBNET_CREATE = 64
BRIDGE_STATICV4_IPADDR_CREATE = "172.16.200.1"
BRIDGE_STATICV4_SUBNET_CREATE = 24
VLAN_STATICV4_IPADDR_CREATE = "172.16.2.1"
VLAN_STATICV4_SUBNET_CREATE = 24
INTERFACE_STATICV4_IPADDR_UPDATE = "172.16.101.1"
INTERFACE_STATICV4_SUBNET_UPDATE = 32
INTERFACE_STATICV6_IPADDR_UPDATE = "2002:db8:abcd:12::1"
INTERFACE_STATICV6_SUBNET_UPDATE = 70
BRIDGE_STATICV4_IPADDR_UPDATE = "172.16.201.1"
BRIDGE_STATICV4_SUBNET_UPDATE = 32
VLAN_STATICV4_IPADDR_UPDATE = "172.16.20.1"
VLAN_STATICV4_SUBNET_UPDATE = 32


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
            "name": "Check MTU less than VLAN parent interface constraint",
            "status": 400,
            "return": 3006,
            "req_data": {
                "if": "em2.2",
                "mtu": 8192
            }
        },
        {
            "name": "Create a staticv4/staticv6 interface",
            "req_data": {
                "if": "em2",
                "descr": "STATIC_TEST",
                "enable": True,
                "type": "staticv4",
                "type6": "staticv6",
                "ipaddr": INTERFACE_STATICV4_IPADDR_CREATE,
                "ipaddrv6": INTERFACE_STATICV6_IPADDR_CREATE,
                "subnet": INTERFACE_STATICV4_SUBNET_CREATE,
                "subnetv6": INTERFACE_STATICV6_SUBNET_CREATE,
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
                "ipaddr": BRIDGE_STATICV4_IPADDR_CREATE,
                "subnet": BRIDGE_STATICV4_SUBNET_CREATE,
                "blockbogons": True
            }
        },
        {
            "name": "Create a static interface on a VLAN",
            "req_data": {
                "if": "em2.2",
                "descr": "VLAN_TEST",
                "enable": True,
                "type": "staticv4",
                "ipaddr": VLAN_STATICV4_IPADDR_CREATE,
                "subnet": VLAN_STATICV4_SUBNET_CREATE,
                "blockbogons": True
            }
        },
        {
            "name": "Apply interfaces",
            "method": "POST",
            "uri": "/api/v1/interface/apply",
            "req_data": {"aysnc": False},
            "resp_time": 30,
            "resp_data_empty": True
        },
        {
            "name": "Check that the created interfaces are now present",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "are_ifs_created"
        }
    ]
    put_tests = [
        {
            "name": "Disable interface",
            "resp_time": 5,
            "req_data": {
                "id": "em2.2",
                "descr": "IF_DISABLED_TEST",
                "enable": False,
                "type": "dhcp",
                "blockbogons": False,
                "apply": True
            },
        },
        {
            "name": "Check that the disabled interfaces is no longer up",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "is_if_disabled"
        },
        {
            "name": "Re-enable and update IP of VLAN interface",
            "req_data": {
                "id": "em2.2",
                "enable": True,
                "type": "staticv4",
                "ipaddr": VLAN_STATICV4_IPADDR_UPDATE,
                "subnet": VLAN_STATICV4_SUBNET_UPDATE,
                "apply": False
            },
        },
        {
            "name": "Update IP of static interface",
            "req_data": {
                "id": "em2",
                "type": "staticv4",
                "type6": "staticv6",
                "ipaddr": INTERFACE_STATICV4_IPADDR_UPDATE,
                "ipaddrv6": INTERFACE_STATICV6_IPADDR_UPDATE,
                "subnet": INTERFACE_STATICV4_SUBNET_UPDATE,
                "subnetv6": INTERFACE_STATICV6_SUBNET_UPDATE,
                "apply": False
            },
        },
        {
            "name": "Update IP of bridge interface",
            "req_data": {
                "id": "bridge0",
                "type": "staticv4",
                "ipaddr": BRIDGE_STATICV4_IPADDR_UPDATE,
                "subnet": BRIDGE_STATICV4_SUBNET_UPDATE,
                "apply": False
            },
        },
        {
            "name": "Apply interfaces",
            "method": "POST",
            "uri": "/api/v1/interface/apply",
            "req_data": {"aysnc": False},
            "resp_time": 30,
            "resp_data_empty": True
        },
        {
            "name": "Check that the updated interface IPs are now present and old IPs are not",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "are_ifs_updated"
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
            "name": "Delete VLAN interface",
            "req_data": {
                "if": "em2.2"
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
        },
        {
            "name": "Check that deleted interfaces no longer exist",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "are_ifs_deleted"
        }
    ]

    def are_ifs_created(self):
        """Checks if the interfaces created in the POST tests are present after being applied."""
        # Local variables
        staticv4_line = f"inet {INTERFACE_STATICV4_IPADDR_CREATE} netmask 0xffffff00"
        staticv6_line = f"inet6 {INTERFACE_STATICV6_IPADDR_CREATE} prefixlen {INTERFACE_STATICV6_SUBNET_CREATE}"
        bridge_line = f"inet {BRIDGE_STATICV4_IPADDR_CREATE} netmask 0xffffff00"
        vlan_line = f"inet {VLAN_STATICV4_IPADDR_CREATE} netmask 0xffffff00"

        # Ensure static interface exists with IPv4 address
        if staticv4_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected interface with static IPv4 '{INTERFACE_STATICV4_IPADDR_CREATE}'")

        # Ensure static interface exists with IPv6 address
        if staticv6_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected interface with static IPv6 '{INTERFACE_STATICV6_IPADDR_CREATE}'")

        # Ensure bridged interface exists with IPv4 address
        if bridge_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected bridge interface with static IPv4 '{BRIDGE_STATICV4_IPADDR_CREATE}'")

        # Ensure VLAN interface exists with IPv4 address
        if vlan_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected VLAN interface with static IPv4 '{VLAN_STATICV4_IPADDR_CREATE}'")

    def are_ifs_updated(self):
        """
        Checks if the interfaces updated in the PUT tests are present after being applied and that the old IPs are
        no longer used.
        """
        # Local variables
        staticv4_line = f"inet {INTERFACE_STATICV4_IPADDR_UPDATE} netmask 0xffffffff"
        staticv6_line = f"inet6 {INTERFACE_STATICV6_IPADDR_UPDATE} prefixlen {INTERFACE_STATICV6_SUBNET_UPDATE}"
        bridge_line = f"inet {BRIDGE_STATICV4_IPADDR_UPDATE} netmask 0xffffffff"
        vlan_line = f"inet {VLAN_STATICV4_IPADDR_UPDATE} netmask 0xffffffff"
        staticv4_old_line = f"inet {INTERFACE_STATICV4_IPADDR_CREATE} netmask 0xffffff00"
        staticv6_old_line = f"inet6 {INTERFACE_STATICV6_IPADDR_CREATE} prefixlen {INTERFACE_STATICV6_SUBNET_CREATE}"
        bridge_old_line = f"inet {BRIDGE_STATICV4_IPADDR_CREATE} netmask 0xffffff00"
        vlan_old_line = f"inet {VLAN_STATICV4_IPADDR_CREATE} netmask 0xffffff00"

        # Ensure static interface exists with updated IPv4 address
        if staticv4_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected interface with static IPv4 '{INTERFACE_STATICV4_IPADDR_UPDATE}'")

        # Ensure old IP is no longer present
        if staticv4_old_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Interface is still using old static IPv4 '{INTERFACE_STATICV4_IPADDR_CREATE}'")

        # Ensure static interface exists with updated IPv6 address
        if staticv6_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected interface with static IPv6 '{INTERFACE_STATICV6_IPADDR_UPDATE}'")

        # Ensure old IPv6 is no longer present
        if staticv6_old_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Interface is still using old static IPv6 '{INTERFACE_STATICV6_IPADDR_CREATE}'")

        # Ensure bridged interface exists with IPv4 address
        if bridge_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected bridge interface with static IPv4 '{BRIDGE_STATICV4_IPADDR_UPDATE}'")

        # Ensure old bridge IP is no longer present
        if bridge_old_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Bridge is still using old static IPv4 '{BRIDGE_STATICV4_IPADDR_CREATE}'")

        # Ensure VLAN interface exists with IPv4 address
        if vlan_line not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected VLAN interface with static IPv4 '{VLAN_STATICV4_IPADDR_UPDATE}'")

        # Ensure old VLAN IP is no longer present
        if vlan_old_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"VLAN is still using old static IPv4 '{VLAN_STATICV4_IPADDR_CREATE}'")

    def are_ifs_deleted(self):
        """Checks if the interfaces deleted in the DELETE tests are no longer present"""
        # Local variables
        staticv4_line = f"inet {INTERFACE_STATICV4_IPADDR_UPDATE} netmask 0xffffffff"
        staticv6_line = f"inet6 {INTERFACE_STATICV6_IPADDR_UPDATE} prefixlen {INTERFACE_STATICV6_SUBNET_UPDATE}"
        bridge_line = f"inet {BRIDGE_STATICV4_IPADDR_UPDATE} netmask 0xffffffff"
        vlan_line = f"inet {VLAN_STATICV4_IPADDR_UPDATE} netmask 0xffffffff"

        # Ensure staticv4 interface no longer exists
        if staticv4_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected interface with IP '{INTERFACE_STATICV4_IPADDR_UPDATE}' to be deleted")

        # Ensure staticv6 interface no longer exists
        if staticv6_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected interface with IP '{INTERFACE_STATICV6_IPADDR_UPDATE}' to be deleted")

        # Ensure bridge interface no longer exists
        if bridge_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected bridge with IP '{BRIDGE_STATICV4_IPADDR_UPDATE}' to be deleted")

        # Ensure vlan interface no longer exists
        if vlan_line in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected VLAN IP '{VLAN_STATICV4_IPADDR_UPDATE}' to be deleted")

    def is_if_disabled(self):
        """Checks if the interface updated to be disabled is no longer up."""
        # Local variables
        ifconfig_lines = self.last_response.get("data", {}).get("cmd_output", "").split("\n")

        # Loop through each line and check if em2.2 is now disabled
        for line in ifconfig_lines:
            if line.startswith("em2.2:") and "UP" in line:
                raise AssertionError("Expected em2.2 to be disabled and not UP")


APIE2ETestInterface()
