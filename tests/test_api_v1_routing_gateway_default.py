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
"""Script used to test the /api/v1/routing/gateway/default endpoint."""
import e2e_test_framework


class APIE2ETestRoutingGatewayDefault(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/routing/gateway/default endpoint."""
    uri = "/api/v1/routing/gateway/default"
    put_privileges = ["page-all", "page-system-gateways"]
    put_tests = [
        {
            "name": "Check IPv4 gateway exists constraint",
            "status": 400,
            "return": 6028,
            "req_data": {"defaultgw4": "INVALID"}
        },
        {
            "name": "Check IPv6 gateway exists constraint",
            "status": 400,
            "return": 6028,
            "req_data": {"defaultgw6": "INVALID"}
        },
        {
            "name": "Check updating default gateways",
            "resp_time": 10,
            "req_data": {"defaultgw4": "automatic", "defaultgw6": "automatic"}
        }
    ]


APIE2ETestRoutingGatewayDefault()
