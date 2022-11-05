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
"""Class used to test the /api/v1/services/unbound/host_override endpoint."""
import subprocess
import ipaddress

import e2e_test_framework

# CONSTANTS
HOST_CREATE = "example"
DOMAIN_CREATE = "example.com"
IPV4_CREATE = "1.2.3.4"
IPV6_CREATE = ipaddress.IPv6Address("::1")
IP_CREATE = [str(IPV4_CREATE), str(IPV6_CREATE)]
ALIAS_HOST_CREATE = "example-alias"
ALIAS_DOMAIN_CREATE = "alias.example.com"
HOST_UPDATE = "example-update"
DOMAIN_UPDATE = "update.example.com"
IPV4_UPDATE = "4.3.2.1"
IPV6_UPDATE = ipaddress.IPv6Address("2001:0000:3238:DFE1:0063:0000:0000:FEFB")
IP_UPDATE = [str(IPV4_UPDATE), str(IPV6_UPDATE)]
ALIAS_HOST_UPDATE = "update-example-alias"
ALIAS_DOMAIN_UPDATE = "update.alias.example.com"


class APIE2ETestServicesUnboundHostOverride(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/unbound/host_override endpoint."""
    uri = "/api/v1/services/unbound/host_override"
    get_tests = [{"name": "Read all host overrides"}]
    post_tests = [
        {
            "name": "Create allow access list so we can test DNS resolution",
            "method": "POST",
            "uri": "/api/v1/services/unbound/access_list",
            "req_data": {
                "aclname": "ALLOW_ALL",
                "aclaction": "allow",
                "row": [
                    {
                        "acl_network": "0.0.0.0",
                        "mask": 0
                    }
                ]
            }
        },
        {
            "name": "Create host override",
            "post_test_callable": "is_host_override_created",
            "resp_time": 10,
            "req_data": {
                "apply": True,
                "host": HOST_CREATE,
                "domain": DOMAIN_CREATE,
                "ip": IP_CREATE,
                "descr": "E2E Test",
                "aliases": [
                    {
                        "host": ALIAS_HOST_CREATE,
                        "domain": ALIAS_DOMAIN_CREATE,
                        "description": "E2E Test"
                    }
                ]
            }
        },
        {
            "name": "Check host is required constraint",
            "status": 400,
            "return": 2004
        },
        {
            "name": "Check host is valid hostname constraint",
            "status": 400,
            "return": 2046,
            "req_data": {
                "host": "!INVALID!"
            }
        },
        {
            "name": "Check domain is required constraint",
            "status": 400,
            "return": 2005,
            "req_data": {
                "host": "pfsense-api"
            }
        },
        {
            "name": "Check domain is valid hostname constraint",
            "status": 400,
            "return": 2047,
            "req_data": {
                "host": "pfsense-api",
                "domain": "!INVALID!"
            }
        },
        {
            "name": "Check IP is required constraint",
            "status": 400,
            "return": 2006,
            "req_data": {
                "host": "pfsense-api",
                "domain": "example.com"
            }
        },
        {
            "name": "Check IP is valid IPv4/IPv6 constraint",
            "status": 400,
            "return": 2011,
            "req_data": {
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["INVALID"]
            }
        },
        {
            "name": "Check alias host is required constraint",
            "status": 400,
            "return": 2007,
            "req_data": {
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {}
                ]
            }
        },
        {
            "name": "Check alias host is valid hostname constraint",
            "status": 400,
            "return": 2046,
            "req_data": {
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {"host": "!INVALID!"}
                ]
            }
        },
        {
            "name": "Check domain is required constraint",
            "status": 400,
            "return": 2008,
            "req_data": {
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {"host": "pfsense-api-alias"}
                ]
            }
        },
        {
            "name": "Check domain is valid hostname constraint",
            "status": 400,
            "return": 2047,
            "req_data": {
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {"host": "pfsense-api-alias", "domain": "!INVALID!"}
                ]
            }
        },
    ]
    put_tests = [
        {
            "name": "Update IPv4 host override",
            "post_test_callable": "is_host_override_updated",
            "resp_time": 10,
            "req_data": {
                "apply": True,
                "id": 0,
                "host": HOST_UPDATE,
                "domain": DOMAIN_UPDATE,
                "ip": IP_UPDATE,
                "descr": "Updated E2E Test",
                "aliases": [
                    {
                        "host": ALIAS_HOST_UPDATE,
                        "domain": ALIAS_DOMAIN_UPDATE,
                        "description": "Updated E2E Test"
                    }
                ],
            }
        },
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 2015
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 2016,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Check host is valid hostname constraint",
            "status": 400,
            "return": 2046,
            "req_data": {
                "id": 0,
                "host": "!INVALID!"
            }
        },
        {
            "name": "Check domain is valid hostname constraint",
            "status": 400,
            "return": 2047,
            "req_data": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "!INVALID!"
            }
        },
        {
            "name": "Check IP is valid IPv4/IPv6 constraint",
            "status": 400,
            "return": 2011,
            "req_data": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["INVALID"]
            }
        },
        {
            "name": "Check alias host is required constraint",
            "status": 400,
            "return": 2007,
            "req_data": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {}
                ]
            }
        },
        {
            "name": "Check alias host is valid hostname constraint",
            "status": 400,
            "return": 2046,
            "req_data": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {"host": "!INVALID!"}
                ]
            }
        },
        {
            "name": "Check domain is required constraint",
            "status": 400,
            "return": 2008,
            "req_data": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {"host": "pfsense-api-alias"}
                ]
            }
        },
        {
            "name": "Check domain is valid hostname constraint",
            "status": 400,
            "return": 2047,
            "req_data": {
                "id": 0,
                "host": "pfsense-api",
                "domain": "example.com",
                "ip": ["127.0.0.1", "::1"],
                "aliases": [
                    {"host": "pfsense-api-alias", "domain": "!INVALID!"}
                ]
            }
        }
    ]
    delete_tests = [
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 2015
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 2016,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Delete host override",
            "post_test_callable": "is_host_override_deleted",
            "resp_time": 10,
            "req_data": {"id": 0, "apply": True}
        },
        {
            "name": "Remove access list",
            "method": "DELETE",
            "uri": "/api/v1/services/unbound/access_list",
            "resp_time": 10,
            "req_data": {"id": 0, "apply": True}
        }
    ]

    def is_host_override_created(self):
        """Checks if the host override was created and it is resolvable."""
        # Check that the host override created in our POST tests is resolvable
        if not self.is_dns_resolvable(HOST_CREATE, DOMAIN_CREATE, IP_CREATE):
            raise AssertionError(f"Expected {HOST_CREATE}.{DOMAIN_CREATE} to resolve {IP_CREATE}")

        # Check that the host override alias is also resolvable
        if not self.is_dns_resolvable(ALIAS_HOST_CREATE, ALIAS_DOMAIN_CREATE, IP_CREATE):
            raise AssertionError(f"Expected {ALIAS_HOST_CREATE}.{ALIAS_DOMAIN_CREATE} to resolve {IP_CREATE}")

    def is_host_override_updated(self):
        """Checks if the host override was updated and it is resolvable."""
        # Check that the host override created in our PUT tests is resolvable
        if not self.is_dns_resolvable(HOST_UPDATE, DOMAIN_UPDATE, IP_UPDATE):
            raise AssertionError(f"Expected {HOST_UPDATE}.{DOMAIN_UPDATE} to resolve {IP_UPDATE}")

        # Check that the host override alias is also resolvable
        if not self.is_dns_resolvable(ALIAS_HOST_UPDATE, ALIAS_DOMAIN_UPDATE, IP_UPDATE):
            raise AssertionError(f"Expected {ALIAS_HOST_UPDATE}.{ALIAS_DOMAIN_UPDATE} to resolve {IP_UPDATE}")

    def is_host_override_deleted(self):
        """Checks if the host override was deleted and it is no longer resolvable."""
        # Check that the host override created in our POST tests is not resolvable
        if self.is_dns_resolvable(HOST_CREATE, DOMAIN_CREATE, any=True):
            raise AssertionError(f"Expected {HOST_CREATE}.{DOMAIN_CREATE} to not resolve")

        # Check that the host override alias is also not resolvable
        if self.is_dns_resolvable(ALIAS_HOST_CREATE, ALIAS_DOMAIN_CREATE, any=True):
            raise AssertionError(f"Expected {ALIAS_HOST_CREATE}.{ALIAS_DOMAIN_CREATE} to not resolve")

        # Check that the host override created in our PUT tests is not resolvable
        if self.is_dns_resolvable(HOST_UPDATE, DOMAIN_UPDATE, any=True):
            raise AssertionError(f"Expected {HOST_UPDATE}.{DOMAIN_UPDATE} to not resolve")

        # Check that the host override alias is also not resolvable
        if self.is_dns_resolvable(ALIAS_HOST_UPDATE, ALIAS_DOMAIN_UPDATE, any=True):
            raise AssertionError(f"Expected {ALIAS_HOST_UPDATE}.{ALIAS_DOMAIN_UPDATE} to not resolve")

    def is_dns_resolvable(self, host, domain, ips=None, any=False):
        """Checks if a specified FQDN resolves a list of IPs"""
        # Local variables
        dig_cmd = ["dig", f"@{self.args.host}", f"{host}.{domain}", "a", f"{host}.{domain}", "aaaa"]
        dig_out = subprocess.check_output(dig_cmd).decode()

        # Ensure ips is a list
        ips = [] if ips is None else ips
        ips = [ips] if not isinstance(ips, list) else ips

        # Check if any successful resolution should be accepted
        if any:
            # Just ensure there was no error resolving (e.g. NXDOMAIN)
            if "status: NOERROR" in dig_out:
                return True
            else:
                return False

        # Otherwise, ensure each IP is present in the response
        for ip in ips:
            if ip not in dig_out:
                return False

        return True


APIE2ETestServicesUnboundHostOverride()
