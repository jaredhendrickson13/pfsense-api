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
"""Script used to test the /api/v1/firewall/states endpoint."""
import e2e_test_framework


class APIE2ETestFirewallStates(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/states endpoint."""
    uri = "/api/v1/firewall/states"

    get_privileges = ["page-all", "page-diagnostics-showstates"]
    delete_privileges = ["page-all", "page-diagnostics-showstates"]

    get_tests = [{"name": "Read all firewalls states"}]
    delete_tests = [
        {
            "name": "Check firewall state deletion",
            "req_data": {"source": "1.2.3.4"},
            "resp_data_empty": True
        },
        {
            "name": "Check source requirement",
            "status": 400,
            "return": 4231
        },
        {
            "name": "Check source IP/CIDR constraint",
            "status": 400,
            "return": 4232,
            "req_data": {"source": "INVALID"}
        },
        {
            "name": "Check destination IP/CIDR constraint",
            "status": 400,
            "return": 4233,
            "req_data": {"source": "1.2.3.4", "destination": "INVALID"}
        },
        {
            "name": "Check sleep minimum constraint",
            "status": 400,
            "return": 4236,
            "req_data": {"source": "1.2.3.4", "sleep": -1}
        },
        {
            "name": "Check sleep maximum constraint",
            "status": 400,
            "return": 4236,
            "req_data": {"source": "1.2.3.4", "sleep": 301}
        }
    ]


APIE2ETestFirewallStates()
