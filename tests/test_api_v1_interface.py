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

class APIUnitTestInterface(unit_test_framework.APIUnitTest):
    url = "/api/v1/interface"
    get_payloads = [{}]
    post_payloads = [
        {
            "if": "em2",
            "descr": "UNIT_TEST",
            "enable": "",
            "type": "staticv4",
            "type6": "staticv6",
            "ipaddr": "10.250.0.1",
            "ipaddrv6": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
            "subnet": "24",
            "subnetv6": "120",
            "blockbogons": True
        }
    ]
    put_payloads = [
        {
            "id": "em2",
            "descr": "UNIT_TEST_UPDATED",
            "enable": "",
            "type": "dhcp",
            "type6": "dhcp6",
            "blockbogons": False,
            "apply": True
        }
    ]
    delete_payloads = [
        {
            "if": "em2"
        }
    ]

APIUnitTestInterface()