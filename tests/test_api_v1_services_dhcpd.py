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

import unit_test_framework

class APIUnitTestServicesDHCPd(unit_test_framework.APIUnitTest):
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
                "maxleasetime": 61
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
    ]

APIUnitTestServicesDHCPd()
