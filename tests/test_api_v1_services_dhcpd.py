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
"""Script used to test the /api/v1/services/dhcpd endpoint."""
import e2e_test_framework


class APIE2ETestServicesDHCPd(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/dhcpd endpoint."""
    uri = "/api/v1/services/dhcpd"
    get_tests = [{"name": "Read all DHCPd configurations"}]
    put_tests = [
        {
            "name": "Update LAN interface's DHCP configuration",
            "payload": {
                "interface": "lan",
                "enable": True,
                "ignorebootp": False,
                "denyunknown": False,
                "range_from": "192.168.1.1",
                "range_to": "192.168.1.100",
                "dnsserver": ["1.1.1.1"],
                "gateway": "192.168.1.1",
                "domainsearchlist": ["pfsense-api.jaredhendrickson.com", "pfsense-api.jh.co"],
                "domain": "pfsense-api.jaredhendrickson.com",
                "mac_allow": ["00:00:00:01:E5:FF", "00:00:00:01:E5"],
                "mac_deny": [],
                "defaultleasetime": 60,
                "maxleasetime": 61,
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "string",
                        "value": "\"test\""
                    }
                ]

            },
            "resp_time": 5    # Allow a few seconds to reload the DHCP service
        },
        {
            "status": 400,
            "return": 2017,
            "name": "Check interface required constraint"
        },
        {
            "status": 400,
            "return": 2018,
            "name": "Check interface exists constraint",
            "payload": {
                "interface": "INVALID"
            }
        },
        {
            "status": 400,
            "return": 2021,
            "name": "Check range_from IPv4 constraint",
            "payload": {
                "interface": "lan",
                "range_from": "INVALID"
            }
        },
        {
            "status": 400,
            "return": 2022,
            "name": "Check range_from available IPv4 constraint",
            "payload": {
                "interface": "lan",
                "range_from": "172.16.1.1"
            }
        },
        {
            "status": 400,
            "return": 2024,
            "name": "Check range_to IPv4 constraint",
            "payload": {
                "interface": "lan",
                "range_to": "INVALID"
            }
        },
        {
            "status": 400,
            "return": 2025,
            "name": "Check range_to available IPv4 constraint",
            "payload": {
                "interface": "lan",
                "range_to": "172.16.1.1"
            }
        },
        {
            "status": 400,
            "return": 2030,
            "name": "Check domain valid hostname constraint",
            "payload": {
                "interface": "lan",
                "domain": False
            }
        },
        {
            "status": 400,
            "return": 2032,
            "name": "Check mac_allow valid MAC constraint",
            "payload": {
                "interface": "lan",
                "mac_allow": "INVALID"
            }
        },
        {
            "status": 400,
            "return": 2033,
            "name": "Check mac_deny valid MAC constraint",
            "payload": {
                "interface": "lan",
                "mac_deny": "INVALID"
            }
        },
        {
            "status": 400,
            "return": 2031,
            "name": "Check domainsearchlist valid hostname constraint",
            "payload": {
                "interface": "lan",
                "domainsearchlist": False
            }
        },
        {
            "status": 400,
            "return": 2080,
            "name": "Check defaultleasetime minimum constraint",
            "payload": {
                "interface": "lan",
                "defaultleasetime": 59
            }
        },
        {
            "status": 400,
            "return": 2081,
            "name": "Check defaultleasetime less than maxleasetime constraint",
            "payload": {
                "interface": "lan",
                "defaultleasetime": 61,
                "maxleasetime": 60
            }
        },
        {
            "status": 400,
            "return": 2082,
            "name": "Check maxleasetime minimum constraint",
            "payload": {
                "interface": "lan",
                "maxleasetime": 59
            }
        },
        {
            "status": 400,
            "return": 2028,
            "name": "Check gateway IPv4 constraint",
            "payload": {
                "interface": "lan",
                "gateway": "INVALID"
            }
        },
        {
            "status": 400,
            "return": 2029,
            "name": "Check gateway in subnet constraint",
            "payload": {
                "interface": "lan",
                "gateway": "172.16.1.1"
            }
        },
        {
            "status": 400,
            "return": 2026,
            "name": "Check dnsserver maximum items constraint",
            "payload": {
                "interface": "lan",
                "dnsserver": ["127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1"]
            }
        },
        {
            "status": 400,
            "return": 2027,
            "name": "Check dnsserver IPv4 constraint",
            "payload": {
                "interface": "lan",
                "dnsserver": ["INVALID"]
            }
        },
        {
            "status": 400,
            "return": 2083,
            "name": "Check numberoptions number requirement",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {}
                ]
            }
        },
        {
            "status": 400,
            "return": 2084,
            "name": "Check numberoptions number minimum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 0
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2084,
            "name": "Check numberoptions number maximum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 255
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2085,
            "name": "Check numberoptions type requirement",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2086,
            "name": "Check numberoptions type choice constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "INVALID"
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2087,
            "name": "Check numberoptions value requirement",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "text"
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2088,
            "name": "Check numberoptions text value quotation constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "text",
                        "value": "\"INVALID\""
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2089,
            "name": "Check numberoptions string value quotation required constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "string",
                        "value": "INVALID"
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2090,
            "name": "Check numberoptions boolean value choice constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "boolean",
                        "value": "INVALID"
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2091,
            "name": "Check numberoptions unsigned integer 8 value minimum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "unsigned integer 8",
                        "value": -1
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2091,
            "name": "Check numberoptions unsigned integer 8 value maximum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "unsigned integer 8",
                        "value": 256
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2092,
            "name": "Check numberoptions unsigned integer 16 value minimum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "unsigned integer 16",
                        "value": -1
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2092,
            "name": "Check numberoptions unsigned integer 16 value maximum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "unsigned integer 16",
                        "value": 65536
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2093,
            "name": "Check numberoptions unsigned integer 32 value minimum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "unsigned integer 32",
                        "value": -1
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2093,
            "name": "Check numberoptions unsigned integer 32 value maximum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "unsigned integer 32",
                        "value": 4294967296
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2094,
            "name": "Check numberoptions signed integer 8 value minimum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "signed integer 8",
                        "value": -129
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2094,
            "name": "Check numberoptions signed integer 8 value maximum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "signed integer 8",
                        "value": 128
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2095,
            "name": "Check numberoptions signed integer 16 value minimum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "signed integer 16",
                        "value": -32769
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2095,
            "name": "Check numberoptions signed integer 16 value maximum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "signed integer 16",
                        "value": 32768
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2096,
            "name": "Check numberoptions signed integer 32 value minimum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "signed integer 32",
                        "value": -2147483649
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2096,
            "name": "Check numberoptions signed integer 32 value maximum constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "signed integer 32",
                        "value": 2147483648
                    }
                ]
            }
        },
        {
            "status": 400,
            "return": 2097,
            "name": "Check numberoptions ip-address value IPv4/hostname constraint",
            "payload": {
                "interface": "lan",
                "numberoptions": [
                    {
                        "number": 55,
                        "type": "ip-address",
                        "value": False
                    }
                ]
            }
        },
    ]


APIE2ETestServicesDHCPd()
