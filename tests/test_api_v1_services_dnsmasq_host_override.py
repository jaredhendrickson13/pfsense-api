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
"""Script used to test the /api/v1/services/dnsmasq/host_override endpoint."""
import e2e_test_framework


class APIE2ETestServicesDnsmasqHostOverride(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/dnsmasq/host_override endpoint."""
    uri = "/api/v1/services/dnsmasq/host_override"

    get_privileges = ["page-all", "page-services-dnsforwarder"]
    post_privileges = ["page-all", "page-services-dnsforwarder-edithost"]
    put_privileges = ["page-all", "page-services-dnsforwarder-edithost"]
    delete_privileges = ["page-all", "page-services-dnsforwarder-edithost"]

    get_tests = [{"name": "Read all host overrides"}]
    post_tests = [
        {
            "name": "Create IPv4 host override",
            "req_data": {
                "host": "pfsense-api",
                "domain": "e2e.test",
                "ip": "1.2.3.4",
                "descr": "E2E Test IPv4",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "e2e.test",
                        "description": "E2E Test"
                    }
                ]
            }
        },
        {
            "name": "Create IPv6 host override",
            "req_data": {
                "host": "pfsense-api",
                "domain": "e2e.test",
                "ip": "fd00:abcd::",
                "descr": "E2E Test IPv6",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "e2e.test",
                        "description": "E2E Test"
                    }
                ]
            }
        },
        {
            "name": "Test IPv4 host override unique constraint",
            "status": 400,
            "return": 2054,
            "req_data": {
                "host": "pfsense-api",
                "domain": "e2e.test",
                "ip": "1.2.3.4",
                "descr": "E2E Test IPv4",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "e2e.test",
                        "description": "E2E Test"
                    }
                ]
            }
        },
        {
            "name": "Test IPv6 host override unique constraint",
            "status": 400,
            "return": 2054,
            "req_data": {
                "host": "pfsense-api",
                "domain": "e2e.test",
                "ip": "fd00:abcd::",
                "descr": "E2E Test IPv6",
                "aliases": [
                    {
                        "host": "pfsense-api-alias",
                        "domain": "e2e.test",
                        "description": "E2E Test"
                    }
                ]
            }
        },
        {
            "name": "Test host requirement",
            "status": 400,
            "return": 2055
        },
        {
            "name": "Test host validation",
            "status": 400,
            "return": 2056,
            "req_data": {
                "host": "!@#!@#",
                "domain": "e2e.test",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test domain requirement",
            "status": 400,
            "return": 2057,
            "req_data": {
                "host": "example",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test domain validation",
            "status": 400,
            "return": 2058,
            "req_data": {
                "host": "pfsense-api",
                "domain": "!@#!@#",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test IP requirement",
            "status": 400,
            "return": 2059,
            "req_data": {
                "host": "invalid-ip",
                "domain": "e2e.test"
            }
        },
        {
            "name": "Test IP validation",
            "status": 400,
            "return": 2060,
            "req_data": {
                "host": "invalid-ip",
                "domain": "e2e.test",
                "ip": "INVALID",
            }
        },
    ]
    put_tests = [
        {
            "name": "Update IPv4 host override",
            "req_data": {
                "id": 0,
                "host": "updated-pfsense-api",
                "domain": "updated-e2e.test",
                "ip": "4.3.2.1",
                "descr": "Updated E2E Test",
                "aliases": [
                    {
                        "host": "updated-pfsense-api-alias",
                        "domain": "updated-e2e.test",
                        "description": "Updated E2E Test"
                    }
                ],
            }
        },
        {
            "name": "Update IPv6 host override",
            "req_data": {
                "id": 1,
                "host": "updated-pfsense-api",
                "domain": "updated-e2e.test",
                "ip": "abcd:fd00::",
                "descr": "Updated E2E Test IPv6",
                "aliases": [
                    {
                        "host": "updated-pfsense-api-alias",
                        "domain": "updated-e2e.test",
                        "description": "Updated E2E Test"
                    }
                ]
            }
        },
        {
            "name": "Test host and domain field unique constraint update tolerance",
            "req_data": {
                "id": 0,
                "host": "updated-pfsense-api",
                "domain": "updated-e2e.test",
                "ip": "2.4.6.8",
                "descr": "Check host and domain field unique constraint tolerance",
                "apply": True
            },
            "resp_time": 10    # Allow a few seconds for dnsmasq to reload
        },
        {
            "name": "Test IPv4 host override unique constraint",
            "status": 400,
            "return": 2054,
            "req_data": {
                "id": 0,
                "host": "updated-pfsense-api",
                "domain": "updated-e2e.test",
                "ip": "0::",
            }
        },
        {
            "name": "Test IPv6 host override unique constraint",
            "status": 400,
            "return": 2054,
            "req_data": {
                "id": 1,
                "host": "updated-pfsense-api",
                "domain": "updated-e2e.test",
                "ip": "4.3.2.1",
            }
        },
        {
            "name": "Test host validation",
            "status": 400,
            "return": 2056,
            "req_data": {
                "id": 0,
                "host": "!@#!@#",
                "domain": "e2e.test",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test domain validation",
            "status": 400,
            "return": 2058,
            "req_data": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "!@#!@#",
                "ip": "1.2.3.4"
            }
        },
        {
            "name": "Test IP validation",
            "status": 400,
            "return": 2060,
            "req_data": {
                "id": 0,
                "host": "invalid-ip",
                "domain": "e2e.test",
                "ip": "INVALID",
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete IPv4 host override",
            "req_data": {"id": 0}
        },
        {
            "name": "Delete IPv6 host override",
            "req_data": {"id": 0, "apply": True},
            "resp_time": 10    # Allow a few seconds for dnsmasq to reload
        }
    ]


APIE2ETestServicesDnsmasqHostOverride()
