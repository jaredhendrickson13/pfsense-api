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

    get_privileges = ["page-all", "page-firewall-nat-1-1"]
    post_privileges = ["page-all", "page-firewall-nat-1-1-edit"]
    put_privileges = ["page-all", "page-firewall-nat-1-1-edit"]
    delete_privileges = ["page-all", "page-firewall-nat-1-1-edit"]

    get_tests = [
        {"name": "Read all 1:1 NAT mappings"}
    ]
    post_tests = [
        {
            "name": "Create 1:1 NAT mapping",
            "req_data": {
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
        },
        {
            "name": "Check interface required constraint",
            "status": 400,
            "return": 4075
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 4076,
            "req_data": {"interface": "INVALID"}
        },
        {
            "name": "Check external required constraint",
            "status": 400,
            "return": 4081,
            "req_data": {"interface": "wan"}
        },
        {
            "name": "Check external IP address constraint",
            "status": 400,
            "return": 4082,
            "req_data": {"interface": "wan", "external": "INVALID"}
        },
        {
            "name": "Check src required constraint",
            "status": 400,
            "return": 4077,
            "req_data": {"interface": "wan", "external": "127.0.0.1"}
        },
        {
            "name": "Check src syntax validation",
            "status": 400,
            "return": 4079,
            "req_data": {"interface": "wan", "external": "127.0.0.1", "src": "INVALID"}
        },
        {
            "name": "Check dst required constraint",
            "status": 400,
            "return": 4078,
            "req_data": {"interface": "wan", "external": "127.0.0.1", "src": "127.0.0.1/32"}
        },
        {
            "name": "Check dst syntax validation",
            "status": 400,
            "return": 4080,
            "req_data": {"interface": "wan", "external": "127.0.0.1", "src": "127.0.0.1", "dst": "INVALID"}
        },
        {
            "name": "Check nat reflection options constraint",
            "status": 400,
            "return": 4008,
            "req_data": {
                "interface": "wan",
                "external": "127.0.0.1",
                "src": "127.0.0.1",
                "dst": "127.0.0.1",
                "natreflection": "INVALID"
            }
        }
    ]
    put_tests = [
        {
            "name": "Update 1:1 NAT mapping and apply",
            "resp_time": 3,    # Allow a few seconds for the firewall filter to reload
            "req_data": {
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
            }
        },
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 4083
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 4084,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 4076,
            "req_data": {"id": 0, "interface": "INVALID"}
        },
        {
            "name": "Check external IP address constraint",
            "status": 400,
            "return": 4082,
            "req_data": {"id": 0, "interface": "wan", "external": "INVALID"}
        },
        {
            "name": "Check src syntax validation",
            "status": 400,
            "return": 4079,
            "req_data": {"id": 0, "interface": "wan", "external": "127.0.0.1", "src": "INVALID"}
        },
        {
            "name": "Check dst syntax validation",
            "status": 400,
            "return": 4080,
            "req_data": {"id": 0, "interface": "wan", "external": "127.0.0.1", "src": "127.0.0.1", "dst": "INVALID"}
        },
        {
            "name": "Check nat reflection options constraint",
            "status": 400,
            "return": 4008,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "external": "127.0.0.1",
                "src": "127.0.0.1",
                "dst": "127.0.0.1",
                "natreflection": "INVALID"
            }
        }
    ]
    delete_tests = [
        {
            "name": "Delete 1:1 NAT mapping and apply",
            "req_data": {"id": 0, "apply": True},
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 4083
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 4084,
            "req_data": {"id": "INVALID"}
        }
    ]


APIE2ETestFirewallNATOneToOne()
