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
"""Script used to test the /api/v1/routing/static_route endpoint."""
import e2e_test_framework


class APIE2ETestRoutingStaticRoute(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/routing/static_route endpoint."""
    uri = "/api/v1/routing/static_route"
    get_tests = [{"name": "Read all static routes"}]
    post_tests = [
        {
            "name": "Create static route",
            "resp_time": 5,
            "req_data": {
                "network": "1.2.3.4/32",
                "gateway": "WAN_DHCP",
                "disabled": False,
                "descr": "E2E Test",
                "apply": True
            }
        },
        {
            "name": "Check that static route is present on system",
            "method": "POST",
            "post_test_callable": "check_for_static_route_created",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "netstat -rn"}
        },
        {
            "name": "Check network required constraint",
            "status": 400,
            "return": 6000
        },
        {
            "name": "Check network is CIDR or alias constraint",
            "status": 400,
            "return": 6001,
            "req_data": {"network": "INVALID"}
        },
        {
            "name": "Check network minimum CIDR constraint",
            "status": 400,
            "return": 6002,
            "req_data": {"network": "1.2.3.4/0"}
        },
        {
            "name": "Check network maximum IPv4 CIDR constraint",
            "status": 400,
            "return": 6002,
            "req_data": {"network": "1.2.3.4/33"}
        },
        {
            "name": "Check network maximum IPv4 CIDR constraint",
            "status": 400,
            "return": 6002,
            "req_data": {"network": "0::/129"}
        },
        {
            "name": "Check gateway required constraint",
            "status": 400,
            "return": 6003,
            "req_data": {"network": "1.2.3.4/32"}
        },
        {
            "name": "Check gateway exists constraint",
            "status": 400,
            "return": 6004,
            "req_data": {"network": "1.2.3.4/32", "gateway": "INVALID"}
        },
    ]
    put_tests = [
        {
            "name": "Update static route",
            "req_data": {
                "id": 0,
                "network": "4.3.2.1/32",
                "gateway": "WAN_DHCP",
                "disabled": False,
                "descr": "Updated E2E Test",
                "apply": True
            },
            "resp_time": 5    # Allow a few seconds to reload the routing table
        },
        {
            "name": "Check that static route is updated on system",
            "method": "POST",
            "post_test_callable": "check_for_static_route_updated",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "netstat -rn"}
        },
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 6005
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 6006,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Check network is CIDR or alias constraint",
            "status": 400,
            "return": 6001,
            "req_data": {"id": 0, "network": "INVALID"}
        },
        {
            "name": "Check network minimum CIDR constraint",
            "status": 400,
            "return": 6002,
            "req_data": {"id": 0, "network": "1.2.3.4/0"}
        },
        {
            "name": "Check network maximum IPv4 CIDR constraint",
            "status": 400,
            "return": 6002,
            "req_data": {"id": 0, "network": "1.2.3.4/33"}
        },
        {
            "name": "Check network maximum IPv4 CIDR constraint",
            "status": 400,
            "return": 6002,
            "req_data": {"id": 0, "network": "0::/129"}
        },
        {
            "name": "Check gateway exists constraint",
            "status": 400,
            "return": 6004,
            "req_data": {"id": 0, "network": "1.2.3.4/32", "gateway": "INVALID"}
        }
    ]
    delete_tests = [
        {
            "name": "Delete static route",
            "req_data": {"id": 0, "apply": True},
            "resp_time": 5    # Allow a few seconds to reload the routing table
        },
        {
            "name": "Check that static route is present on system",
            "method": "POST",
            "post_test_callable": "check_for_static_route_deleted",
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": "netstat -rn"}
        },
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 6005
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 6006,
            "req_data": {"id": "INVALID"}
        }
    ]

    def check_for_static_route_created(self):
        """Checks if the static route created by this test case is present on the remote system."""
        # Check if the route we created for 1.2.3.4/32 is found in the routing table.
        if "1.2.3.4" not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError("1.2.3.4/32 was not found in the system table after creation via API")

    def check_for_static_route_updated(self):
        """Checks if the static route updated by this test case is present on the remote system."""
        # Ensure the route was correctly updated in the systems routing table.
        if "4.3.2.1" not in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError("4.3.2.1/32 was not found in the system table after update via API")
        # Ensure the old route is no longer present
        if "1.2.3.4" in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError("old route to 1.2.3.4/32 was still present in the system table after update via API")

    def check_for_static_route_deleted(self):
        """Checks if the static route deleted by this test case is no longer present on the remote system."""
        # Check if the route we delete for 4.3.2.1/32 is no longer in the routing table.
        if "4.3.2.1" in self.last_response.get("data", {}).get("cmd_output", ""):
            raise AssertionError("4.3.2.1/32 was not deleted in the system table after deletion via API")


APIE2ETestRoutingStaticRoute()
