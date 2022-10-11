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
            "payload": {
                "rules": []
            }
        },
        {
            "name": "Flush and replace all rules",
            "payload": {
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
        {"name": "Flush all firewall rules", "payload": {}},
        {
            "name": "Create an allow all rule on the WAN to prevent lockout",
            "uri": "/api/v1/firewall/rule",
            "method": "POST",
            "payload": {
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


APIE2ETestFirewallRuleFlush()
