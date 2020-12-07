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

class APIUnitTestInterfaceVLAN(unit_test_framework.APIUnitTest):
    url = "/api/v1/interface/vlan"
    get_payloads = [{}]
    post_payloads = [
        {
            "if": "lo0",
            "tag": 10,
            "pcp": 0,
            "descr": "Unit Test 0"
        },
        {
            "if": "lo0",
            "tag": 11,
            "pcp": 1,
            "descr": "Unit Test 1",
        },
        {
            "if": "lo0",
            "tag": 12,
            "pcp": 2,
            "descr": "Unit Test 2",
        }
    ]
    put_payloads = [
        {
            "id": 0,
            "if": "lo0",
            "tag": 20,
            "pcp": 7,
            "descr": "Updated unit Test 0"
        },
        {
            "vlanif": "lo0.11",
            "if": "lo0",
            "tag": 21,
            "pcp": 6,
            "descr": "Updated unit Test 1"
        },
        {
            "id": 2,
            "if": "lo0",
            "tag": 22,
            "pcp": 5,
            "descr": "Updated unit Test 2"
        }
    ]
    delete_payloads = [
        {
            "id": 0
        },
        {
            "vlanif": "lo0.22"
        },
        {
            "id": 0
        }
    ]

APIUnitTestInterfaceVLAN()