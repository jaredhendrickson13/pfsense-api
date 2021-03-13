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


class APIUnitTestServicesUnboundHostOverride(unit_test_framework.APIUnitTest):
    uri = "/api/v1/services/unbound/host_override"
    get_tests = [{"name": "Read all host overrides"}]
    post_tests = [
        {
            "name": "Create IPv4 host override",
            "payload": {
                "host": "pfsense-api",
                "domain": "unit.test",
                "ip": "1.2.3.4",
                "descr": "Unit Test IPv4",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "unit.test",
                        "description": "Unit Test"
                    }
                ]
            }
        },
        {
            "name": "Create IPv6 host override",
            "payload": {
                "host": "pfsense-api",
                "domain": "unit.test",
                "ip": "fd00:abcd::",
                "descr": "Unit Test IPv6",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "unit.test",
                        "description": "Unit Test"
                    }
                ]
            }
        },
        {
            "name": "Test IPv4 host override unique constraint",
            "status": 400,
            "return": 2010,
            "payload": {
                "host": "pfsense-api",
                "domain": "unit.test",
                "ip": "1.2.3.4",
                "descr": "Unit Test IPv4",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "unit.test",
                        "description": "Unit Test"
                    }
                ]
            }
        },
        {
            "name": "Test IPv6 host override unique constraint",
            "status": 400,
            "return": 2010,
            "payload": {
                "host": "pfsense-api",
                "domain": "unit.test",
                "ip": "fd00:abcd::",
                "descr": "Unit Test IPv6",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "unit.test",
                        "description": "Unit Test"
                    }
                ]
            }
        },
        {
            "name": "Test host validation",
            "status": 400,
            "return": 2046,
            "payload": {
                "host": "!@#!@#",
                "domain": "unit.test",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test domain validation",
            "status": 400,
            "return": 2047,
            "payload": {
                "host": "pfsense-api",
                "domain": "!@#!@#",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test IP validation",
            "status": 400,
            "return": 2011,
            "payload": {
                "host": "invalid-ip",
                "domain": "unit.test",
                "ip": "INVALID",
            }
        },
    ]
    put_tests = [
        {
            "name": "Update IPv4 host override",
            "payload": {
                "id": 0,
                "host": "updated-pfsense-api",
                "domain": "updated-unit.test",
                "ip": "4.3.2.1",
                "descr": "Updated Unit Test",
                "aliases": [
                    {
                        "host": "updated-pfsense-api-alias",
                        "domain": "updated-unit.test",
                        "description": "Updated Unit Test"
                    }
                ],
            }
        },
        {
            "name": "Update IPv6 host override",
            "payload": {
                "id": 1,
                "host": "updated-pfsense-api",
                "domain": "updated-unit.test",
                "ip": "abcd:fd00::",
                "descr": "Updated Unit Test IPv6",
                "aliases": [
                    {
                        "host": "updated-pfsense-api-alias",
                        "domain": "updated-unit.test",
                        "description": "Updated Unit Test"
                    }
                ]
            }
        },
        {
            "name": "Test host and domain field unique constraint update tolerance",
            "payload": {
                "id": 0,
                "host": "updated-pfsense-api",
                "domain": "updated-unit.test",
                "ip": "2.4.6.8",
                "descr": "Check host and domain field unique constraint tolerance",
                "apply": True
            },
            "resp_time": 10    # Allow a few seconds for Unbound to reload
        },
        {
            "name": "Test IPv4 host override unique constraint",
            "status": 400,
            "return": 2010,
            "payload": {
                "id": 0,
                "host": "updated-pfsense-api",
                "domain": "updated-unit.test",
                "ip": "0::",
            }
        },
        {
            "name": "Test IPv6 host override unique constraint",
            "status": 400,
            "return": 2010,
            "payload": {
                "id": 1,
                "host": "updated-pfsense-api",
                "domain": "updated-unit.test",
                "ip": "4.3.2.1",
            }
        },
        {
            "name": "Test host validation",
            "status": 400,
            "return": 2046,
            "payload": {
                "id": 0,
                "host": "!@#!@#",
                "domain": "unit.test",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test domain validation",
            "status": 400,
            "return": 2047,
            "payload": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "!@#!@#",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test IP validation",
            "status": 400,
            "return": 2011,
            "payload": {
                "id": 0,
                "host": "invalid-ip",
                "domain": "unit.test",
                "ip": "INVALID",
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete IPv4 host overrride",
            "payload": {"id": 0}
        },
        {
            "name": "Delete IPv6 host overrride",
            "payload": {"id": 0, "apply": True},
            "resp_time": 10    # Allow a few seconds for Unbound to reload
        }
    ]


APIUnitTestServicesUnboundHostOverride()
