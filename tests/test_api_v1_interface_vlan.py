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
"""Script used to test the /api/v1/interface/vlan endpoint."""
import random

import e2e_test_framework
from e2e_test_framework.tools import parse_ifconfig

# Constants
EXTRA_IF = "em2"
PARENT_IF_CREATE = EXTRA_IF
PARENT_IF_UPDATE = "em1"
PARENT_IF_DUP = "em1"
VLAN_TAG_CREATE = random.randint(2, 4094)
VLAN_TAG_UPDATE = random.randint(2, 4094)
VLAN_TAG_DUP = random.randint(2, 4094)
VLAN_PCP_CREATE = random.randint(1, 7)
VLAN_PCP_UPDATE = random.randint(1, 7)
VLAN_PCP_DUP = random.randint(1, 7)
VLAN_IF_CREATE = f"{PARENT_IF_CREATE}.{VLAN_TAG_CREATE}"
VLAN_IF_UPDATE = f"{PARENT_IF_UPDATE}.{VLAN_TAG_UPDATE}"
VLAN_IF_DUP = f"{PARENT_IF_DUP}.{VLAN_TAG_DUP}"


class APIE2ETestInterfaceVLAN(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/interface/vlan endpoint."""
    uri = "/api/v1/interface/vlan"

    get_privileges = ["page-all", "page-interfaces-vlan", "page-interfaces-vlan-edit"]
    post_privileges = ["page-all", "page-interfaces-vlan-edit"]
    put_privileges = ["page-all", "page-interfaces-vlan-edit"]
    delete_privileges = ["page-all", "page-interfaces-vlan-edit"]

    get_tests = [{"name": "Read all interface VLANs"}]
    post_tests = [
        {
            "name": "Create extra interface for testing",
            "method": "POST",
            "uri": "/api/v1/interface",
            "req_data": {"if": EXTRA_IF, "enable": True, "apply": True},
            "resp_time": 10
        },
        {
            "name": "Create interface VLAN",
            "req_data": {
                "if": PARENT_IF_CREATE,
                "tag": VLAN_TAG_CREATE,
                "pcp": VLAN_PCP_CREATE,
                "descr": "E2E Test"
            }
        },
        {
            "name": "Assign VLAN to interface for testing",
            "method": "POST",
            "uri": "/api/v1/interface",
            "req_data": {"if": VLAN_IF_CREATE, "enable": True, "apply": True},
            "resp_time": 10
        },
        {
            "name": "Create interface VLAN duplicate for unique constraint tests",
            "post_test_callable": "is_vlan_created",
            "req_data": {
                "if": PARENT_IF_DUP,
                "tag": VLAN_TAG_DUP,
                "pcp": VLAN_PCP_DUP,
                "descr": "E2E dup test"
            }
        },
        {
            "name": "Test non-existent parent interface",
            "status": 400,
            "return": 3051,
            "req_data": {
                "if": "FAKE_INTERFACE",
                "tag": 11,
                "pcp": 0,
                "descr": "E2E Test"
            }
        },
        {
            "name": "Test VLAN tag max threshold",
            "status": 400,
            "return": 3052,
            "req_data": {
                "if": "lo0",
                "tag": 4097,
            }
        },
        {
            "name": "Test VLAN tag minimum threshold",
            "status": 400,
            "return": 3052,
            "req_data": {
                "if": "lo0",
                "tag": 0,
            }
        },
        {
            "name": "Test VLAN priority max threshold",
            "status": 400,
            "return": 3053,
            "req_data": {
                "if": "lo0",
                "tag": 200,
                "pcp": 8,
            }
        },
        {
            "name": "Test VLAN unique constraint",
            "status": 400,
            "return": 3054,
            "req_data": {
                "if": PARENT_IF_DUP,
                "tag": VLAN_TAG_DUP,
            }
        },
    ]
    put_tests = [
        {
            "name": "Update interface VLAN",
            "post_test_callable": "is_vlan_updated",
            "req_data": {
                "vlanif": VLAN_IF_CREATE,
                "if": PARENT_IF_UPDATE,
                "tag": VLAN_TAG_UPDATE,
                "pcp": VLAN_PCP_UPDATE,
                "descr": "Updated E2E test 0"
            }
        },
        {
            "name": "Test VLAN ID or VLANIF required constraint",
            "status": 400,
            "return": 3056
        },
        {
            "name": "Test VLAN ID exists constraint",
            "status": 400,
            "return": 3050,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Test VLAN IF exists constraint",
            "status": 400,
            "return": 3050,
            "req_data": {"vlanif": "INVALID"}
        },
        {
            "name": "Test VLAN tag max threshold",
            "status": 400,
            "return": 3052,
            "req_data": {
                "vlanif": VLAN_IF_UPDATE,
                "tag": 4097,
            }
        },
        {
            "name": "Test VLAN tag minimum threshold",
            "status": 400,
            "return": 3052,
            "req_data": {
                "vlanif": VLAN_IF_UPDATE,
                "tag": 0,
            }
        },
        {
            "name": "Test VLAN priority max threshold",
            "status": 400,
            "return": 3053,
            "req_data": {
                "vlanif": VLAN_IF_UPDATE,
                "pcp": 8,
            }
        },
        {
            "name": "Test VLAN unique constraint",
            "status": 400,
            "return": 3054,
            "req_data": {
                "vlanif": VLAN_IF_UPDATE,
                "if": PARENT_IF_DUP,
                "tag": VLAN_TAG_DUP
            }
        }
    ]
    delete_tests = [
        {
            "name": "Test VLAN ID or VLANIF required constraint",
            "status": 400,
            "return": 3056
        },
        {
            "name": "Test VLAN ID exists constraint",
            "status": 400,
            "return": 3050,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Test VLAN IF exists constraint",
            "status": 400,
            "return": 3050,
            "req_data": {"vlanif": "INVALID"}
        },
        {
            "name": "Ensure VLAN can't be deleted while assigned to interface",
            "status": 400,
            "return": 3049,
            "req_data": {"vlanif": VLAN_IF_UPDATE}
        },
        {
            "name": "Delete interface assignment using VLAN",
            "method": "DELETE",
            "uri": "/api/v1/interface",
            "req_data": {"if": VLAN_IF_UPDATE}
        },
        {
            "name": "Delete extra parent interface",
            "method": "DELETE",
            "uri": "/api/v1/interface",
            "req_data": {"if": EXTRA_IF}
        },
        {
            "name": "Delete interface VLAN",
            "req_data": {
                "vlanif": VLAN_IF_UPDATE
            }
        },
        {
            "name": "Delete duplicate interface VLAN",
            "post_test_callable": "is_vlan_deleted",
            "req_data": {
                "vlanif": VLAN_IF_DUP
            }
        }
    ]

    def is_vlan_created(self):
        """Checks the ifconfig output for the created VLAN."""
        # Lines get too long with f-strings, simpler to keep it using format()
        # pylint: disable=consider-using-f-string

        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")
        ifconfig = parse_ifconfig(ifconfig_out)
        vlan_line = "vlan: {tag} vlanproto: 802.1q vlanpcp: {pcp} parent interface: {parent}".format(
            tag=VLAN_TAG_CREATE,
            pcp=VLAN_PCP_CREATE,
            parent=PARENT_IF_CREATE
        )

        # Check that the VLAN interface is present on the system
        if VLAN_IF_CREATE not in ifconfig:
            raise AssertionError(f"Expected VLAN interface '{VLAN_IF_CREATE}' to exist.")

        # Check the VLAN is configured as requested
        if vlan_line not in ifconfig.get(VLAN_IF_CREATE):
            raise AssertionError(f"Expected {VLAN_IF_CREATE} with PCP {VLAN_PCP_CREATE} to exist")

    def is_vlan_updated(self):
        """Checks the ifconfig output for the updated VLAN."""
        # Lines get too long with f-strings, simpler to keep it using format()
        # pylint: disable=consider-using-f-string

        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")
        ifconfig = parse_ifconfig(ifconfig_out)
        vlan_line = "vlan: {tag} vlanproto: 802.1q vlanpcp: {pcp} parent interface: {parent}".format(
            tag=VLAN_TAG_UPDATE,
            pcp=VLAN_PCP_UPDATE,
            parent=PARENT_IF_UPDATE
        )

        # Check that the old VLAN interface is no longer present on the system
        if VLAN_IF_CREATE in ifconfig:
            raise AssertionError(f"Expected VLAN interface '{VLAN_IF_CREATE}' to no longer exist.")

        # Check that the new VLAN interface is present on the system
        if VLAN_IF_UPDATE not in ifconfig:
            raise AssertionError(f"Expected VLAN interface '{VLAN_IF_UPDATE}' to exist.")

        # Check the new VLAN is configured as requested
        if vlan_line not in ifconfig.get(VLAN_IF_UPDATE):
            raise AssertionError(f"Expected {VLAN_IF_UPDATE} with PCP {VLAN_PCP_UPDATE} to exist")

    def is_vlan_deleted(self):
        """Checks the ifconfig output for the deleted VLAN."""
        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")
        ifconfig = parse_ifconfig(ifconfig_out)

        # Check that the old VLAN interface is no longer present on the system
        if VLAN_IF_CREATE in ifconfig or VLAN_IF_UPDATE in ifconfig or VLAN_IF_DUP in ifconfig:
            raise AssertionError("Expected no VLAN interfaces to exist after deletion.")


APIE2ETestInterfaceVLAN()
