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
            "tag": 3,
            "pcp": 7,
            "descr": "Unit Test"
        }
    ]
    put_payloads = [
        {
            "vlanif": "lo0.3",
            "if": "lo0",
            "tag": 5,
            "pcp": 2,
            "descr": "Updated Unit Test"
        }
    ]
    delete_payloads = [
        {
            "vlanif": "lo0.5"
        }
    ]

APIUnitTestInterfaceVLAN()