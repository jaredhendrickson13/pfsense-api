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
"""Script used to test the /api/v1/interface/bridge endpoint."""
import e2e_test_framework
from e2e_test_framework.tools import parse_ifconfig

# Constants
BR_MEMBER_IF_CREATE = "em1"
BR_MEMBER_IF_UPDATE = "em2"


class APIE2ETestInterfaceBridge(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/interface/bridge endpoint."""
    uri = "/api/v1/interface/bridge"
    get_tests = [{"name": "Read all interface VLANs"}]
    post_tests = [
        {
            "name": "Assign extra interface to use in tests",
            "method": "POST",
            "uri": "/api/v1/interface",
            "req_data": {
                "if": BR_MEMBER_IF_UPDATE,
                "enable": True,
                "apply": True
            },
            "resp_time": 10
        },
        {
            "name": "Create bridge for LAN",
            "post_test_callable": "is_bridge_created",
            "req_data": {
                "members": [BR_MEMBER_IF_CREATE],
                "descr": "Test bridge"
            }
        },
        {
            "name": "Check members required constraint",
            "status": 400,
            "return": 3066
        },
        {
            "name": "Check members minimum constraint",
            "status": 400,
            "return": 3066,
            "req_data": {
                "members": []
            }
        },
        {
            "name": "Check non-existent member interface constraint",
            "status": 400,
            "return": 3067,
            "req_data": {
                "members": ["NonexistingIface"]
            }
        },
        {
            "name": "Check 1 bridge per member constraint",
            "status": 400,
            "return": 3068,
            "req_data": {
                "members": ["lan"]
            }
        }
    ]
    put_tests = [
        {
            "name": "Update bridge for LAN",
            "post_test_callable": "is_bridge_updated",
            "req_data": {
                "id": "bridge0",
                "members": [BR_MEMBER_IF_UPDATE],
                "descr": "Updated test bridge"
            }
        },
        {
            "name": "Check bridge ID required constraint",
            "status": 400,
            "return": 3071
        },
        {
            "name": "Check bridge ID exists constraint",
            "status": 400,
            "return": 3072,
            "req_data": {
                "id": "invalidbridgeid"
            }
        },
        {
            "name": "Check members minimum constraint",
            "status": 400,
            "return": 3066,
            "req_data": {
                "id": "bridge0",
                "members": []
            }
        },
        {
            "name": "Check non-existent member interface constraint",
            "status": 400,
            "return": 3067,
            "req_data": {
                "id": "bridge0",
                "members": ["NonexistingIface"]
            }
        }
    ]
    delete_tests = [
        {
            "name": "Check bridge ID required constraint",
            "status": 400,
            "return": 3071
        },
        {
            "name": "Check bridge ID exists constraint",
            "status": 400,
            "return": 3072,
            "req_data": {
                "id": "invalidbridgeid"
            }
        },
        {
            "name": "Check bridge deletion",
            "post_test_callable": "is_bridge_deleted",
            "req_data": {
                "id": "bridge0"
            }
        },
        {
            "name": "Delete interface used for testing",
            "method": "DELETE",
            "uri": "/api/v1/interface",
            "req_data": {"if": BR_MEMBER_IF_UPDATE, "apply": True},
            "resp_time": 5
        }
    ]

    def is_bridge_created(self):
        """Checks if the bridge interface is present and contains the correct members"""
        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")
        ifconfig = parse_ifconfig(ifconfig_out)

        # Check if the bridge interface was added to the system
        if "bridge0" not in ifconfig:
            raise AssertionError("Expected bridge0 interface to be present")

        # Check that the bridge interface has the correct member(s)
        if f"member: {BR_MEMBER_IF_CREATE}" not in "\n".join(ifconfig.get("bridge0")):
            raise AssertionError(f"Expected bridge0 interface to contain member interface {BR_MEMBER_IF_CREATE}")

    def is_bridge_updated(self):
        """Checks if the bridge interface is updated and contains the correct members"""
        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")
        ifconfig = parse_ifconfig(ifconfig_out)

        # Check if the bridge interface was added to the system
        if "bridge0" not in ifconfig:
            raise AssertionError("Expected bridge0 interface to be present")

        # Check that the bridge interface no longer has the old member
        if f"member: {BR_MEMBER_IF_CREATE}" in "\n".join(ifconfig.get("bridge0")):
            raise AssertionError(f"Bridge0 interface still using member interface {BR_MEMBER_IF_CREATE}")

        # Check that the bridge interface has the correct updated member(s)
        if f"member: {BR_MEMBER_IF_UPDATE}" not in "\n".join(ifconfig.get("bridge0")):
            raise AssertionError(f"Expected bridge0 interface to contain member interface {BR_MEMBER_IF_UPDATE}")

    def is_bridge_deleted(self):
        """Checks if the bridge interface is deleted on the system"""
        # Local variables
        ifconfig_out = self.pfsense_shell("ifconfig")
        ifconfig = parse_ifconfig(ifconfig_out)

        # Check if the bridge interface was added to the system
        if "bridge0" in ifconfig:
            raise AssertionError("Expected bridge0 interface to be deleted")


APIE2ETestInterfaceBridge()
