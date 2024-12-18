#Copyright 2019-2025 Jared Hendrickson
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
"""Script used to test the /api/v1/firewall/traffic_shaper/queue endpoint."""
import e2e_test_framework


class APIE2ETestFirewallTrafficShaperQueue(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/traffic_shaper/queue endpoint."""
    uri = "/api/v1/firewall/traffic_shaper/queue"

    post_privileges = ["page-all", "page-firewall-trafficshaper-queues"]
    delete_privileges = ["page-all", "page-firewall-trafficshaper-queues"]

    post_tests = [
        {
            "name": "Create PRIQ parent traffic shaper",
            "uri": "/api/v1/firewall/traffic_shaper",
            "req_data": {
                "interface": "lan",
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
            "name": "Create traffic shaper queue",
            "req_data": {
                "interface": "lan",
                "name": "Test_Queue",
                "priority": 15,
                "description": "Traffic Shaper Queue E2E test",
                "default": True
            }
        },
        {
            "name": "Check interface requirement",
            "status": 400,
            "return": 4110
        },
        {
            "name": "Check interface exists validation",
            "status": 400,
            "return": 4111,
            "req_data": {
                "interface": "INVALID"
            }
        },
        {
            "name": "Check interface traffic shaper exists validation",
            "status": 400,
            "return": 4122,
            "req_data": {
                "interface": "wan"
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4123,
            "req_data": {
                "interface": "lan"
            }
        },
        {
            "name": "Check name character validation",
            "status": 400,
            "return": 4124,
            "req_data": {
                "interface": "lan",
                "name": "THIS NAME IS NOT VALID!!!"
            }
        },
        {
            "name": "Check name minimum length constraint",
            "status": 400,
            "return": 4124,    # Regex fails with an empty string, use that error code instead of 4125
            "req_data": {
                "interface": "lan",
                "name": ""
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 4125,
            "req_data": {
                "interface": "lan",
                "name": "THIS_NAME_IS_TOO_LONG"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 4126,
            "req_data": {
                "interface": "lan",
                "name": "Test_Queue"
            }
        },
        {
            "name": "Check PRIQ priority minimum constraint",
            "status": 400,
            "return": 4128,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "priority": -1
            }
        },
        {
            "name": "Check PRIQ priority maximum constraint",
            "status": 400,
            "return": 4128,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "priority": 16
            }
        },
        {
            "name": "Check queue limit minimum constraint",
            "status": 400,
            "return": 4120,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "qlimit": 0
            }
        },
        {
            "name": "Update to FAIRQ parent traffic shaper",
            "uri": "/api/v1/firewall/traffic_shaper",
            "method": "PUT",
            "req_data": {
                "interface": "lan",
                "scheduler": "FAIRQ",
                "bandwidthtype": "Mb",
                "bandwidth": 2000
            }
        },
        {
            "name": "Check FAIRQ/CBQ priority minimum constraint",
            "status": 400,
            "return": 4127,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "priority": -1
            }
        },
        {
            "name": "Check FAIRQ/CBQ priority maximum constraint",
            "status": 400,
            "return": 4127,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "priority": 8
            }
        },
        {
            "name": "Check bandwidth type validation",
            "status": 400,
            "return": 4116,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "bandwidthtype": "INVALID"
            }
        },
        {
            "name": "Check bandwidth minimum constraint",
            "status": 400,
            "return": 4118,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "bandwidthtype": "Gb",
                "bandwidth": 0
            }
        },
        {
            "name": "Check percentage-based bandwidth maximum constraint",
            "status": 400,
            "return": 4119,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "bandwidthtype": "%",
                "bandwidth": 101
            }
        },
        {
            "name": "Check bandwidth parent maximum constraint",
            "status": 400,
            "return": 4129,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "bandwidthtype": "Gb",
                "bandwidth": 100    # 100Gb is over the parent's 1Gb maximum
            }
        },
        {
            "name": "Create traffic shaper queue to increase child bandwidth",
            "req_data": {
                "interface": "lan",
                "name": "Mb-BW_Queue",
                "bandwidthtype": "Gb",
                "bandwidth": 1,
                "red": True
            }
        },
        {
            "name": "Create another traffic shaper queue to push sum of child bandwidth over parent maximum",
            "status": 400,
            "return": 4129,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "bandwidthtype": "Mb",
                "bandwidth": 1024,    # 1024Mb + our other child queue's 1Gb will exceed parent's 2000Mb limit
                "red": True
            }
        },
        {
            "name": "Update to HFSC parent traffic shaper",
            "uri": "/api/v1/firewall/traffic_shaper",
            "method": "PUT",
            "req_data": {
                "interface": "lan",
                "scheduler": "HFSC"
            }
        },
        {
            "name": "Check upperlimit3 requirement",
            "status": 400,
            "return": 4133,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "upperlimit": True
            }
        },
        {
            "name": "Check upperlimit3 validation",
            "status": 400,
            "return": 4134,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "upperlimit": True,
                "upperlimit3": "INVALID"
            }
        },
        {
            "name": "Check upperlimit1 validation",
            "status": 400,
            "return": 4130,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "upperlimit": True,
                "upperlimit1": "INVALID",
                "upperlimit3": "1Mb"
            }
        },
        {
            "name": "Check upperlimit2 requirement",
            "status": 400,
            "return": 4131,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "upperlimit": True,
                "upperlimit1": "1Mb",
                "upperlimit3": "1Mb"
            }
        },
        {
            "name": "Check upperlimit2 validation",
            "status": 400,
            "return": 4132,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "upperlimit": True,
                "upperlimit1": "1Mb",
                "upperlimit2": 0,
                "upperlimit3": "1Mb"
            }
        },
        {
            "name": "Check linkshare3 requirement",
            "status": 400,
            "return": 4138,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "linkshare": True
            }
        },
        {
            "name": "Check linkshare3 validation",
            "status": 400,
            "return": 4139,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "linkshare": True,
                "linkshare3": "INVALID"
            }
        },
        {
            "name": "Check linkshare1 validation",
            "status": 400,
            "return": 4135,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "linkshare": True,
                "linkshare1": "INVALID",
                "linkshare3": "1Mb"
            }
        },
        {
            "name": "Check linkshare2 requirement",
            "status": 400,
            "return": 4136,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "linkshare": True,
                "linkshare1": "1Mb",
                "linkshare3": "1Mb"
            }
        },
        {
            "name": "Check linkshare2 validation",
            "status": 400,
            "return": 4137,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "linkshare": True,
                "linkshare1": "1Mb",
                "linkshare2": 0,
                "linkshare3": "1Mb"
            }
        },
        {
            "name": "Check realtime3 requirement",
            "status": 400,
            "return": 4143,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "realtime": True
            }
        },
        {
            "name": "Check realtime3 validation",
            "status": 400,
            "return": 4144,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "realtime": True,
                "realtime3": "INVALID"
            }
        },
        {
            "name": "Check realtime1 validation",
            "status": 400,
            "return": 4140,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "realtime": True,
                "realtime1": "INVALID",
                "realtime3": "1Mb"
            }
        },
        {
            "name": "Check realtime2 requirement",
            "status": 400,
            "return": 4141,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "realtime": True,
                "realtime1": "1Mb",
                "realtime3": "1Mb"
            }
        },
        {
            "name": "Check realtime2 validation",
            "status": 400,
            "return": 4142,
            "req_data": {
                "interface": "lan",
                "name": "New_Queue",
                "realtime": True,
                "realtime1": "1Mb",
                "realtime2": 0,
                "realtime3": "1Mb"
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete Test_Queue",
            "req_data": {
                "interface": "lan",
                "name": "Test_Queue"
            }
        },
        {
            "name": "Check interface requirement",
            "status": 400,
            "return": 4110
        },
        {
            "name": "Check interface validation",
            "status": 400,
            "return": 4111,
            "req_data": {
                "interface": "INVALID"
            }
        },
        {
            "name": "Check interface with no traffic shaper",
            "status": 400,
            "return": 4122,
            "req_data": {
                "interface": "wan",
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4123,
            "req_data": {
                "interface": "lan"
            }
        },
        {
            "name": "Check name validation",
            "status": 400,
            "return": 4145,
            "req_data": {
                "interface": "lan",
                "name": "INVALID"
            }
        },
        {
            "name": "Delete parent traffic shaper",
            "uri": "/api/v1/firewall/traffic_shaper",
            "req_data": {
                "interface": "lan",
                "apply": True
            }
        },
    ]


APIE2ETestFirewallTrafficShaperQueue()
