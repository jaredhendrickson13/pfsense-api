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

import e2e_test_framework

class APIE2ETestFirewallRuleSort(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/firewall/rule/sort"

    put_tests = [
        {
            "name": "Sort firewall rules alphaetically by their description in an ascending order",
            "payload": {
                "field": "descr",
                "option": "ascend",
                "dry_run": True
            }
        },
        {
            "name": "Check sort field requirement",
            "status": 400,
            "return": 4237,
            "payload": {
                "option": "descend",
                "dry_run": True            }
        },
        {
            "name": "Check sort option choices constraint",
            "status": 400,
            "return": 4238,
            "payload": {
                "field": "descr",
                "option": "INVALID",
                "dry_run": True            }
        },
    ]

APIE2ETestFirewallRuleSort()
