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
import random

import e2e_test_framework
from e2e_test_framework.tools import is_if_in_ifconfig

# Constants
IF_WAN_ID = "em0"
IF_ID = "em2"
BR_MEMBER_ID = "em1"
VLAN_TAG = random.randint(2, 4094)
VLAN_IF = f"{IF_ID}.{VLAN_TAG}"
IF_STATICV4_IPADDR_CREATE = "172.16.100.1"
IF_STATICV4_SUBNET_CREATE = random.randint(24, 30)
IF_STATICV6_IPADDR_CREATE = "2001:db8:abcd:12::1"
IF_STATICV6_SUBNET_CREATE = random.randint(64, 128)
BR_STATICV4_IPADDR_CREATE = "172.16.200.1"
BR_STATICV4_SUBNET_CREATE = random.randint(24, 30)
VLAN_STATICV4_IPADDR_CREATE = "172.16.2.1"
VLAN_STATICV4_SUBNET_CREATE = random.randint(24, 30)
IF_STATICV4_IPADDR_UPDATE = "172.16.101.1"
IF_STATICV4_SUBNET_UPDATE = random.randint(24, 30)
IF_STATICV6_IPADDR_UPDATE = "2002:db8:abcd:12::1"
IF_STATICV6_SUBNET_UPDATE = random.randint(64, 128)
BR_STATICV4_IPADDR_UPDATE = "172.16.201.1"
BR_STATICV4_SUBNET_UPDATE = random.randint(24, 30)
VLAN_STATICV4_IPADDR_UPDATE = "172.16.20.1"
VLAN_STATICV4_SUBNET_UPDATE = random.randint(24, 30)


class APIE2ETestInterface(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/interface endpoint."""
    uri = "/api/v1/interface"

    get_privileges = ["page-all", "page-interfaces-assignnetworkports"]
    post_privileges = ["page-all", "page-interfaces-assignnetworkports"]
    put_privileges = ["page-all", "page-interfaces-assignnetworkports"]
    delete_privileges = ["page-all", "page-interfaces-assignnetworkports"]

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
                "if": IF_WAN_ID
            }
        },
        {
            "name": "Check IPv4 type option constraint",
            "status": 400,
            "return": 3025,
            "req_data": {
                "if": IF_ID,
                "type": "INVALID"
            }
        },
        {
            "name": "Check IPv4 type option constraint",
            "status": 400,
            "return": 3041,
            "req_data": {
                "if": IF_ID,
                "type6": "INVALID"
            }
        },
        {
            "name": "Check spoof MAC address validation",
            "status": 400,
            "return": 3003,
            "req_data": {
                "if": IF_ID,
                "spoofmac": "INVALID"
            }
        },
        {
            "name": "Check MTU minimum constraint",
            "status": 400,
            "return": 3004,
            "req_data": {
                "if": IF_ID,
                "mtu": 1279
            }
        },
        {
            "name": "Check MTU maximum constraint",
            "status": 400,
            "return": 3004,
            "req_data": {
                "if": IF_ID,
                "mtu": 8193
            }
        },
        {
            "name": "Check MSS minimum constraint",
            "status": 400,
            "return": 3005,
            "req_data": {
                "if": IF_ID,
                "mss": 575
            }
        },
        {
            "name": "Check MSS maximum constraint",
            "status": 400,
            "return": 3005,
            "req_data": {
                "if": IF_ID,
                "mss": 65536
            }
        },
        {
            "name": "Check media choices constraint",
            "status": 400,
            "return": 3007,
            "req_data": {
                "if": IF_ID,
                "media": "INVALID"
            }
        },
        {
            "name": "Check description cannot start with pkg_",
            "status": 400,
            "return": 3059,
            "req_data": {
                "if": IF_ID,
                "descr": "pkg_INVALID"
            }
        },
        {
            "name": "Check description cannot match existing alias name",
            "status": 400,
            "return": 3060,
            "req_data": {
                "if": IF_ID,
                "descr": "TEST_ALIAS"
            }
        },
        {
            "name": "Check description cannot be numeric",
            "status": 400,
            "return": 3061,
            "req_data": {
                "if": IF_ID,
                "descr": "10051"
            }
        },
        {
            "name": "Check description cannot be in use",
            "status": 400,
            "return": 3008,
            "req_data": {
                "if": IF_ID,
                "descr": "WAN"
            }
        },
        {
            "name": "Check static IPv4 address requirement",
            "status": 400,
            "return": 3011,
            "req_data": {
                "if": IF_ID,
                "descr": "TEST",
                "type": "staticv4"
            }
        },
        {
            "name": "Check static IPv4 address validation",
            "status": 400,
            "return": 3010,
            "req_data": {
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
                "descr": "TEST",
                "type6": "staticv6"
            }
        },
        {
            "name": "Check static IPv6 address validation",
            "status": 400,
            "return": 3026,
            "req_data": {
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
                "descr": "TEST",
                "type6": "6rd"
            }
        },
        {
            "name": "Check 6RD IPv6 gateway validation",
            "status": 400,
            "return": 3035,
            "req_data": {
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
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
                "if": IF_ID,
                "descr": "TEST",
                "type6": "track6"
            }
        },
        {
            "name": "Check Track6 IPv6 interface exists constraint",
            "status": 400,
            "return": 3038,
            "req_data": {
                "if": IF_ID,
                "descr": "TEST",
                "type6": "track6",
                "track6-interface": "INVALID"
            }
        },
        {
            "name": "Create interface bridge for testing",
            "uri": "/api/v1/interface/bridge",
            "req_data": {
                "members": [BR_MEMBER_ID]
            }
        },
        {
            "name": "Create interface VLAN for testing",
            "uri": "/api/v1/interface/vlan",
            "resp_time": 5,
            "req_data": {
                "if": IF_ID,
                "tag": VLAN_TAG
            }
        },
        {
            "name": "Check MTU less than VLAN parent interface constraint",
            "status": 400,
            "return": 3006,
            "req_data": {
                "if": VLAN_IF,
                "mtu": 8192
            }
        },
        {
            "name": "Create a staticv4/staticv6 interface",
            "req_data": {
                "if": IF_ID,
                "descr": "STATIC_TEST",
                "enable": True,
                "type": "staticv4",
                "type6": "staticv6",
                "ipaddr": IF_STATICV4_IPADDR_CREATE,
                "ipaddrv6": IF_STATICV6_IPADDR_CREATE,
                "subnet": IF_STATICV4_SUBNET_CREATE,
                "subnetv6": IF_STATICV6_SUBNET_CREATE,
                "blockbogons": True
            }
        },
        {
            "name": "Create a static interface on the bridge",
            "req_data": {
                "if": "bridge0",
                "descr": "BR_TEST",
                "enable": True,
                "type": "staticv4",
                "ipaddr": BR_STATICV4_IPADDR_CREATE,
                "subnet": BR_STATICV4_SUBNET_CREATE,
                "blockbogons": True
            }
        },
        {
            "name": "Create a static interface on a VLAN",
            "req_data": {
                "if": VLAN_IF,
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
            "resp_data_empty": True,
            "post_test_callable": "are_ifs_created"
        }
    ]
    put_tests = [
        {
            "name": "Disable interface",
            "resp_time": 5,
            "post_test_callable": "is_if_disabled",
            "req_data": {
                "id": VLAN_IF,
                "descr": "IF_DISABLED_TEST",
                "enable": False,
                "type": "dhcp",
                "blockbogons": False,
                "apply": True
            },
        },
        {
            "name": "Re-enable and update IP of VLAN interface",
            "req_data": {
                "id": VLAN_IF,
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
                "id": IF_ID,
                "type": "staticv4",
                "type6": "staticv6",
                "ipaddr": IF_STATICV4_IPADDR_UPDATE,
                "ipaddrv6": IF_STATICV6_IPADDR_UPDATE,
                "subnet": IF_STATICV4_SUBNET_UPDATE,
                "subnetv6": IF_STATICV6_SUBNET_UPDATE,
                "apply": False
            },
        },
        {
            "name": "Update IP of bridge interface",
            "req_data": {
                "id": "bridge0",
                "type": "staticv4",
                "ipaddr": BR_STATICV4_IPADDR_UPDATE,
                "subnet": BR_STATICV4_SUBNET_UPDATE,
                "apply": False
            },
        },
        {
            "name": "Apply interfaces",
            "method": "POST",
            "uri": "/api/v1/interface/apply",
            "req_data": {"aysnc": False},
            "resp_time": 30,
            "resp_data_empty": True,
            "post_test_callable": "are_ifs_updated"
        }
    ]
    delete_tests = [
        {
            "name": "Delete interface",
            "resp_time": 3,
            "req_data": {
                "if": IF_ID
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
                "if": VLAN_IF
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
                "vlanif": VLAN_IF
            }
        },
        {
            "name": "Delete test firewall alias",
            "uri": "/api/v1/firewall/alias",
            "post_test_callable": "are_ifs_deleted",
            "req_data": {
                "id": "TEST_ALIAS"
            }
        }
    ]

    def are_ifs_created(self):
        """Checks if the interfaces created in the POST tests are present after being applied."""
        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")

        # Ensure static interface exists with IPv4 address
        if not is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV4_IPADDR_CREATE, IF_STATICV4_SUBNET_CREATE):
            raise AssertionError(f"Expected interface with static IPv4 '{IF_STATICV4_IPADDR_CREATE}'")

        # Ensure static interface exists with IPv6 address
        if not is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV6_IPADDR_CREATE, IF_STATICV6_SUBNET_CREATE):
            raise AssertionError(f"Expected interface with static IPv6 '{IF_STATICV6_IPADDR_CREATE}'")

        # Ensure bridged interface exists with IPv4 address
        if not is_if_in_ifconfig(ifconfig_out, "bridge0", BR_STATICV4_IPADDR_CREATE, BR_STATICV4_SUBNET_CREATE):
            raise AssertionError(f"Expected bridge interface with static IPv4 '{BR_STATICV4_IPADDR_CREATE}'")

        # Ensure VLAN interface exists with IPv4 address
        if not is_if_in_ifconfig(ifconfig_out, VLAN_IF, VLAN_STATICV4_IPADDR_CREATE, VLAN_STATICV4_SUBNET_CREATE):
            raise AssertionError(f"Expected VLAN interface with static IPv4 '{VLAN_STATICV4_IPADDR_CREATE}'")

    def are_ifs_updated(self):
        """
        Checks if the interfaces updated in the PUT tests are present after being applied and that the old IPs are
        no longer used.
        """
        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")

        # Ensure static interface exists with updated IPv4 address
        if not is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV4_IPADDR_UPDATE, IF_STATICV4_SUBNET_UPDATE):
            raise AssertionError(f"Expected interface with static IPv4 '{IF_STATICV4_IPADDR_UPDATE}'")

        # Ensure old IP is no longer present
        if is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV4_IPADDR_CREATE, IF_STATICV4_SUBNET_CREATE):
            raise AssertionError(f"Interface is still using old static IPv4 '{IF_STATICV4_IPADDR_CREATE}'")

        # Ensure static interface exists with updated IPv6 address
        if not is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV6_IPADDR_UPDATE, IF_STATICV6_SUBNET_UPDATE):
            raise AssertionError(f"Expected interface with static IPv6 '{IF_STATICV6_IPADDR_UPDATE}'")

        # Ensure old IPv6 is no longer present
        if is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV6_IPADDR_CREATE, IF_STATICV6_SUBNET_CREATE):
            raise AssertionError(f"Interface is still using old static IPv6 '{IF_STATICV6_IPADDR_CREATE}'")

        # Ensure bridged interface exists with IPv4 address
        if not is_if_in_ifconfig(ifconfig_out, "bridge0", BR_STATICV4_IPADDR_UPDATE, BR_STATICV4_SUBNET_UPDATE):
            raise AssertionError(f"Expected bridge interface with static IPv4 '{BR_STATICV4_IPADDR_UPDATE}'")

        # Ensure old bridge IP is no longer present
        if is_if_in_ifconfig(ifconfig_out, "bridge0", BR_STATICV4_IPADDR_CREATE, BR_STATICV4_SUBNET_CREATE):
            raise AssertionError(f"Bridge is still using old static IPv4 '{BR_STATICV4_IPADDR_CREATE}'")

        # Ensure VLAN interface exists with IPv4 address
        if not is_if_in_ifconfig(ifconfig_out, VLAN_IF, VLAN_STATICV4_IPADDR_UPDATE, VLAN_STATICV4_SUBNET_UPDATE):
            raise AssertionError(f"Expected VLAN interface with static IPv4 '{VLAN_STATICV4_IPADDR_UPDATE}'")

        # Ensure old VLAN IP is no longer present
        if is_if_in_ifconfig(ifconfig_out, VLAN_IF, VLAN_STATICV4_IPADDR_CREATE, VLAN_STATICV4_SUBNET_CREATE):
            raise AssertionError(f"VLAN is still using old static IPv4 '{VLAN_STATICV4_IPADDR_CREATE}'")

    def are_ifs_deleted(self):
        """Checks if the interfaces deleted in the DELETE tests are no longer present"""
        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")

        # Ensure staticv4 interface no longer exists
        if is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV4_IPADDR_UPDATE, IF_STATICV4_SUBNET_UPDATE):
            raise AssertionError(f"Expected interface with IP '{IF_STATICV4_IPADDR_UPDATE}' to be deleted")

        # Ensure staticv6 interface no longer exists
        if is_if_in_ifconfig(ifconfig_out, IF_ID, IF_STATICV6_IPADDR_UPDATE, IF_STATICV6_SUBNET_UPDATE):
            raise AssertionError(f"Expected interface with IP '{IF_STATICV6_IPADDR_UPDATE}' to be deleted")

        # Ensure bridge interface no longer exists
        if is_if_in_ifconfig(ifconfig_out, "bridge0", BR_STATICV4_IPADDR_UPDATE, BR_STATICV4_SUBNET_UPDATE):
            raise AssertionError(f"Expected bridge with IP '{BR_STATICV4_IPADDR_UPDATE}' to be deleted")

        # Ensure vlan interface no longer exists
        if is_if_in_ifconfig(ifconfig_out, VLAN_IF, VLAN_STATICV4_IPADDR_UPDATE, VLAN_STATICV4_SUBNET_UPDATE):
            raise AssertionError(f"Expected VLAN IP '{VLAN_STATICV4_IPADDR_UPDATE}' to be deleted")

    def is_if_disabled(self):
        """Checks if the interface updated to be disabled is no longer up."""
        # Local variables
        ifconfig_lines = self.pfsense_shell("ifconfig").split("\n")

        # Loop through each line and check if em2.2 is now disabled
        for line in ifconfig_lines:
            if line.startswith(f"{VLAN_IF}:") and "UP" in line:
                raise AssertionError(f"Expected {VLAN_IF} to be disabled and not UP")


APIE2ETestInterface()
