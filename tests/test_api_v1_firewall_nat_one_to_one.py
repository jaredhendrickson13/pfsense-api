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
"""Script used to test the /api/v1/firewall/nat/one_to_one endpoint."""
import e2e_test_framework


class APIE2ETestFirewallNATOneToOne(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/nat/one_to_one endpoint."""
    uri = "/api/v1/firewall/nat/one_to_one"
    get_tests = [
        {"name": "Read all 1:1 NAT mappings"}
    ]
    post_tests = [
        {
            "name": "Create 1:1 NAT mapping",
            "payload": {
                "interface": "wan",
                "src": "any",
                "dst": "wanip",
                "external": "192.168.1.123",
                "natreflection": "enable",
                "descr": "E2E Test",
                "nobinat": True,
                "top": True,
                "disabled": True
            }
        }
    ]
    put_tests = [
        {
            "name": "Update 1:1 NAT mapping and apply",
            "payload": {
                "id": 0,
                "interface": "wan",
                "src": "!1.1.1.1",
                "dst": "1.2.3.4",
                "external": "4.3.2.1",
                "natreflection": "disable",
                "descr": "Updated E2E Test",
                "disabled": False,
                "nobinat": False,
                "top": False,
                "apply": True
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        }
    ]
    delete_tests = [
        {
            "name": "Delete 1:1 NAT mapping and apply",
            "payload": {"id": 0, "apply": True},
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        }
    ]


APIE2ETestFirewallNATOneToOne()
