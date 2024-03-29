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
"""Script used to test the /api/v1/firewall/nat/one_to_one endpoint."""
import e2e_test_framework


class APIE2ETestFirewallAliasAdvanced(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/alias/advanced endpoint."""
    uri = "/api/v1/firewall/alias/advanced"

    get_privileges = ["page-all", "page-system-advanced-firewall"]
    put_privileges = ["page-all", "page-system-advanced-firewall"]

    get_tests = [
        {"name": "Read all advanced alias settings"}
    ]
    put_tests = [
        {
            "name": "Update advanced alias settings",
            "req_data": {
                "aliasesresolveinterval": 300,
                "checkaliasesurlcert": True
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Ensure aliasesresolveinterval is numeric",
            "status": 400,
            "return": 4242,
            "req_data": {
                "aliasesresolveinterval": "INVALID"
            }
        }
    ]


APIE2ETestFirewallAliasAdvanced()
