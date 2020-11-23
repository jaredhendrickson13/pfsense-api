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

class APIUnitTestFirewallNATOutboundMapping(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/nat/outbound/mapping"
    get_payloads = [{}]
    post_payloads = [
        {
            "interface": "WAN",
            "protocol": "tcp",
            "src": "any",
            "srcport": 434,
            "dst": "1.1.1.1",
            "dstport": 443,
            "target": "192.168.1.123/24",
            "natport": 443,
            "poolopts": "round-robin",
            "descr": "Unit Test",
            "nosync": True,
            "top": True
        }
    ]
    put_payloads = [
        {
            "id": 0,
            "interface": "WAN",
            "protocol": "udp",
            "src": "any",
            "srcport": "433",
            "dst": "1.1.1.1",
            "dstport": "443",
            "target": "192.168.1.123/24",
            "natstaticport": True,
            "poolopts": "round-robin",
            "descr": "Updated Unit Test",
            "nonat": True,
            "disabled": True,
            "nosync": True,
            "top": True
        }
    ]
    delete_payloads = [
        {"id": 0}
    ]

APIUnitTestFirewallNATOutboundMapping()