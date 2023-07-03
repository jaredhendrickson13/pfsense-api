# Copyright 2023 Jared Hendrickson
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
"""Script used to test the /api/v1/firewall/states/size endpoint."""
import e2e_test_framework


class APIE2ETestFirewallStatesSize(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/states/size endpoint."""
    uri = "/api/v1/firewall/states/size"

    get_privileges = ["page-all", "page-system-advanced-firewall"]
    put_privileges = ["page-all", "page-system-advanced-firewall"]

    get_tests = [{"name": "Read firewall states size"}]
    put_tests = [
        {
            "name": "Update firewall states size to 20000",
            "req_data": {"maximumstates": 20000}
        },
        {
            "name": "Revert firewall states size to default",
            "req_data": {"maximumstates": "default"}
        }
    ]


APIE2ETestFirewallStatesSize()
