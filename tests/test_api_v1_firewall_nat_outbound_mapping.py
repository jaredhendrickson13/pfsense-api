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

import e2e_test_framework

class APIE2ETestFirewallNATOutboundMapping(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/firewall/nat/outbound/mapping"
    get_tests = [
        {"name": "Read all outbound NAT mappings"}
    ]
    post_tests = [
        {
            "name": "Create port-based outbound NAT mapping",
            "payload": {
                "interface": "WAN",
                "protocol": "tcp",
                "src": "any",
                "srcport": 434,
                "dst": "1.1.1.1",
                "dstport": 443,
                "target": "192.168.1.123/24",
                "natport": 443,
                "poolopts": "round-robin",
                "descr": "E2E Test",
                "nosync": True,
                "top": True
            }
        },
        {
            "name": "Create non-port-based outbound NAT mapping",
            "payload": {
                "interface": "WAN",
                "protocol": "any",
                "src": "any",
                "dst": "1.1.1.1",
                "target": "192.168.1.123/24",
                "poolopts": "round-robin",
                "descr": "E2E Test 2",
                "nosync": True,
                "top": True
            }
        }
    ]
    put_tests = [
        {
            "name": "Update port-based outbound NAT mapping to non-port-based",
            "payload": {
                "id": 0,
                "interface": "WAN",
                "protocol": "any",
                "src": "any",
                "dst": "1.1.1.1",
                "target": "192.168.1.123/24",
                "poolopts": "round-robin",
                "descr": "Updated E2E Test",
                "nonat": True,
                "disabled": True,
                "nosync": True,
                "top": True
            }
        },
        {
            "name": "Update non-port-based outbound NAT mapping to port-based",
            "payload": {
                "id": 1,
                "interface": "WAN",
                "protocol": "udp",
                "src": "any",
                "srcport": "433",
                "dst": "1.1.1.1",
                "dstport": "443",
                "target": "192.168.1.123/24",
                "staticnatport": True,
                "poolopts": "round-robin",
                "descr": "Updated E2E Test",
                "nonat": False,
                "disabled": True,
                "nosync": True,
                "top": True
            }
        }
    ]
    delete_tests = [
        {"name": "Delete non-port-based outbound NAT mapping", "payload": {"id": 0}},
        {"name": "Delete port-based outbound NAT mapping", "payload": {"id": 0}},
    ]

APIE2ETestFirewallNATOutboundMapping()
