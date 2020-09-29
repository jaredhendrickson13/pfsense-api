# Copyright 2020 Jared Hendrickson
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

class APIUnitTestFirewallVirtualIP(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/virtual_ip"
    get_payloads = [{}]
    post_payloads = [
        {
            "mode": "carp",
            "interface": "wan",
            "subnet": "172.16.77.239/32",
            "password": "testpass",
            "descr": "Unit Test"
        },
        {
            "mode": "proxyarp",
            "interface": "wan",
            "subnet": "172.16.77.240/32",
            "descr": "Unit Test"
        }
    ]
    put_payloads = [
        {
            "id": 0,
            "mode": "carp",
            "interface": "wan",
            "subnet": "172.16.77.229/32",
            "password": "newtestpass",
            "descr": "Updated unit Test"
        },
        {
            "id": 1,
            "mode": "proxyarp",
            "interface": "wan",
            "subnet": "172.16.77.230/32",
            "descr": "Updated unit Test"
        }
    ]
    delete_payloads = [
        {"id": 0},
        {"id": 0}
    ]

APIUnitTestFirewallVirtualIP()