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
"""Script used to test the /api/v1/services/unbound/host_override/flush endpoint."""
import e2e_test_framework


class APIE2ETestServicesUnboundHostOverrideFlush(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/unbound/host_override/flush endpoint."""
    uri = "/api/v1/services/unbound/host_override/flush"
    put_tests = [
        {
            "name": "Check host_overrides field array requirement",
            "return": 2098,
            "status": 400
        },
        {
            "name": "Check ability to replace all existing host overrides",
            "req_data": {
                "host_overrides": [
                    {
                        "host": "test1",
                        "domain": "example.com",
                        "ip": ["127.0.0.1"],
                        "descr": "Entry #1",
                        "aliases": [
                            {"host": "test1-alias", "domain": "example.com"}
                        ]
                    },
                    {
                        "host": "test2",
                        "domain": "example.com",
                        "ip": ["127.0.0.1"],
                        "descr": "Entry #2",
                        "aliases": [
                            {"host": "test2-alias", "domain": "example.com"}
                        ]
                    }
                ]
            }
        }
    ]
    delete_tests = [
        {"name": "Flush all Unbound host overrides", "req_data": {}},
    ]


APIE2ETestServicesUnboundHostOverrideFlush()
