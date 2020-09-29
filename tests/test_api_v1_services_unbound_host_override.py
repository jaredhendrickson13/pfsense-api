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

class APIUnitTestServicesUnboundHostOverride(unit_test_framework.APIUnitTest):
    url = "/api/v1/services/unbound/host_override"
    get_payloads = [{}]
    post_payloads = [
        {
            "host": "pfsense-api",
            "domain": "unit.test",
            "ip": "1.2.3.4",
            "descr": "Unit Test",
            "aliases": [
                {
                    "host": "pfsense-api-alias",
                    "domain": "unit.test",
                    "description": "Unit Test"
                }
            ]
        }
    ]
    put_payloads = [
        {
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
            "apply": True
        }
    ]
    delete_payloads = [
        {"id": 0}
    ]

APIUnitTestServicesUnboundHostOverride()