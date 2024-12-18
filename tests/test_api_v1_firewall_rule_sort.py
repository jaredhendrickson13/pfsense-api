#Copyright 2019-2025 Jared Hendrickson
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
"""Script used to test the /api/v1/firewall/rule/sort endpoint."""
import e2e_test_framework


class APIE2ETestFirewallRuleSort(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/rule/sort endpoint."""
    uri = "/api/v1/firewall/rule/sort"
    put_privileges = ["page-all", "page-firewall-rules"]
    put_tests = [
        {
            "name": "Set firewall rules in a random order",
            "uri": "/api/v1/firewall/rule/flush",
            "req_data": {
                "rules": [
                    {
                        "type": "pass",
                        "interface": "wan",
                        "ipprotocol": "inet",
                        "protocol": "any",
                        "src": "any",
                        "dst": "any",
                        "descr": "2 - Test Rule"
                    },
                    {
                        "type": "pass",
                        "interface": "wan",
                        "ipprotocol": "inet",
                        "protocol": "any",
                        "src": "any",
                        "dst": "any",
                        "descr": "0 - Test Rule"
                    },
                    {
                        "type": "pass",
                        "interface": "wan",
                        "ipprotocol": "inet",
                        "protocol": "any",
                        "src": "any",
                        "dst": "any",
                        "descr": "3 - Test Rule"
                    },
                    {
                        "type": "pass",
                        "interface": "wan",
                        "ipprotocol": "inet",
                        "protocol": "any",
                        "src": "any",
                        "dst": "any",
                        "descr": "1 - Test Rule"
                    }
                ]
            }
        },
        {
            "name": "Sort firewall rules alphabetically by their description in an ascending order",
            "post_test_callable": "are_rules_ascending_by_descr",
            "req_data": {
                "field": "descr",
                "option": "ascend",
                "dry_run": True
            }
        },
        {
            "name": "Sort firewall rules alphabetically by their description in an descending order",
            "post_test_callable": "are_rules_descending_by_descr",
            "req_data": {
                "field": "descr",
                "option": "descend",
                "dry_run": True
            }
        },
        {
            "name": "Check sort field requirement",
            "status": 400,
            "return": 4237,
            "req_data": {
                "option": "descend",
                "dry_run": True
            }
        },
        {
            "name": "Check sort option choices constraint",
            "status": 400,
            "return": 4238,
            "req_data": {
                "field": "descr",
                "option": "INVALID",
                "dry_run": True
            }
        }
    ]

    def are_rules_ascending_by_descr(self):
        """Checks if the ACL is in ascending order by descr"""
        # Since we are sorting in ascending order, start at 3
        counter = 0

        # Loop through each data entry from the last request
        for rule in self.last_response.get("data"):
            if rule.get("descr") != f"{counter} - Test Rule":
                raise AssertionError("Expected ACL to be in ascending order by it's description")

            counter = counter + 1

    def are_rules_descending_by_descr(self):
        """Checks if the ACL is in descending order by descr"""
        # Since we are sorting in descending order, start at 3
        counter = 3

        # Loop through each data entry from the last request
        for rule in self.last_response.get("data"):
            if rule.get("descr") != f"{counter} - Test Rule":
                raise AssertionError("Expected ACL to be in descending order by it's description")

            counter = counter - 1





APIE2ETestFirewallRuleSort()
