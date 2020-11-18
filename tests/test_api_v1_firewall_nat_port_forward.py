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

class APIUnitTestFirewallNATPortForward(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/nat/port_forward"
    get_payloads = [{}]
    post_payloads = [
        {
            "interface": "WAN",
            "protocol": "tcp",
            "src": "any",
            "srcport": "433",
            "dst": "wanip",
            "dstport": "443",
            "target": "192.168.1.123",
            "local-port": "443",
            "natreflection": "purenat",
            "descr": "Unit Test",
            "nosync": True,
            "top": True
        }
    ]
    put_payloads = [
        {
            "id": 0,
            "interface": "WAN",
            "protocol": "tcp",
            "src": "!1.1.1.1/24",
            "srcport": "any",
            "dst": "wanip",
            "dstport": 80,
            "target": "192.168.1.1",
            "local-port": "80",
            "natreflection": "enable",
            "descr": "Updated Unit Test",
            "nosync": False,
            "nordr": True,
            "disabled": True,
            "top": True,
            "apply": True
        }
    ]
    delete_payloads = [
        {"id": 0}
    ]

APIUnitTestFirewallNATPortForward()