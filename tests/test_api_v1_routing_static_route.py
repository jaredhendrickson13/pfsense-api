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

import unit_test_framework

class APIUnitTestRoutingStaticRoute(unit_test_framework.APIUnitTest):
    uri = "/api/v1/routing/static_route"
    get_tests = [{"name": "Read all static routes"}]
    post_tests = [
        {
            "name": "Create static route",
            "payload": {"network": "172.16.172.0/29", "gateway": "WAN_DHCP", "disabled": True, "descr": "Unit Test"}
        }
    ]
    put_tests = [
        {
            "name": "Update static route",
            "payload": {
                "id": 0,
                "network": "172.16.173.0/29",
                "gateway": "WAN_DHCP",
                "disabled": True,
                "descr": "Updated Unit Test"
            },
            "resp_time": 5    # Allow a few seconds to reload the routing table
        },
    ]
    delete_tests = [
        {
            "name": "Delete static route",
            "payload": {"id": 0},
            "resp_time": 5    # Allow a few seconds to reload the routing table
        }
    ]

APIUnitTestRoutingStaticRoute()
