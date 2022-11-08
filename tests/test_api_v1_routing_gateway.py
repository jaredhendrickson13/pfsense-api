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
"""Script used to test the /api/v1/routing/gateway endpoint."""
import re

import e2e_test_framework


class APIE2ETestRoutingGateway(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/routing/gateway endpoint."""
    uri = "/api/v1/routing/gateway"

    get_privileges = ["page-all", "page-system-gateways"]
    post_privileges = ["page-all", "page-system-gateways-editgateway"]
    put_privileges = ["page-all", "page-system-gateways-editgateway"]
    delete_privileges = ["page-all", "page-system-gateways-editgateway"]

    get_tests = [{"name": "Read all routing gateways"}]
    post_tests = [
        {
            "name": "Create IPv4 routing gateway",
            "req_data": {
                "interface": "wan",
                "name": "TEST_ROUTING_GATEWAY_V4",
                "ipprotocol": "inet",
                "gateway": "172.16.209.1",
                "monitor": "172.16.209.250",
                "descr": "E2E test"
            }
        },
        {
            "name": "Create IPv6 routing gateway",
            "req_data": {
                "interface": "wan",
                "name": "TEST_ROUTING_GATEWAY_V6",
                "ipprotocol": "inet6",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7332",
                "descr": "E2E test"
            }
        },
        {
            "name": "Create a IPv4 static route using IPv4 routing gateway",
            "resp_time": 5,
            "uri": "/api/v1/routing/static_route",
            "post_test_callable": "check_for_static_route_created",
            "req_data": {
                "network": "1.2.3.4/32",
                "gateway": "TEST_ROUTING_GATEWAY_V4",
                "apply": True
            }
        },
        {
            "name": "Check interface requirement",
            "status": 400,
            "return": 6007
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 6008,
            "req_data": {
                "interface": "INVALID"
            }
        },
        {
            "name": "Check IP protocol requirement",
            "status": 400,
            "return": 6009,
            "req_data": {
                "interface": "wan"
            }
        },
        {
            "name": "Check IP protocol choice constraint",
            "status": 400,
            "return": 6010,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "INVALID"
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 6011,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet"
            }
        },
        {
            "name": "Check name character constraint",
            "status": 400,
            "return": 6012,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "!@#INVALID NAME<>?^&*()"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 6026,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "WAN_DHCP"
            }
        },
        {
            "name": "Check gateway address requirement",
            "status": 400,
            "return": 6013,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST"
            }
        },
        {
            "name": "Check gateway address validation",
            "status": 400,
            "return": 6014,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST",
                "gateway": "INVALID GATEWAY ADDRESS"
            }
        },
        {
            "name": "Check non-dynamic interface gateway constraint",
            "status": 400,
            "return": 6029,
            "req_data": {
                "interface": "lan",
                "ipprotocol": "inet",
                "name": "TEST",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check dynamic IPv4 gateway override",
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "DYNAMICv4",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check dynamic IPv4 gateway override maximum constraint",
            "status": 400,
            "return": 6029,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "NEWDYNAMICv4",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check dynamic IPv6 gateway override",
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "DYNAMICv6",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check dynamic IPv6 gateway override maximum constraint",
            "status": 400,
            "return": 6029,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "NEWDYNAMICv6",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check monitor IP validation",
            "status": 400,
            "return": 6025,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "monitor": "0::"
            }
        },
        {
            "name": "Check weight minimum constraint",
            "status": 400,
            "return": 6015,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "weight": 0
            }
        },
        {
            "name": "Check weight maximum constraint",
            "status": 400,
            "return": 6015,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "weight": 31
            }
        },
        {
            "name": "Check data payload minimum constraint",
            "status": 400,
            "return": 6016,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "data_payload": 0
            }
        },
        {
            "name": "Check low latency minimum constraint",
            "status": 400,
            "return": 6017,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "latencylow": 0
            }
        },
        {
            "name": "Check high latency minimum constraint",
            "status": 400,
            "return": 6018,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "latencylow": 5,
                "latencyhigh": 4
            }
        },
        {
            "name": "Check low loss minimum constraint",
            "status": 400,
            "return": 6019,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "losslow": 0
            }
        },
        {
            "name": "Check low loss maximum constraint",
            "status": 400,
            "return": 6019,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "losslow": 101
            }
        },
        {
            "name": "Check high loss minimum constraint",
            "status": 400,
            "return": 6020,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "losslow": 99,
                "losshigh": 50
            }
        },
        {
            "name": "Check high loss minimum constraint",
            "status": 400,
            "return": 6020,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "losslow": 50,
                "losshigh": 101
            }
        },
        {
            "name": "Check interval minimum constraint",
            "status": 400,
            "return": 6021,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "interval": 0
            }
        },
        {
            "name": "Check interval maximum constraint",
            "status": 400,
            "return": 6021,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "interval": 100000000000
            }
        },
        {
            "name": "Check loss interval minimum constraint",
            "status": 400,
            "return": 6022,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "loss_interval": 0
            }
        },
        {
            "name": "Check time period minimum constraint",
            "status": 400,
            "return": 6023,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "time_period": 0
            }
        },
        {
            "name": "Check alert interval minimum constraint",
            "status": 400,
            "return": 6024,
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "TEST_MONITOR",
                "gateway": "172.16.77.200",
                "alert_interval": 0
            }
        }
    ]
    put_tests = [
        {
            "name": "Update IPv4 routing gateway",
            "resp_time": 5,
            "post_test_callable": "check_for_static_route_updated",
            "req_data": {
                "id": "TEST_ROUTING_GATEWAY_V4",
                "name": "UPDATED_TEST_ROUTING_GATEWAY_V4",
                "ipprotocol": "inet",
                "gateway": "172.16.209.2",
                "monitor": "172.16.209.100",
                "descr": "Updated E2E Test",
                "disabled": False,
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
                "losshigh": 10,
                "apply": True
            }
        },
        {
            "name": "Ensure gateway cannot be disabled while in use",
            "status": 400,
            "return": 6030,
            "req_data": {
                "id": "UPDATED_TEST_ROUTING_GATEWAY_V4",
                "disabled": True
            }
        },
        {
            "name": "Update IPv6 routing gateway",
            "resp_time": 5,
            "req_data": {
                "id": "TEST_ROUTING_GATEWAY_V6",
                "name": "UPDATED_TEST_ROUTING_GATEWAY_V6",
                "ipprotocol": "inet6",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "monitor": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "descr": "Updated E2E Test",
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
        },
        {
            "name": "Check ID requirement",
            "status": 400,
            "return": 6027
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 6028,
            "req_data": {
                "id": "INVALID"
            }
        },
        {
            "name": "Check interface exists constraint",
            "status": 400,
            "return": 6008,
            "req_data": {
                "id": 0,
                "interface": "INVALID"
            }
        },
        {
            "name": "Check IP protocol choice constraint",
            "status": 400,
            "return": 6010,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "ipprotocol": "INVALID"
            }
        },
        {
            "name": "Check name character constraint",
            "status": 400,
            "return": 6012,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "!@#INVALID NAME<>?^&*()"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 6026,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "DYNAMICv6"
            }
        },
        {
            "name": "Check gateway address validation",
            "status": 400,
            "return": 6014,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST",
                "gateway": "INVALID GATEWAY ADDRESS"
            }
        },
        {
            "name": "Check non-dynamic interface gateway constraint",
            "status": 400,
            "return": 6029,
            "req_data": {
                "id": 0,
                "interface": "lan",
                "ipprotocol": "inet",
                "name": "TEST",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check dynamic IPv4 gateway override maximum constraint",
            "status": 400,
            "return": 6029,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "ipprotocol": "inet",
                "name": "NEWDYNAMICv4",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check dynamic IPv6 gateway override maximum constraint",
            "status": 400,
            "return": 6029,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "NEWDYNAMICv6",
                "gateway": "dynamic"
            }
        },
        {
            "name": "Check monitor IP validation",
            "status": 400,
            "return": 6025,
            "req_data": {
                "id": 0,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "monitor": "172.16.77.50"
            }
        },
        {
            "name": "Check weight minimum constraint",
            "status": 400,
            "return": 6015,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "weight": 0
            }
        },
        {
            "name": "Check weight maximum constraint",
            "status": 400,
            "return": 6015,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "weight": 31
            }
        },
        {
            "name": "Check data payload minimum constraint",
            "status": 400,
            "return": 6016,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "data_payload": 0
            }
        },
        {
            "name": "Check low latency minimum constraint",
            "status": 400,
            "return": 6017,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "latencylow": 0
            }
        },
        {
            "name": "Check high latency minimum constraint",
            "status": 400,
            "return": 6018,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "latencylow": 5,
                "latencyhigh": 4
            }
        },
        {
            "name": "Check low loss minimum constraint",
            "status": 400,
            "return": 6019,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "losslow": 0
            }
        },
        {
            "name": "Check low loss maximum constraint",
            "status": 400,
            "return": 6019,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "losslow": 101
            }
        },
        {
            "name": "Check high loss minimum constraint",
            "status": 400,
            "return": 6020,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "losslow": 99,
                "losshigh": 50
            }
        },
        {
            "name": "Check high loss minimum constraint",
            "status": 400,
            "return": 6020,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "losslow": 50,
                "losshigh": 101
            }
        },
        {
            "name": "Check interval minimum constraint",
            "status": 400,
            "return": 6021,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "interval": 0
            }
        },
        {
            "name": "Check interval maximum constraint",
            "status": 400,
            "return": 6021,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "interval": 100000000000
            }
        },
        {
            "name": "Check loss interval minimum constraint",
            "status": 400,
            "return": 6022,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "loss_interval": 0
            }
        },
        {
            "name": "Check time period minimum constraint",
            "status": 400,
            "return": 6023,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "time_period": 0
            }
        },
        {
            "name": "Check alert interval minimum constraint",
            "status": 400,
            "return": 6024,
            "req_data": {
                "id": 1,
                "interface": "wan",
                "ipprotocol": "inet6",
                "name": "TEST_MONITOR",
                "gateway": "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                "alert_interval": 0
            }
        }
    ]
    delete_tests = [
        {
            "name": "Ensure gateway cannot be deleted while in use",
            "status": 400,
            "return": 6031,
            "req_data": {"id": "UPDATED_TEST_ROUTING_GATEWAY_V4"}
        },
        {
            "name": "Delete the static route used for testing",
            "uri": "/api/v1/routing/static_route",
            "resp_time": 5,
            "req_data": {"id": 0, "apply": True}
        },
        {
            "name": "Check ID requirement",
            "status": 400,
            "return": 6027
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 6028,
            "req_data": {
                "id": "INVALID"
            }
        },
        {
            "name": "Delete IPv4 routing gateway",
            "req_data": {
                "id": 0
            },
            "resp_time": 5    # Allow a few seconds to safely remove the gateway and reload the route table
        },
        {
            "name": "Delete IPv6 routing gateway",
            "req_data": {
                "id": 0
            },
            "resp_time": 5    # Allow a few seconds to safely remove the gateway and reload the route table
        },
        {
            "name": "Delete dynamic IPv4 gateway override",
            "req_data": {
                "id": 0
            },
            "resp_time": 5    # Allow a few seconds to safely remove the gateway and reload the route table
        },
        {
            "name": "Delete dynamic IPv6 gateway override",
            "req_data": {
                "id": 0,
                "apply": True
            },
            "resp_time": 5    # Allow a few seconds to safely remove the gateway and reload the route table
        }
    ]

    def check_for_static_route_created(self):
        """Checks if the static route created by this test case is present on the remote system."""
        # Variables
        routing_table = self.pfsense_shell("netstat -rn")
        routing_table = re.sub(' +', ' ', routing_table)

        # Ensure the route we created exists and is correctly assigned the gateway we created
        if "1.2.3.4 172.16.209.1" not in routing_table:
            raise AssertionError("route for 1.2.3.4/32 with gateway 172.16.209.1 not in routing table")

    def check_for_static_route_updated(self):
        """Checks if the static route created by this test case had its gateway updated correctly."""
        # Variables
        routing_table = self.pfsense_shell("netstat -rn")
        routing_table = re.sub(' +', ' ', routing_table)

        # Ensure the route we created exists and is correctly assigned the gateway we updated
        if "1.2.3.4 172.16.209.2" not in routing_table:
            raise AssertionError("route for 1.2.3.4/32 did not get it's gateway updated to 172.16.209.2")


APIE2ETestRoutingGateway()
