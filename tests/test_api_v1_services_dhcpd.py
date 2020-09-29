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

class APIUnitTestServicesDHCPd(unit_test_framework.APIUnitTest):
    url = "/api/v1/services/dhcpd"
    get_payloads = [{}]
    put_payloads = [
        {
            "interface": "lan",
            "enable": True,
            "ignorebootp": False,
            "denyunknown": False,
            "range_from": "192.168.1.25",
            "range_to": "192.168.1.50",
            "dnsserver": ["1.1.1.1"],
            "gateway": "192.168.1.2",
            "domainsearchlist": ["pfsense-api.jaredhendrickson.com", "pfsense-api.jh.co"],
            "domain": "pfsense-api.jaredhendrickson.com",
            "mac_allow": ["00:00:00:01:E5:FF", "00:00:00:01:E5"],
            "mac_deny": []
        }
    ]

APIUnitTestServicesDHCPd()