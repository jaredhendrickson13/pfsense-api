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
"""Script used to test the /api/v1/firewall/virtual_ip endpoint."""
import e2e_test_framework

# Constants
CARP_VHID_CREATE = 10
CARP_VHID_UPDATE = 20
CARP_SUBNET_CREATE = "172.16.209.100/32"
CARP_SUBNET_UPDATE = "172.16.209.200/32"
IPALIAS_SUBNET_CREATE = "172.16.209.101/32"
IPALIAS_SUBNET_UPDATE = "172.16.209.201/32"
PROXYARP_SUBNET_CREATE = "172.16.209.102/32"
PROXYARP_SUBNET_UPDATE = "172.16.209.202/32"


class APIE2ETestFirewallVirtualIP(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/virtual_ip endpoint."""
    uri = "/api/v1/firewall/virtual_ip"
    get_tests = [{"name": "Read all virtual IPs"}]
    post_tests = [
        {
            "name": "Create CARP virtual IP",
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": CARP_SUBNET_CREATE,
                "password": "testpass",
                "descr": "E2E Test",
                "vhid": CARP_VHID_CREATE
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check ifconfig for CARP virtual IP",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "check_carp_exists"
        },
        {
            "name": "Create Proxy ARP virtual IP",
            "req_data": {
                "mode": "proxyarp",
                "interface": "lan",
                "subnet": PROXYARP_SUBNET_CREATE,
                "descr": "E2E Test"
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check for choparp process running proxyarp virtual IP",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "/usr/bin/top -baHS 999 | grep choparp"},
            "post_test_callable": "check_proxyarp_exists"
        },
        {
            "name": "Create IP Alias virtual IP",
            "req_data": {
                "mode": "ipalias",
                "interface": "lan",
                "subnet": IPALIAS_SUBNET_CREATE,
                "descr": "E2E Test"
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check ifconfig for IP alias virtual IP",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "check_ip_alias_exists"
        },
        {
            "name": "Check mode requirement",
            "status": 400,
            "return": 4019
        },
        {
            "name": "Check mode validation",
            "status": 400,
            "return": 4023,
            "req_data": {
                "mode": "INVALID"
            }
        },
        {
            "name": "Check interface requirement",
            "status": 400,
            "return": 4020,
            "req_data": {
                "mode": "ipalias"
            }
        },
        {
            "name": "Check interface validation",
            "status": 400,
            "return": 4024,
            "req_data": {
                "mode": "ipalias",
                "interface": "INVALID"
            }
        },
        {
            "name": "Check subnet requirement",
            "status": 400,
            "return": 4021,
            "req_data": {
                "mode": "ipalias",
                "interface": "lan"
            }
        },
        {
            "name": "Check subnet validation",
            "status": 400,
            "return": 4025,
            "req_data": {
                "mode": "ipalias",
                "interface": "lan",
                "subnet": "INVALID"
            }
        },
        {
            "name": "Check subnet unique constraint",
            "status": 400,
            "return": 4026,
            "req_data": {
                "mode": "ipalias",
                "interface": "lan",
                "subnet": IPALIAS_SUBNET_CREATE
            }
        },
        {
            "name": "Check CARP VHID minimum constraint",
            "status": 400,
            "return": 4028,
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "vhid": -1
            }
        },
        {
            "name": "Check CARP VHID maximum constraint",
            "status": 400,
            "return": 4028,
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "vhid": 4000000
            }
        },
        {
            "name": "Check CARP VHID unique constraint duplicate VHID on same interface",
            "status": 400,
            "return": 4027,
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "vhid": CARP_VHID_CREATE
            }
        },
        {
            "name": "Check CARP password requirement & VHID unique constraint duplicate VHID on different interface",
            "status": 400,
            "return": 4022,
            "req_data": {
                "mode": "carp",
                "interface": "wan",
                "subnet": "172.16.77.23/32",
                "vhid": 10
            }
        },
        {
            "name": "Check CARP advertisement skew minimum constraint",
            "status": 400,
            "return": 4030,
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advskew": -1
            }
        },
        {
            "name": "Check CARP advertisement skew maximum constraint",
            "status": 400,
            "return": 4030,
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advskew": 4030
            }
        },
        {
            "name": "Check CARP advertisement base minimum constraint",
            "status": 400,
            "return": 4029,
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advbase": 0
            }
        },
        {
            "name": "Check CARP advertisement base maximum constraint",
            "status": 400,
            "return": 4029,
            "req_data": {
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advbase": 4030
            }
        },
    ]
    put_tests = [
        {
            "name": "Update CARP virtual IP with static VHID",
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": "lan",
                "subnet": CARP_SUBNET_UPDATE,
                "password": "newtestpass",
                "vhid": CARP_VHID_UPDATE,
                "descr": "Updated E2E test",
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check ifconfig for updated CARP virtual IP",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "check_carp_exists"
        },
        {
            "name": "Update Proxy ARP virtual IP",
            "req_data": {
                "id": 1,
                "mode": "proxyarp",
                "interface": "lan",
                "subnet": PROXYARP_SUBNET_UPDATE,
                "descr": "Updated E2E test",
            },
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check for choparp process running updated proxyarp virtual IP",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "/usr/bin/top -baHS 999 | grep choparp"},
            "post_test_callable": "check_proxyarp_exists"
        },
        {
            "name": "Update IP Alias virtual IP",
            "req_data": {
                "id": 2,
                "mode": "ipalias",
                "interface": "lan",
                "subnet": IPALIAS_SUBNET_UPDATE,
                "descr": "Updated E2E test",
            },
            "resp_time": 10  # Allow up to ten seconds for vips
        },
        {
            "name": "Check ifconfig for updated IP alias virtual IP",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "check_ip_alias_exists"
        },
        {
            "name": "Check subnet validation",
            "status": 400,
            "return": 4025,
            "req_data": {
                "id": 2,
                "mode": "ipalias",
                "interface": "lan",
                "subnet": "INVALID"
            }
        },
        {
            "name": "Check subnet unique constraint",
            "status": 400,
            "return": 4026,
            "req_data": {
                "id": 2,
                "mode": "ipalias",
                "interface": "lan",
                "subnet": IPALIAS_SUBNET_UPDATE,
            }
        },
        {
            "name": "Check CARP VHID minimum constraint",
            "status": 400,
            "return": 4028,
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "vhid": -1
            }
        },
        {
            "name": "Check CARP VHID maximum constraint",
            "status": 400,
            "return": 4028,
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "vhid": 4000000
            }
        },
        {
            "name": "Check CARP advertisement skew minimum constraint",
            "status": 400,
            "return": 4030,
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advskew": -1
            }
        },
        {
            "name": "Check CARP advertisement skew maximum constraint",
            "status": 400,
            "return": 4030,
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advskew": 4030
            }
        },
        {
            "name": "Check CARP advertisement base minimum constraint",
            "status": 400,
            "return": 4029,
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advbase": 0
            }
        },
        {
            "name": "Check CARP advertisement base maximum constraint",
            "status": 400,
            "return": 4029,
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.252/32",
                "advbase": 4030
            }
        },
        {
            "name": "Check CARP password requirement when updating from non-CARP virtual IP",
            "status": 400,
            "return": 4022,
            "req_data": {
                "id": 2,
                "mode": "carp",
                "interface": "lan",
                "subnet": "172.16.77.234/32",
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete CARP virtual IP",
            "req_data": {"id": 0},
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check ifconfig for deleted CARP virtual IP",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "check_carp_does_not_exist"
        },
        {
            "name": "Delete Proxy ARP virtual IP",
            "req_data": {"id": 0},
            "resp_time": 10     # Allow up to ten seconds for vips
        },
        {
            "name": "Check for choparp process running deleted proxyarp virtual IP",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "/usr/bin/top -baHS 999 | grep choparp"},
            "post_test_callable": "check_proxyarp_does_not_exist"
        },
        {
            "name": "Delete IP Alias virtual IP",
            "req_data": {"id": 0},
            "resp_time": 10  # Allow up to ten seconds for vips
        },
        {
            "name": "Check ifconfig for deleted IP alias virtual IP",
            "method": "POST",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "ifconfig"},
            "post_test_callable": "check_ip_alias_does_not_exist"
        },
    ]

    def check_carp_exists(self):
        """Checks if our CARP is present after creating/updating an CARP VIP"""
        # Local variables
        carp_ip = self.last_request.get("req_data").get("subnet").split("/")[0]
        carp_vhid = self.last_request.get("req_data").get("vhid")

        if f"{carp_ip} vhid {carp_vhid}" not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected IP alias VIP '{carp_ip}' with VHID {carp_vhid} to be present")

    def check_ip_alias_exists(self):
        """Checks if our IP alias is present after creating/updating an IP alias VIP"""
        # Local varaible
        alias_ip = self.last_request.get("req_data").get("subnet").split("/")[0]

        if alias_ip not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected IP alias VIP '{alias_ip}' to be present")

    def check_proxyarp_exists(self):
        """Checks if our proxyarp IP is present after creating/updating a proxyarp VIP"""
        # Local variables
        proxyarp_ip = self.last_request.get("req_data").get("subnet")

        if f"{proxyarp_ip}" not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected proxyarp VIP '{proxyarp_ip}' to be present")

    def check_carp_does_not_exist(self):
        """Checks if our CARP is no longer present after deleting an CARP VIP"""
        # Local variables
        carp_ip = CARP_SUBNET_UPDATE.split("/")[0]
        carp_vhid = CARP_VHID_UPDATE

        if f"{carp_ip} vhid {carp_vhid}" in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected IP alias VIP '{carp_ip}' with VHID {carp_vhid} to be deleted")

    def check_ip_alias_does_not_exist(self):
        """Checks if our IP alias is no longer present after deleting an IP alias VIP"""
        # Local varaible
        alias_ip = IPALIAS_SUBNET_UPDATE.split("/")[0]

        if alias_ip in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected IP alias VIP '{alias_ip}' to be deleted")

    def check_proxyarp_does_not_exist(self):
        """Checks if our proxyarp IP is no longer present after deleting a proxyarp VIP"""
        # Local variables
        proxyarp_ip = PROXYARP_SUBNET_UPDATE

        if f"{proxyarp_ip}" in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError(f"Expected proxyarp VIP '{proxyarp_ip}' to be deleted")


APIE2ETestFirewallVirtualIP()
