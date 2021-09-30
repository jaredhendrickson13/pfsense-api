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

class APIUnitTestInterface(unit_test_framework.APIUnitTest):
    uri = "/api/v1/interface"
    get_tests = [
        {"name": "Read all configured interfaces"}
    ]
    post_tests = [
        {
            "name": "Create interface bridge",
            "uri": "/api/v1/interface/bridge",
            "payload": {
                "members": "em1"
            }
        },
        {
            "name": "Create a staticv4/staticv6 interface",
            "payload": {
                "if": "em2",
                "descr": "UNIT_TEST",
                "enable": True,
                "type": "staticv4",
                "type6": "staticv6",
                "ipaddr": "10.250.0.1",
                "ipaddrv6": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "subnet": "24",
                "subnetv6": "120",
                "blockbogons": True
            }
        },
        {
            "name": "Create a static interface on the bridge",
            "payload": {
                "if": "bridge0",
                "descr": "BRIDGE_TEST",
                "enable": False,
                "type": "dhcp",
                "blockbogons": True
            }
        }
    ]
    put_tests = [
        {
            "name": "Update staticv4/staticv6 interface to dhcp/dhcp6 and apply",
            "payload": {
                "id": "em2",
                "descr": "UNIT_TEST_UPDATED",
                "enable": False,
                "type": "dhcp",
                "type6": "dhcp6",
                "blockbogons": False,
                "apply": True
            },
            "resp_time": 5    # Allow a few seconds to bounce the interface when applying
        }
    ]
    delete_tests = [
        {
            "name": "Delete interface",
            "payload": {
                "if": "em2"
            }
        },
        {
            "name": "Check cannot delete interface bridge in use constraint",
            "uri": "/api/v1/interface/bridge",
            "status": 400,
            "return": 3073,
            "payload": {
                "id": "bridge0"
            }
        },
        {
            "name": "Delete bridged interface",
            "payload": {
                "if": "bridge0"
            }
        },
        {
            "name": "Delete interface bridge",
            "uri": "/api/v1/interface/bridge",
            "payload": {
                "id": "bridge0"
            }
        }
    ]

APIUnitTestInterface()
