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
"""Script used to test the /api/v1/firewall/nat/port_forward endpoint."""
import e2e_test_framework


class APIE2ETestFirewallNATPortForward(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/nat/port_forward endpoint."""
    uri = "/api/v1/firewall/nat/port_forward"
    get_tests = [
        {"name": "Read all NAT port forwards"}
    ]
    post_tests = [
        {
            "name": "Create NAT port forward",
            "req_data": {
                "interface": "WAN",
                "protocol": "tcp",
                "src": "any",
                "srcport": "433",
                "dst": "wanip",
                "dstport": "443",
                "target": "192.168.1.123",
                "local-port": "443",
                "natreflection": "purenat",
                "descr": "E2E Test",
                "nosync": True,
                "top": True
            }
        },
        {
            "name": "Check interface required constraint",
            "status": 400,
            "return": 4000
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 4006,
            "req_data": {"interface": "INVALID"}
        },
        {
            "name": "Check protocol required constraint",
            "status": 400,
            "return": 4001,
            "req_data": {"interface": "wan"}
        },
        {
            "name": "Check protocol option constraint",
            "status": 400,
            "return": 4007,
            "req_data": {"interface": "wan", "protocol": "INVALID"}
        },
        {
            "name": "Check target required constraint",
            "status": 400,
            "return": 4002,
            "req_data": {"interface": "wan", "protocol": "tcp"}
        },
        {
            "name": "Check target IPv4 or alias constraint",
            "status": 400,
            "return": 4009,
            "req_data": {"interface": "wan", "protocol": "tcp", "target": "INVALID"}
        },
        {
            "name": "Check local port required when protocol is port-based constraint",
            "status": 400,
            "return": 4003,
            "req_data": {"interface": "wan", "protocol": "tcp", "target": "1.2.3.4"}
        },
        {
            "name": "Check local port is port or alias constraint",
            "status": 400,
            "return": 4010,
            "req_data": {"interface": "wan", "protocol": "tcp", "target": "1.2.3.4", "local-port": "INVALID"}
        },
        {
            "name": "Check source required constraint",
            "status": 400,
            "return": 4004,
            "req_data": {"interface": "wan", "protocol": "tcp", "target": "1.2.3.4", "local-port": 443}
        },
        {
            "name": "Check source is valid IP constraint",
            "status": 400,
            "return": 4011,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "INVALID"
            }
        },
        {
            "name": "Check destination required constraint",
            "status": 400,
            "return": 4005,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1"
            }
        },
        {
            "name": "Check destination is valid IP constraint",
            "status": 400,
            "return": 4012,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "INVALID"
            }
        },
        {
            "name": "Check srcport is valid port, range or alias constraint",
            "status": 400,
            "return": 4013,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "4.2.3.1",
                "srcport": "INVALID"
            }
        },
        {
            "name": "Check dstport is valid port, range or alias constraint",
            "status": 400,
            "return": 4014,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "4.2.3.1",
                "srcport": "443:445",
                "dstport": "INVALID"
            }
        },
        {
            "name": "Check natreflection options constraint",
            "status": 400,
            "return": 4008,
            "req_data": {
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "4.2.3.1",
                "srcport": "443:445",
                "dstport": 443,
                "natreflection": "INVALID"
            }
        },
    ]
    put_tests = [
        {
            "name": "Update NAT port forward and apply",
            "req_data": {
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
                "descr": "Updated E2E Test",
                "nosync": False,
                "nordr": True,
                "disabled": True,
                "top": True,
                "apply": True
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 4015
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 4016,
            "req_data": {"id": "INVALID"}
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 4006,
            "req_data": {"id": 0, "interface": "INVALID"}
        },
        {
            "name": "Check protocol option constraint",
            "status": 400,
            "return": 4007,
            "req_data": {"id": 0, "interface": "wan", "protocol": "INVALID"}
        },
        {
            "name": "Check target IPv4 or alias constraint",
            "status": 400,
            "return": 4009,
            "req_data": {"id": 0, "interface": "wan", "protocol": "tcp", "target": "INVALID"}
        },
        {
            "name": "Check local port is port or alias constraint",
            "status": 400,
            "return": 4010,
            "req_data": {"id": 0, "interface": "wan", "protocol": "tcp", "target": "1.2.3.4", "local-port": "INVALID"}
        },
        {
            "name": "Check source is valid IP constraint",
            "status": 400,
            "return": 4011,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "INVALID"
            }
        },
        {
            "name": "Check destination is valid IP constraint",
            "status": 400,
            "return": 4012,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "INVALID"
            }
        },
        {
            "name": "Check srcport is valid port, range or alias constraint",
            "status": 400,
            "return": 4013,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "4.2.3.1",
                "srcport": "INVALID"
            }
        },
        {
            "name": "Check dstport is valid port, range or alias constraint",
            "status": 400,
            "return": 4014,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "4.2.3.1",
                "srcport": "443:445",
                "dstport": "INVALID"
            }
        },
        {
            "name": "Check natreflection options constraint",
            "status": 400,
            "return": 4008,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "protocol": "tcp",
                "target": "1.2.3.4",
                "local-port": 443,
                "src": "4.3.2.1",
                "dst": "4.2.3.1",
                "srcport": 443,
                "dstport": "443:445",
                "natreflection": "INVALID"
            }
        },
    ]
    delete_tests = [
        {
            "name": "Check ID required constraint",
            "status": 400,
            "return": 4015
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 4016,
            "req_data": {"id": "INVALID"}
        },
        {"name": "Delete NAT port forward", "req_data": {"id": 0}}
    ]


APIE2ETestFirewallNATPortForward()
