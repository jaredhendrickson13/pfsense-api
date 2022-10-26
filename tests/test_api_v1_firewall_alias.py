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
"""Script used to test the /api/v1/firewall/alias endpoint."""
import e2e_test_framework


class APIE2ETestFirewallAlias(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/alias endpoint."""
    uri = "/api/v1/firewall/alias"
    get_tests = [
        {"name": "Read all aliases"}
    ]
    post_tests = [
        {
            "name": "Create network alias",
            "payload": {
                "name": "RFC1918",
                "type": "network",
                "descr": "E2E Test",
                "address": ["10.0.0.0/8", "172.16.0.0/12", "192.168.0.0/24"],
                "detail": ["Class A", "Class B", "Class C"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Create port alias",
            "payload": {
                "name": "HTTP_PORTS",
                "type": "port",
                "descr": "E2E Test",
                "address": [80, 443, 8443, "8080:8081"],
                "detail": ["HTTP", "HTTPS", "HTTPS-ALT"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Create host alias",
            "payload": {
                "name": "DNS_SERVERS",
                "type": "host",
                "descr": "E2E Test",
                "address": ["1.1.1.1", "8.8.8.8", "8.8.4.4", "RFC1918"],
                "detail": ["Cloudflare DNS", "Google DNS", "Secondary Google DNS"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Create host alias using hostnames",
            "payload": {
                "name": "GOOGLE_DNS",
                "type": "host",
                "descr": "E2E Test",
                "address": ["dns.google"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Check that GOOGLE_DNS actually populates a table with resolved hostnames",
            "method": "GET",
            "delay": 3,
            "uri": "/api/v1/system/table",
            "post_test_callable": "check_google_dns_table",
            "payload": {"name": "GOOGLE_DNS"}
        },
        {
            "name": "Test name requirement",
            "status": 400,
            "return": 4050
        },
        {
            "name": "Test name unique constraint",
            "status": 400,
            "return": 4056,
            "payload": {
                "name": "DNS_SERVERS"
            }
        },
        {
            "name": "Test name validation",
            "status": 400,
            "return": 4053,
            "payload": {
                "name": "!@#"
            }
        },
        {
            "name": "Test type requirement",
            "status": 400,
            "return": 4061,
            "payload": {
                "name": "TEST"
            }
        },
        {
            "name": "Test type validation",
            "status": 400,
            "return": 4057,
            "payload": {
                "name": "TEST",
                "type": "INVALID"
            }
        },
        {
            "name": "Test network alias address validation",
            "status": 400,
            "return": 4059,
            "payload": {
                "name": "TEST",
                "type": "network",
                "address": ["!@#!@#!@#"]
            }
        },
        {
            "name": "Test host alias address validation",
            "status": 400,
            "return": 4058,
            "payload": {
                "name": "TEST",
                "type": "host",
                "address": ["!@#!@#!@#"]
            }
        },
        {
            "name": "Test port alias address validation",
            "status": 400,
            "return": 4060,
            "payload": {
                "name": "TEST",
                "type": "port",
                "address": ["!@#!@#!@#"]
            }
        }
    ]
    put_tests = [
        {
            "name": "Update network alias",
            "payload": {
                "id": "RFC1918",
                "name": "UPDATED_RFC1918",
                "type": "network",
                "descr": "Updated E2E Test",
                "address": ["10.0.0.0/32", "172.16.0.0/32", "192.168.0.0/32"],
                "detail": ["New Class A", "New Class B", "New Class C"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Update port alias",
            "payload": {
                "id": "HTTP_PORTS",
                "name": "UPDATED_HTTP_PORTS",
                "type": "port",
                "descr": "Updated E2E Test",
                "address": [8080, 4433, 443],
                "detail": ["HTTP-ALT", "HTTPS-ALT"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Update host alias",
            "payload": {
                "id": "DNS_SERVERS",
                "name": "UPDATED_DNS_SERVERS",
                "type": "host",
                "descr": "Updated E2E Test",
                "address": ["8.8.8.8"],
                "detail": ["Google DNS"]
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Test ID requirement",
            "status": 400,
            "return": 4050
        },
        {
            "name": "Test name unique constraint",
            "status": 400,
            "return": 4056,
            "payload": {
                "id": "UPDATED_HTTP_PORTS",
                "name": "UPDATED_DNS_SERVERS"
            }
        },
        {
            "name": "Test name validation",
            "status": 400,
            "return": 4053,
            "payload": {
                "id": "UPDATED_HTTP_PORTS",
                "name": "!@#"
            }
        },
        {
            "name": "Test type validation",
            "status": 400,
            "return": 4057,
            "payload": {
                "id": "UPDATED_HTTP_PORTS",
                "type": "INVALID"
            }
        },
        {
            "name": "Test update host to network alias address validation tolerance",
            "status": 200,
            "payload": {
                "id": "UPDATED_DNS_SERVERS",
                "type": "network"
            }
        },
        {
            "name": "Test update network to port alias address validation",
            "status": 400,
            "return": 4060,
            "payload": {
                "id": "UPDATED_RFC1918",
                "type": "port"
            }
        },
        {
            "name": "Test update port to host alias address validation",
            "status": 400,
            "return": 4058,
            "payload": {
                "id": "UPDATED_HTTP_PORTS",
                "type": "host"
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete network alias",
            "payload": {
                "id": "UPDATED_RFC1918"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Delete port alias",
            "payload": {
                "id": "UPDATED_HTTP_PORTS"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Delete host alias",
            "payload": {
                "id": "UPDATED_DNS_SERVERS"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Delete GOOGLE_DNS alias/table",
            "payload": {
                "id": "GOOGLE_DNS"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Check that GOOGLE_DNS table no longer exists",
            "method": "GET",
            "delay": 8,
            "uri": "/api/v1/system/table",
            "status": 400,
            "return": 1083,
            "payload": {"name": "GOOGLE_DNS"}
        },
        {
            "name": "Test delete non-existing alias",
            "status": 400,
            "return": 4055,
            "payload": {
                "id": "INVALID"
            }
        }
    ]

    def check_google_dns_table(self):
        """Checks the the GOOGLE_DNS table is created with our dns.google hostname alias"""
        # If the return data is a dict, convert it to a list
        if isinstance(self.last_response["data"], dict):
            tables = self.last_response["data"].values()
        # If the return data is a list, just store the list in the tables var
        elif isinstance(self.last_response["data"], list):
            tables = self.last_response["data"]
        # Otherwise, our table doesn't exist, fail the test
        else:
            raise AssertionError("expected GOOGLE_DNS table to exist")

        # Ensure the alias correctly resolved 8.8.8.8 and 8.8.4.4 in the table
        for table in tables:
            if "8.8.8.8" not in table["entries"]:
                raise AssertionError(f"expected '8.8.8.8' to be in GOOGLE_DNS table {self.last_response}")
            if "8.8.4.4" not in table["entries"]:
                raise AssertionError("expected '8.8.4.4' to be in GOOGLE_DNS table")


APIE2ETestFirewallAlias()
