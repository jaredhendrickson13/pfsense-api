# Copyright 2021 Jared Hendrickson
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

class APIUnitTestRoutingGateway(unit_test_framework.APIUnitTest):
    uri = "/api/v1/routing/gateway"
    get_tests = [{"name": "Read all routing gateways"}]
    post_tests = [
        {
            "name": "Create routing gateway",
            "payload": {
                "interface": "wan",
                "name": "API_UNIT_TEST_GATEWAY",
                "ipprotocol": "inet",
                "gateway": "172.16.209.1",
                "monitor": "172.16.209.250",
                "descr": "Unit test"
            }
        }
    ]
    put_tests = [
        {
            "name": "Update routing gateway",
            "payload": {
                "id": 0,
                "name": "UPDATED_UNIT_TEST_GATEWAY",
                "ipprotocol": "inet6",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "monitor": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "descr": "Updated Unit Test",
                "disabled": True,
                "action_disable": True,
                "monitor_disable": True,
                "weight": 2,
                "data_payload": 5,
                "latencylow": 300,
                "latencyhigh": 600,
                "interval": 2100,
                "loss_interval": 2500,
                "action_interval": 1040,
                "time_period": 66000,
                "losslow": 5,
                "losshigh": 10
            }
        }
    ]
    delete_tests = [
        {
            "name": "Delete routing gateway",
            "payload": {
                "id": 0
            },
            "resp_time": 5    # Allow a few seconds to safely remove the gateway and reload the route table
        }
    ]

APIUnitTestRoutingGateway()