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
import random

import e2e_test_framework
from e2e_test_framework.tools import is_if_in_ifconfig

# Constants
EXTRA_INTERFACE = "em2"
TEST_INTERFACE_CHOICES = ["em1", "em2"]
CARP_INTERFACE_CREATE = random.choice(TEST_INTERFACE_CHOICES)
CARP_INTERFACE_UPDATE = random.choice(TEST_INTERFACE_CHOICES)
CARP_VHID_CREATE = random.randint(0, 128)
CARP_VHID_UPDATE = random.randint(129, 255)
CARP_ADVBASE_CREATE = random.randint(1, 254)
CARP_ADVBASE_UPDATE = random.randint(1, 254)
CARP_ADVSKEW_CREATE = random.randint(1, 254)
CARP_ADVSKEW_UPDATE = random.randint(1, 254)
CARP_SUBNET_CREATE = f"172.16.5.100/{random.randint(24, 29)}"
CARP_SUBNET_UPDATE = f"172.16.6.100/{random.randint(24, 29)}"
IPALIAS_INTERFACE_CREATE = random.choice(TEST_INTERFACE_CHOICES)
IPALIAS_INTERFACE_UPDATE = random.choice(TEST_INTERFACE_CHOICES)
IPALIAS_SUBNET_CREATE = f"172.16.7.100/{random.randint(24, 29)}"
IPALIAS_SUBNET_UPDATE = f"172.16.8.100/{random.randint(24, 29)}"
PROXYARP_INTERFACE_CREATE = random.choice(TEST_INTERFACE_CHOICES)
PROXYARP_INTERFACE_UPDATE = random.choice(TEST_INTERFACE_CHOICES)
PROXYARP_SUBNET_CREATE = "172.16.9.100/32"
PROXYARP_SUBNET_UPDATE = "172.16.10.100/32"


class APIE2ETestFirewallVirtualIP(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/virtual_ip endpoint."""
    uri = "/api/v1/firewall/virtual_ip"
    get_tests = [{"name": "Read all virtual IPs"}]
    post_tests = [
        {
            "name": "Create extra interface assignment for testing",
            "method": "POST",
            "uri": "/api/v1/interface",
            "resp_time": 10,
            "req_data": {
                "if": EXTRA_INTERFACE,
                "type": "staticv4",
                "ipaddr": "172.16.55.1",
                "subnet": 24,
                "enable": True,
                "apply": True
            }
        },
        {
            "name": "Create CARP virtual IP",
            "post_test_callable": "is_carp_created",
            "req_data": {
                "mode": "carp",
                "interface": CARP_INTERFACE_CREATE,
                "subnet": CARP_SUBNET_CREATE,
                "password": "testpass",
                "descr": "E2E Test",
                "vhid": CARP_VHID_CREATE,
                "advbase": CARP_ADVBASE_CREATE,
                "advskew": CARP_ADVSKEW_CREATE
            },
        },
        {
            "name": "Create Proxy ARP virtual IP",
            "post_test_callable": "is_proxyarp_created",
            "req_data": {
                "mode": "proxyarp",
                "interface": PROXYARP_INTERFACE_CREATE,
                "subnet": PROXYARP_SUBNET_CREATE,
                "descr": "E2E Test"
            }
        },
        {
            "name": "Create IP Alias virtual IP",
            "post_test_callable": "is_ipalias_created",
            "req_data": {
                "mode": "ipalias",
                "interface": IPALIAS_INTERFACE_CREATE,
                "subnet": IPALIAS_SUBNET_CREATE,
                "descr": "E2E Test"
            }
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
                "interface": CARP_INTERFACE_CREATE,
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
            "post_test_callable": "is_carp_updated",
            "req_data": {
                "id": 0,
                "mode": "carp",
                "interface": CARP_INTERFACE_UPDATE,
                "subnet": CARP_SUBNET_UPDATE,
                "password": "newtestpass",
                "vhid": CARP_VHID_UPDATE,
                "advbase": CARP_ADVBASE_UPDATE,
                "advskew": CARP_ADVSKEW_UPDATE,
                "descr": "Updated E2E test"
            }
        },
        {
            "name": "Update Proxy ARP virtual IP",
            "post_test_callable": "is_proxyarp_updated",
            "req_data": {
                "id": 1,
                "mode": "proxyarp",
                "interface": PROXYARP_INTERFACE_UPDATE,
                "subnet": PROXYARP_SUBNET_UPDATE,
                "descr": "Updated E2E test",
            }
        },
        {
            "name": "Update IP Alias virtual IP",
            "post_test_callable": "is_ipalias_updated",
            "req_data": {
                "id": 2,
                "mode": "ipalias",
                "interface": IPALIAS_INTERFACE_UPDATE,
                "subnet": IPALIAS_SUBNET_UPDATE,
                "descr": "Updated E2E test",
            }
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
            "post_test_callable": "is_carp_deleted",
            "req_data": {"id": 0}
        },
        {
            "name": "Delete Proxy ARP virtual IP",
            "post_test_callable": "is_proxyarp_deleted",
            "req_data": {"id": 0}
        },
        {
            "name": "Delete IP Alias virtual IP",
            "post_test_callable": "is_ipalias_deleted",
            "req_data": {"id": 0}
        },
        {
            "name": "Delete the extra interface we created for testing",
            "method": "DELETE",
            "uri": "/api/v1/interface",
            "req_data": {"if": EXTRA_INTERFACE}
        }
    ]

    def is_carp_created(self):
        """Checks if our CARP is present after creating a CARP VIP"""
        # Local variables
        carp_subnet = CARP_SUBNET_CREATE
        carp_ip = carp_subnet.split("/")[0]
        carp_bitmask = int(carp_subnet.split("/")[1])
        carp_interface = CARP_INTERFACE_CREATE
        carp_vhid = CARP_VHID_CREATE
        carp_advbase = CARP_ADVBASE_CREATE
        carp_advskew = CARP_ADVSKEW_CREATE
        ifconfig_carp_line = f"vhid {carp_vhid} advbase {carp_advbase} advskew {carp_advskew}"
        ifconfig_out = self.pfsense_shell("ifconfig")

        # Check that the CARP VIP created in the test is correctly represented in ifconfig
        if not is_if_in_ifconfig(ifconfig_out, carp_interface, carp_ip, carp_bitmask, carp_vhid):
            raise AssertionError(
                f"Expected CARP VIP '{carp_subnet}' with VHID {carp_vhid} to exist, got: {ifconfig_out}"
            )

        # Check that the CARP advbase and advskew are correctly represented in ifconfig
        if ifconfig_carp_line not in ifconfig_out:
            raise AssertionError(
                f"Expected '{ifconfig_carp_line}' in ifconfig for interface {carp_interface}, got {ifconfig_out}"
            )

    def is_carp_updated(self):
        """Checks if our CARP is present after creating a CARP VIP"""
        # Local variables
        carp_subnet = CARP_SUBNET_UPDATE
        carp_ip = carp_subnet.split("/")[0]
        carp_ip_prev = CARP_SUBNET_CREATE.split("/")[0]
        carp_bitmask = int(carp_subnet.split("/")[1])
        carp_interface = CARP_INTERFACE_UPDATE
        carp_vhid = CARP_VHID_UPDATE
        carp_advbase = CARP_ADVBASE_UPDATE
        carp_advskew = CARP_ADVSKEW_UPDATE
        ifconfig_carp_line = f"vhid {carp_vhid} advbase {carp_advbase} advskew {carp_advskew}"
        ifconfig_out = self.pfsense_shell("ifconfig")

        # Check that the CARP VIP created in the test is correctly represented in ifconfig
        if not is_if_in_ifconfig(ifconfig_out, carp_interface, carp_ip, carp_bitmask, carp_vhid):
            raise AssertionError(
                f"Expected CARP VIP '{carp_subnet}' with VHID {carp_vhid} to exist, got {ifconfig_out}"
            )

        # Check that the CARP advbase and advskew are correctly represented in ifconfig
        if ifconfig_carp_line not in ifconfig_out:
            raise AssertionError(
                f"Expected '{ifconfig_carp_line}' in ifconfig for interface {carp_interface}, got {ifconfig_out}"
            )

        # Check that the previous CARP IP is no longer present
        if carp_ip_prev in ifconfig_out:
            raise AssertionError(
                f"Expected previous CARP IP '{carp_ip_prev}' to no longer exist in ifconfig, got {ifconfig_out}"
            )

    def is_carp_deleted(self):
        """Checks if our CARP is no longer present after deleting an CARP VIP"""
        # Local variables
        carp_ip = CARP_SUBNET_UPDATE.split("/", maxsplit=1)[0]
        carp_vhid = CARP_VHID_UPDATE
        ifconfig_out = self.pfsense_shell("ifconfig")

        if carp_ip in ifconfig_out or f"vhid {carp_vhid}" in ifconfig_out:
            raise AssertionError(
                f"Expected CARP VIP '{carp_ip}' with VHID {carp_vhid} to be deleted in ifconfig, got {ifconfig_out}"
            )

    def is_ipalias_created(self):
        """Checks if our IPALIAS is present after creating an IPALIAS VIP"""
        # Local variables
        ipalias_subnet = IPALIAS_SUBNET_CREATE
        ipalias_ip = ipalias_subnet.split("/")[0]
        ipalias_bitmask = int(ipalias_subnet.split("/")[1])
        ipalias_interface = IPALIAS_INTERFACE_CREATE
        ifconfig_out = self.pfsense_shell("ifconfig")

        # Check that the IPALIAS VIP created in the test is correctly represented in ifconfig
        if not is_if_in_ifconfig(ifconfig_out, ipalias_interface, ipalias_ip, ipalias_bitmask):
            raise AssertionError(f"Expected IPALIAS VIP '{ipalias_subnet}' to exist, got {ifconfig_out}")

    def is_ipalias_updated(self):
        """Checks if our IPALIAS is present after creating an IPALIAS VIP"""
        # Local variables
        ipalias_subnet = IPALIAS_SUBNET_UPDATE
        ipalias_ip = ipalias_subnet.split("/")[0]
        ipalias_ip_prev = IPALIAS_SUBNET_CREATE.split("/")[0]
        ipalias_bitmask = int(ipalias_subnet.split("/")[1])
        ipalias_interface = IPALIAS_INTERFACE_UPDATE
        ifconfig_out = self.pfsense_shell("ifconfig")

        # Check that the IPALIAS VIP created in the test is correctly represented in ifconfig
        if not is_if_in_ifconfig(ifconfig_out, ipalias_interface, ipalias_ip, ipalias_bitmask):
            raise AssertionError(f"Expected IPALIAS VIP '{ipalias_subnet}' to exist, got {ifconfig_out}")

        # Check that the previous IPALIAS IP is no longer present
        if ipalias_ip_prev in ifconfig_out:
            raise AssertionError(
                f"Expected previous IPALIAS IP '{ipalias_ip_prev}' to no longer exist in ifconfig, got {ifconfig_out}"
            )

    def is_ipalias_deleted(self):
        """Checks if our IPALIAS is no longer present after deleting an IPALIAS VIP"""
        # Local variables
        ipalias_ip = IPALIAS_SUBNET_UPDATE.split("/", maxsplit=1)[0]
        ifconfig_out = self.pfsense_shell("ifconfig")

        if ipalias_ip in ifconfig_out:
            raise AssertionError(f"Expected IPALIAS VIP '{ipalias_ip}' to be deleted in ifconfig, got {ifconfig_out}")

    def is_proxyarp_created(self):
        """Checks if our proxyarp IP is present after creating a proxyarp VIP"""
        # Local variables
        proxyarp_ip = PROXYARP_SUBNET_CREATE
        choparp_processes = self.pfsense_shell("/usr/bin/top -baHS 999 | grep choparp")

        # Ensure there is an active choparp process advertising this subnet
        if proxyarp_ip not in choparp_processes:
            raise AssertionError(f"Expected proxyarp VIP '{proxyarp_ip}' to be present, got {choparp_processes}")

    def is_proxyarp_updated(self):
        """Checks if our proxyarp IP is present after updating a proxyarp VIP"""
        # Local variables
        proxyarp_ip = PROXYARP_SUBNET_UPDATE
        proxyarp_ip_prev = PROXYARP_SUBNET_CREATE
        choparp_processes = self.pfsense_shell("/usr/bin/top -baHS 999 | grep choparp")

        # Ensure there is an active choparp process advertising this subnet
        if proxyarp_ip not in choparp_processes:
            raise AssertionError(f"Expected proxyarp VIP '{proxyarp_ip}' to be present, got {choparp_processes}")

        # Ensure the choparp process advertising the previous subnet is no longer present
        if proxyarp_ip_prev in choparp_processes:
            raise AssertionError(
                f"Expected previous proxyarp VIP '{proxyarp_ip_prev}' to no longer be present, got {choparp_processes}"
            )

    def is_proxyarp_deleted(self):
        """Checks if our proxyarp IP is not present after deleting a proxyarp VIP"""
        # Local variables
        proxyarp_ip =  PROXYARP_SUBNET_UPDATE
        choparp_processes = self.pfsense_shell("/usr/bin/top -baHS 999 | grep choparp")

        # Ensure there is an active choparp process advertising this subnet
        if proxyarp_ip in choparp_processes:
            raise AssertionError(f"Expected proxyarp VIP '{proxyarp_ip}' to be deleted, got {choparp_processes}")


APIE2ETestFirewallVirtualIP()
