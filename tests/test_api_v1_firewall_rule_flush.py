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
"""Script used to test the /api/v1/firewall/rule/flush endpoint."""
import e2e_test_framework


class APIE2ETestFirewallRuleFlush(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/rule/flush endpoint."""
    uri = "/api/v1/firewall/rule/flush"
    put_tests = [
        {
            "name": "Check rules array type constraint",
            "status": 400,
            "return": 4240
        },
        {
            "name": "Check rules array minimum items constraint",
            "status": 400,
            "return": 4241,
            "req_data": {
                "rules": []
            }
        },
        {
            "name": "Flush and replace all rules",
            "req_data": {
                "apply": "true",
                "rules": [
                    {
                        "type": "pass",
                        "interface": "wan",
                        "ipprotocol": "inet",
                        "protocol": "any",
                        "src": "any",
                        "dst": "any"
                    },
                    {
                        "type": "pass",
                        "interface": "wan",
                        "ipprotocol": "inet",
                        "protocol": "any",
                        "src": "lan",
                        "dst": "any"
                    }
                ]
            }
        }
    ]
    delete_tests = [
        {"name": "Flush all firewall rules"},
        {
            "name": "Read all firewall rules and ensure it is now empty",
            "uri": "/api/v1/firewall/rule",
            "method": "GET",
            "resp_data_empty": True,
            "post_test_callable": "is_acl_empty"
        },
        {
            "name": "Create an allow all rule on the WAN to prevent lockout",
            "uri": "/api/v1/firewall/rule",
            "method": "POST",
            "req_data": {
                "interface": "wan",
                "type": "pass",
                "ipprotocol": "inet",
                "protocol": "any",
                "src": "any",
                "dst": "any",
                "apply": True
            }
        },
    ]

    def is_acl_empty(self):
        """Checks if the response data from a GET firewall rules call is empty after flush"""
        if self.last_response.get("data"):
            raise AssertionError("Expected no firewall rules to be present")


APIE2ETestFirewallRuleFlush()
