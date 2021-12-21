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

class APIE2ETestFirewallRule(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/firewall/rule"
    get_tests = [
        {"name": "Read all firewall rules"}
    ]
    post_tests = [
        {
            "name": "Create a parent traffic shaper (altq) to use in rule",
            "uri": "/api/v1/firewall/traffic_shaper",
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
                "log": True,
                "top": True
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
            "payload": {
                "type": "INVALID"
            }
        },
        {
            "name": "Test interface requirement",
            "status": 400,
            "return": 4034,
            "payload": {
                "type": "pass"
            }
        },
        {
            "name": "Test interface validation",
            "status": 400,
            "return": 4034,
            "payload": {
                "type": "pass",
                "interface": "INVALID"
            }
        },
        {
            "name": "Test IP protocol requirement",
            "status": 400,
            "return": 4035,
            "payload": {
                "type": "pass",
                "interface": "wan"
            }
        },
        {
            "name": "Test IP protocol validation",
            "status": 400,
            "return": 4041,
            "payload": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "INVALID"
            }
        },
        {
            "name": "Test protocol requirement",
            "status": 400,
            "return": 4036,
            "payload": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "inet"
            }
        },
        {
            "name": "Test protocol validation",
            "status": 400,
            "return": 4042,
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
    ]
    put_tests = [
        {
            "name": "Update firewall rule",
            "payload": {
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
            "name": "Test tracker requirement",
            "status": 400,
            "return": 4031
        },
        {
            "name": "Test type validation",
            "status": 400,
            "return": 4039,
            "payload": {
                "type": "INVALID"
            }
        },
        {
            "name": "Test interface validation",
            "status": 400,
            "return": 4034,
            "payload": {
                "type": "pass",
                "interface": "INVALID"
            }
        },
        {
            "name": "Test IP protocol validation",
            "status": 400,
            "return": 4041,
            "payload": {
                "type": "pass",
                "interface": "wan",
                "ipprotocol": "INVALID"
            }
        },
        {
            "name": "Test protocol validation",
            "status": 400,
            "return": 4042,
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
    ]
    delete_tests = [
        {"name": "Delete firewall rule", "payload": {}},    # Tracker ID gets populated by post_post() method
        {
            "name": "Delete traffic shaper queue used to test",
            "uri": "/api/v1/firewall/traffic_shaper/queue",
            "payload": {"interface": "wan", "name": "Test_Altq"}
        },
        {
            "name": "Delete traffic shaper used to test",
            "uri": "/api/v1/firewall/traffic_shaper",
            "payload": {"interface": "wan"}
        },
        {
            "name": "Delete limiter queue used to test",
            "uri": "/api/v1/firewall/traffic_shaper/limiter/queue",
            "payload": {"limiter": "Test_Limiter", "name": "Test_DNQueue"}
        },
        {
            "name": "Delete limiter used to test",
            "uri": "/api/v1/firewall/traffic_shaper/limiter",
            "payload": {"name": "Test_Limiter"}
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
            "payload": {"tracker": "INVALID"}
        }
    ]

    # Override our PRE/POST methods
    def post_post(self):
        # We create a firewall rule in the 7th test, ensure we have run at least 7 tests
        if len(self.post_responses) == 7:
            # Assign the required tracker ID created in the POST request to the PUT and DELETE payloads
            self.delete_tests[0]["payload"]["tracker"] = self.post_responses[6]["data"]["tracker"]

            key = 0
            for value in self.put_tests:
                if "payload" in self.put_tests[key].keys():
                    self.put_tests[key]["payload"]["tracker"] = self.post_responses[6]["data"]["tracker"]
                key += 1

APIE2ETestFirewallRule()
