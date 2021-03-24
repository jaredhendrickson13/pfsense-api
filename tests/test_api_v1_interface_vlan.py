# Copyright 2021 Jared Hendrickson
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
    uri = "/api/v1/interface/vlan"
    get_tests = [{"name": "Read all interface VLANs"}]
    post_tests = [
        {
            "name": "Create interface VLAN 10",
            "payload": {
                "if": "lo0",
                "tag": 10,
                "pcp": 0,
                "descr": "Unit Test"
            }
        },
        {
            "name": "Create interface VLAN 11",
            "payload": {
                "if": "lo0",
                "tag": 11,
                "pcp": 0,
                "descr": "Unit Test"
            }
        },
        {
            "name": "Test non-existent parent interface",
            "status": 400,
            "return": 3051,
            "payload": {
                "if": "FAKE_INTERFACE",
                "tag": 11,
                "pcp": 0,
                "descr": "Unit Test"
            }
        },
        {
            "name": "Test VLAN tag max threshold",
            "status": 400,
            "return": 3052,
            "payload": {
                "if": "lo0",
                "tag": 4097,
            }
        },
        {
            "name": "Test VLAN tag minimum threshold",
            "status": 400,
            "return": 3052,
            "payload": {
                "if": "lo0",
                "tag": 0,
            }
        },
        {
            "name": "Test VLAN priority max threshold",
            "status": 400,
            "return": 3053,
            "payload": {
                "if": "lo0",
                "tag": 200,
                "pcp": 8,
            }
        },
        {
            "name": "Test VLAN unique constraint",
            "status": 400,
            "return": 3054,
            "payload": {
                "if": "lo0",
                "tag": 10,
            }
        },
    ]
    put_tests = [
        {
            "name": "Update interface VLAN 10 to VLAN 20",
            "payload": {
                "vlanif": "lo0.10",
                "if": "lo0",
                "tag": 20,
                "pcp": 7,
                "descr": "Updated unit Test 0"
            }
        },
        {
            "name": "Update interface VLAN 20 with no changed values",
            "payload": {
                "vlanif": "lo0.20"
            }
        },
        {
            "name": "Update interface VLAN 20 with same values",
            "payload": {
                "vlanif": "lo0.20",
                "if": "lo0",
                "tag": 20,
                "pcp": 7,
                "descr": "Updated unit Test 0"
            }
        },
        {
            "name": "Test VLAN tag max threshold",
            "status": 400,
            "return": 3052,
            "payload": {
                "vlanif": "lo0.20",
                "tag": 4097,
            }
        },
        {
            "name": "Test VLAN tag minimum threshold",
            "status": 400,
            "return": 3052,
            "payload": {
                "vlanif": "lo0.20",
                "tag": 0,
            }
        },
        {
            "name": "Test VLAN priority max threshold",
            "status": 400,
            "return": 3053,
            "payload": {
                "vlanif": "lo0.20",
                "pcp": 8,
            }
        },
        {
            "name": "Test VLAN unique constraint",
            "status": 400,
            "return": 3054,
            "payload": {
                "vlanif": "lo0.20",
                "if": "lo0",
                "tag": 11,
            }
        },

    ]
    delete_tests = [
        {
            "name": "Delete interface VLAN 20 by ID",
            "payload": {
                "id": 0
            }
        },
        {
            "name": "Delete interface VLAN 11",
            "payload": {
                "vlanif": "lo0.11"
            }
        }
    ]

APIUnitTestInterfaceVLAN()