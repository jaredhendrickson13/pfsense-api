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
"""Script used to test the /api/v1/firewall/rule endpoint."""
import os
import time

import e2e_test_framework


class APIE2ETestFirewallRule(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/rule endpoint."""
    uri = "/api/v1/firewall/rule"
    get_tests = [
        {"name": "Read all firewall rules"}
    ]
    post_tests = [
        {
            "name": "Create a parent traffic shaper (altq) to use in rule",
            "uri": "/api/v1/firewall/traffic_shaper",
            "req_data": {
                "interface": "wan",
                "scheduler": "PRIQ",
                "bandwidthtype": "Gb",
                "bandwidth": 1,
                "enabled": True,
                "qlimit": 1000,
                "tbrconfig": 1000,
                "apply": True
            }
        },
        {
            "name": "Create another traffic shaper queue (altq) to use in the rule",
            "uri": "/api/v1/firewall/traffic_shaper/queue",
            "req_data": {
                "interface": "wan",
                "name": "Test_Altq",
                "priority": 14,
                "description": "Traffic Shaper Queue E2E test",
                "default": True
            }
        },
        {
            "name": "Create traffic shaper queue (altq) to use in the rule",
            "uri": "/api/v1/firewall/traffic_shaper/queue",
            "req_data": {
                "interface": "wan",
                "name": "Test_Altq2",
                "priority": 15,
                "description": "Traffic Shaper Queue E2E test",
                "default": False
            }
        },
        {
            "name": "Create parent firewall traffic shaper limiter (dummynet) to use in rule",
            "uri": "/api/v1/firewall/traffic_shaper/limiter",
            "req_data": {
                "name": "Test_Limiter",
                "bandwidth": [{"bw": 100, "bwscale": "Mb"}],
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "E2E test",
                "aqm": "codel",
                "sched": "fq_pie",
                "qlimit": 7000,
                "delay": 1,
                "plr": 0.01,
                "buckets": 16,
                "ecn": True,
                "apply": True
            }
        },
        {
            "name": "Create firewall traffic shaper limiter queue (dummynet) to use in rule",
            "uri": "/api/v1/firewall/traffic_shaper/limiter/queue",
            "req_data": {
                "limiter": "Test_Limiter",
                "name": "Test_DNQueue",
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "E2E test",
                "aqm": "codel",
                "qlimit": 7000,
                "weight": 1,
                "plr": 0.01,
                "buckets": 16,
                "ecn": True,
                "apply": True
            }
        },
        {
            "name": "Create another firewall traffic shaper limiter queue (dummynet) to use in rule",
            "uri": "/api/v1/firewall/traffic_shaper/limiter/queue",
            "req_data": {
                "limiter": "Test_Limiter",
                "name": "Test_DNQueue2",
                "mask": "srcaddress",
                "maskbits": 31,
                "description": "E2E test",
                "aqm": "codel",
                "qlimit": 7000,
                "weight": 1,
                "plr": 0.01,
                "buckets": 16,
                "ecn": True,
                "apply": True
            }
        },
        {
            "name": "Create firewall rule",
            "req_data": {
                "type": "block",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "172.16.77.121",
                "srcport": "any",
                "dst": "127.0.0.1",
                "dstport": "443",
                "descr": "E2E test",
                "dnpipe": "Test_DNQueue",
                "pdnpipe": "Test_DNQueue2",
                "defaultqueue": "Test_Altq",
                "ackqueue": "Test_Altq2",
                "gateway": "",
                "log": True,
                "top": True
            },
            "resp_time": 3    # Accommodate the mandatory 1 second delay for firewall rule creations
        },
        {
            "name": "Block ping and ensure block actually works",
            "post_test_callable": "is_ping_unsuccessful",
            "req_data": {
                "type": "block",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "icmp",
                "src": "any",
                "dst": "any",
                "descr": "E2E block ping test",
                "top": True,
                "apply": True
            },
            "resp_time": 3  # Accommodate the mandatory 1 second delay for firewall rule creations
        },
        {
            "name": "Create floating firewall rule",
            "req_data": {
                "type": "block",
                "interface": "wan,lan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "172.16.77.121",
                "srcport": "any",
                "dst": "127.0.0.1",
                "dstport": "443",
                "descr": "Unit test",
                "floating": True
            },
            "resp_time": 3    # Accommodate the mandatory 1 second delay for firewall rule creations
        },
        {
            "name": "Test type requirement",
            "status": 400,
            "return": 4033
        },
        {
            "name": "Test type validation",
            "status": 400,
            "return": 4039,
            "req_data": {
                "type": "INVALID"
            }
        },
        {
            "name": "Test interface requirement",
            "status": 400,
            "return": 4034,
            "req_data": {
                "type": "pass"
            }
        },
        {
            "name": "Test interface validation",
            "status": 400,
            "return": 4040,
            "req_data": {
                "type": "pass",
                "interface": "INVALID"
            }
        },
        {
            "name": "Test IP protocol requirement",
            "status": 400,
            "return": 4035,
            "req_data": {
                "type": "pass",
                "interface": "wan"
            }
        },
        {
            "name": "Test IP protocol validation",
            "status": 400,
            "return": 4041,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "INVALID"
            }
        },
        {
            "name": "Test protocol requirement",
            "status": 400,
            "return": 4036,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet"
            }
        },
        {
            "name": "Test protocol validation",
            "status": 400,
            "return": 4042,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "INVALID"
            }
        },
        {
            "name": "Test ICMP type validation",
            "status": 400,
            "return": 4046,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "icmp",
                "icmptype": ["INVALID"]
            }
        },
        {
            "name": "Test source requirement",
            "status": 400,
            "return": 4037,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "any"
            }
        },
        {
            "name": "Test source validation",
            "status": 400,
            "return": 4044,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "any",
                "src": "INVALID"
            }
        },
        {
            "name": "Test destination requirement",
            "status": 400,
            "return": 4038,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "any",
                "src": "any"
            }
        },
        {
            "name": "Test destination validation",
            "status": 400,
            "return": 4045,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "any",
                "src": "any",
                "dst": "INVALID"
            }
        },
        {
            "name": "Test source port requirement",
            "status": 400,
            "return": 4047,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any"
            }
        },
        {
            "name": "Test source port validation",
            "status": 400,
            "return": 4048,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "INVALID"
            }
        },
        {
            "name": "Test destination port requirement",
            "status": 400,
            "return": 4047,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
            }
        },
        {
            "name": "Test destination port validation",
            "status": 400,
            "return": 4049,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "INVALID"
            }
        },
        {
            "name": "Test gateway validation",
            "status": 400,
            "return": 4043,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "gateway": "INVALID"
            }
        },
        {
            "name": "Test schedule validation",
            "status": 400,
            "return": 4150,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "sched": "INVALID"
            }
        },
        {
            "name": "Test dnpipe validation",
            "status": 400,
            "return": 4222,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "dnpipe": "INVALID"
            }
        },
        {
            "name": "Test dnpipe requirement when pdnpipe provided",
            "status": 400,
            "return": 4223,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "pdnpipe": "INVALID"
            }
        },
        {
            "name": "Test dnpipe and pdnpipe cannot match",
            "status": 400,
            "return": 4224,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "dnpipe": "Test_DNQueue",
                "pdnpipe": "Test_DNQueue"
            }
        },
        {
            "name": "Test dnpipe and pdnpipe type requirements",
            "status": 400,
            "return": 4225,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "dnpipe": "Test_Limiter",
                "pdnpipe": "Test_DNQueue"
            }
        },
        {
            "name": "Test unknown default queue",
            "status": 400,
            "return": 4226,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "defaultqueue": "INVALID"
            }
        },
        {
            "name": "Test default queue requirement when ackqueue is provided",
            "status": 400,
            "return": 4227,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "ackqueue": "INVALID"
            }
        },
        {
            "name": "Test unknown ackqueue",
            "status": 400,
            "return": 4228,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "defaultqueue": "Test_Altq",
                "ackqueue": "INVALID"
            }
        },
        {
            "name": "Test default queue and ackqueue cannot match",
            "status": 400,
            "return": 4229,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "defaultqueue": "Test_Altq",
                "ackqueue": "Test_Altq"
            }
        },
        {
            "name": "Test unknown floating direction",
            "status": 400,
            "return": 4239,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "floating": True,
                "direction": "INVALID"
            }
        },
        {
            "name": "Check statetype options constraint",
            "status": 400,
            "return": 4243,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "statetype": "INVALID"
            }
        },
        {
            "name": "Check protocol must be 'tcp' when statetype is 'synproxy state' constraint",
            "status": 400,
            "return": 4244,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["syn"],
                "statetype": "synproxy state"
            }
        },
        {
            "name": "Check gateway must be default when statetype is 'synproxy state' constraint",
            "status": 400,
            "return": 4245,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": "syn,fin",
                "statetype": "synproxy state",
                "gateway": "WAN_DHCP"
            }
        },
        {
            "name": "Check tcpflags2 (out of) options constraint",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["INVALID"]
            }
        },
        {
            "name": "Check tcpflags2 (out of) options constraint (CSV)",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": "syn,INVALID"
            }
        },
        {
            "name": "Check tcpflags1 (set) options constraint",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["syn"],
                "tcpflags1": ["INVALID"]
            }
        },
        {
            "name": "Check tcpflags1 (set) options constraint (CSV)",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["syn"],
                "tcpflags1": ["INVALID"]
            }
        },
        {
            "name": "Check tcpflags1 (set) must be in tcpflags2 (out of) constraint",
            "status": 400,
            "return": 4247,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["syn"],
                "tcpflags1": ["fin"]
            }
        },
        {
            "name": "Check non-floating rules must have 1 interface constraint",
            "status": 400,
            "return": 4248,
            "req_data": {
                "type": "pass",
                "interface": ["wan", "lan"],
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any"
            }
        }
    ]
    put_tests = [
        {
            "name": "Update firewall rule",
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "172.16.77.125",
                "srcport": "8080-8081",
                "dst": "(self)",
                "dstport": "2222-4444",
                "descr": "Updated E2E test",
                "gateway": "WAN_DHCP",
                "log": False,
                "top": True,
                "apply": True
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Update floating firewall rule",
            "req_data": {
                "type": "block",
                "floating": True,
                "interface": ["wan", "lan"],
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "172.16.77.121",
                "srcport": "any",
                "dst": "127.0.0.1",
                "dstport": "443",
                "descr": "Unit test",
                "direction": "out"
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Test ability to revert the gateway field to default",
            "req_data": {
                "gateway": "default",
                "apply": True
            },
            "resp_time": 3    # Allow a few seconds for the firewall filter to reload
        },
        {
            "name": "Test tracker requirement",
            "status": 400,
            "return": 4031
        },
        {
            "name": "Test type validation",
            "status": 400,
            "return": 4039,
            "req_data": {
                "type": "INVALID"
            }
        },
        {
            "name": "Test interface validation",
            "status": 400,
            "return": 4040,
            "req_data": {
                "type": "pass",
                "interface": "INVALID"
            }
        },
        {
            "name": "Test IP protocol validation",
            "status": 400,
            "return": 4041,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "INVALID"
            }
        },
        {
            "name": "Test protocol validation",
            "status": 400,
            "return": 4042,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "INVALID"
            }
        },
        {
            "name": "Test ICMP type validation",
            "status": 400,
            "return": 4046,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "icmp",
                "icmptype": ["INVALID"]
            }
        },
        {
            "name": "Test source validation",
            "status": 400,
            "return": 4044,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "any",
                "src": "INVALID"
            }
        },
        {
            "name": "Test destination validation",
            "status": 400,
            "return": 4045,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "any",
                "src": "any",
                "dst": "INVALID"
            }
        },
        {
            "name": "Test source port validation",
            "status": 400,
            "return": 4048,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "INVALID"
            }
        },
        {
            "name": "Test destination port validation",
            "status": 400,
            "return": 4049,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "INVALID"
            }
        },
        {
            "name": "Test gateway validation",
            "status": 400,
            "return": 4043,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "gateway": "INVALID"
            }
        },
        {
            "name": "Test schedule validation",
            "status": 400,
            "return": 4150,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "sched": "INVALID"
            }
        },
        {
            "name": "Test unknown floating direction",
            "status": 400,
            "return": 4239,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "direction": "Test_Direction"
            }
        },
        {
            "name": "Check statetype options constraint",
            "status": 400,
            "return": 4243,
             "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "statetype": "INVALID"
            }
        },
        {
            "name": "Check protocol must be 'tcp' when statetype is 'synproxy state' constraint",
            "status": 400,
            "return": 4244,
             "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "statetype": "synproxy state"
            }
        },
        {
            "name": "Check gateway must be default when statetype is 'synproxy state' constraint",
            "status": 400,
            "return": 4245,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "statetype": "synproxy state",
                "gateway": "WAN_DHCP"
            }
        },
        {
            "name": "Check tcpflags2 (out of) options constraint",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["INVALID"]
            }
        },
        {
            "name": "Check tcpflags2 (out of) options constraint (CSV)",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": "syn,INVALID"
            }
        },
        {
            "name": "Check tcpflags1 (set) options constraint",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["syn"],
                "tcpflags1": ["INVALID"]
            }
        },
        {
            "name": "Check tcpflags1 (set) options constraint (CSV)",
            "status": 400,
            "return": 4246,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["syn"],
                "tcpflags1": ["INVALID"]
            }
        },
        {
            "name": "Check tcpflags1 (set) must be in tcpflags2 (out of) constraint",
            "status": 400,
            "return": 4247,
            "req_data": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "tcp/udp",
                "src": "any",
                "dst": "any",
                "srcport": "any",
                "dstport": "any",
                "tcpflags2": ["syn"],
                "tcpflags1": ["fin"]
            }
        }
    ]
    delete_tests = [
        {"name": "Delete firewall rule", "req_data": {}},    # Tracker ID gets populated by post_post() method
        {
            # Tracker ID gets populated by post_post() method
            "name": "Delete ICMP test firewall rule",
            "post_test_callable": "is_ping_successful",
            "req_data": {"apply": True},
            "resp_time": 3
        },
        {"name": "Delete floating firewall rule", "req_data": {}},    # Tracker ID gets populated by post_post() method
        {
            "name": "Delete traffic shaper queue used to test",
            "uri": "/api/v1/firewall/traffic_shaper/queue",
            "req_data": {"interface": "wan", "name": "Test_Altq"}
        },
        {
            "name": "Delete traffic shaper used to test",
            "uri": "/api/v1/firewall/traffic_shaper",
            "req_data": {"interface": "wan"}
        },
        {
            "name": "Delete limiter queue used to test",
            "uri": "/api/v1/firewall/traffic_shaper/limiter/queue",
            "req_data": {"limiter": "Test_Limiter", "name": "Test_DNQueue"}
        },
        {
            "name": "Delete limiter used to test",
            "uri": "/api/v1/firewall/traffic_shaper/limiter",
            "req_data": {"name": "Test_Limiter"}
        },
        {
            "name": "Test tracker requirement",
            "status": 400,
            "return": 4031
        },
        {
            "name": "Test delete non-existing rule",
            "status": 400,
            "return": 4032,
            "req_data": {"tracker": "INVALID"}
        }
    ]

    def is_ping_unsuccessful(self):
        """Checks that we can't ping our target after a block rule is put in place"""
        # Give the filter a moment to reload
        time.sleep(3)

        # Send a single ping and ensure the target host does not respond
        if os.system(f"ping -c 1 -t 1 {self.args.host} > /dev/null") == 0:
            raise AssertionError("Expected ping to be unsuccessful after block rule is applied")

    def is_ping_successful(self):
        """Checks that we can ping our target after a block rule is removed"""
        # Give the filter a moment to reload
        time.sleep(3)

        # Send a single ping and ensure the target host does not respond
        if os.system(f"ping -c 1 -t 1 {self.args.host} > /dev/null") != 0:
            raise AssertionError("Expected ping to be successful after block rule is removed")

    # Override our PRE/POST methods
    def post_post(self):
        # After we've created rules in our tests, ensure the tracker is added to PUT and DELETE tests
        if len(self.post_responses) == 9:
            # Assign the required tracker ID created in the POST request to the PUT and DELETE req_datas
            self.delete_tests[0]["req_data"]["tracker"] = self.post_responses[6]["data"]["tracker"]
            self.delete_tests[1]["req_data"]["tracker"] = self.post_responses[7]["data"]["tracker"]
            self.delete_tests[2]["req_data"]["tracker"] = self.post_responses[8]["data"]["tracker"]

            key = 0
            for _ in self.put_tests:
                if "req_data" in self.put_tests[key]:
                    self.put_tests[key]["req_data"]["tracker"] = self.post_responses[6]["data"]["tracker"]
                    self.put_tests[key]["req_data"]["tracker"] = self.post_responses[8]["data"]["tracker"]
                key += 1


APIE2ETestFirewallRule()
