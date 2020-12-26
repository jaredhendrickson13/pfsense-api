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

class APIUnitTestFirewallRule(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/rule"
    get_payloads = [{}]
    post_payloads = [
        {
            "type": "block",
            "interface": "wan",
            "ipprotocol": "inet",
            "protocol": "tcp/udp",
            "src": "172.16.77.121",
            "srcport": "any",
            "dst": "127.0.0.1",
            "dstport": "443",
            "descr": "Unit test",
            "top": True
        }
    ]
    put_payloads = [
        {
            "type": "pass",
            "interface": "wan",
            "ipprotocol": "inet",
            "protocol": "tcp/udp",
            "src": "172.16.77.125",
            "srcport": "HTTP",
            "dst": "127.0.0.1",
            "dstport": "HTTP",
            "descr": "Updated Unit test",
            "top": True
        }
    ]
    delete_payloads = [{"id": 0}]

    # Override our PRE/POST methods
    def post_post(self):
        # Assign the required tracker ID created in the POST request to the PUT and DELETE payloads
        try:
            self.put_payloads[0]["tracker"] = self.post_responses[0]["data"]["tracker"]
            self.delete_payloads[0]["tracker"] = self.post_responses[0]["data"]["tracker"]
        except IndexError:
            pass
        except KeyError:
            pass

APIUnitTestFirewallRule()