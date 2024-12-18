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
"""Script used to test the /api/v1/firewall/traffic_shaper endpoint."""
import e2e_test_framework


class APIE2ETestFirewallTrafficShaper(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/traffic_shaper endpoint."""
    uri = "/api/v1/firewall/traffic_shaper"

    get_privileges = ["page-all", "page-firewall-trafficshaper"]
    post_privileges = ["page-all", "page-firewall-trafficshaper"]
    put_privileges = ["page-all", "page-firewall-trafficshaper"]
    delete_privileges = ["page-all", "page-firewall-trafficshaper"]

    get_tests = [{"name": "Read all traffic shapers"}]
    post_tests = [
        {
            "name": "Create a traffic shaper",
            "req_data": {
                "interface": "lan",
                "scheduler": "PRIQ",
                "bandwidthtype": "Gb",
                "bandwidth": 1,
                "enabled": False,
                "qlimit": 1000,
                "tbrconfig": 1000,
                "apply": True
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
            "name": "Check interface unique constraint",
            "status": 400,
            "return": 4112,
            "req_data": {
                "interface": "LAN"
            }
        },
        {
            "name": "Check scheduler requirement",
            "status": 400,
            "return": 4113,
            "req_data": {
                "interface": "wan"
            }
        },
        {
            "name": "Check scheduler validation",
            "status": 400,
            "return": 4114,
            "req_data": {
                "interface": "wan",
                "scheduler": "INVALID"
            }
        },
        {
            "name": "Check bandwidth type requirement",
            "status": 400,
            "return": 4115,
            "req_data": {
                "interface": "wan",
                "scheduler": "HFSC"
            }
        },
        {
            "name": "Check bandwidth type validation",
            "status": 400,
            "return": 4116,
            "req_data": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "INVALID"
            }
        },
        {
            "name": "Check bandwidth requirement",
            "status": 400,
            "return": 4117,
            "req_data": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "%"
            }
        },
        {
            "name": "Check bandwidth minimum constraint",
            "status": 400,
            "return": 4118,
            "req_data": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "%",
                "bandwidth": 0
            }
        },
        {
            "name": "Check bandwidth maximum constraint when percentage type is chosen",
            "status": 400,
            "return": 4119,
            "req_data": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "%",
                "bandwidth": 101
            }
        },
        {
            "name": "Check queue limit minimum constraint",
            "status": 400,
            "return": 4120,
            "req_data": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "%",
                "bandwidth": 25,
                "qlimit": 0
            }
        },
        {
            "name": "Check TBR size minimum constraint",
            "status": 400,
            "return": 4121,
            "req_data": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "%",
                "bandwidth": 25,
                "tbrconfig": 0
            }
        },
    ]
    put_tests = [
        {
            "name": "Create a traffic shaper",
            "req_data": {
                "interface": "lan",
                "scheduler": "HFSC",
                "bandwidthtype": "Mb",
                "bandwidth": 100,
                "enabled": True,
                "qlimit": 500,
                "tbrconfig": 500,
                "apply": True
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
            "name": "Check non-existing traffic shaper",
            "status": 400,
            "return": 4122,
            "req_data": {
                "interface": "wan"
            }
        },
        {
            "name": "Check scheduler validation",
            "status": 400,
            "return": 4114,
            "req_data": {
                "interface": "lan",
                "scheduler": "INVALID"
            }
        },
        {
            "name": "Check bandwidth requirement when changing bandwidth types",
            "status": 400,
            "return": 4117,
            "req_data": {
                "interface": "lan",
                "bandwidthtype": "b"
            }
        },
        {
            "name": "Check bandwidth type validation",
            "status": 400,
            "return": 4116,
            "req_data": {
                "interface": "lan",
                "bandwidthtype": "INVALID"
            }
        },

        {
            "name": "Check bandwidth minimum constraint",
            "status": 400,
            "return": 4118,
            "req_data": {
                "interface": "lan",
                "bandwidth": 0
            }
        },
        {
            "name": "Check bandwidth maximum constraint when percentage type is chosen",
            "status": 400,
            "return": 4119,
            "req_data": {
                "interface": "lan",
                "bandwidthtype": "%",
                "bandwidth": 101
            }
        },
        {
            "name": "Check queue limit minimum constraint",
            "status": 400,
            "return": 4120,
            "req_data": {
                "interface": "lan",
                "qlimit": 0
            }
        },
        {
            "name": "Check TBR size minimum constraint",
            "status": 400,
            "return": 4121,
            "req_data": {
                "interface": "lan",
                "tbrconfig": 0
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete traffic shaper for LAN interface",
            "req_data": {
                "interface": "lan",
                "apply": True
            }
        },
        {
            "name": "Check interface requirement",
            "status": 400,
            "return": 4110
        },
        {
            "name": "Delete non-existent traffic shaper",
            "status": 400,
            "return": 4122,
            "req_data": {
                "interface": "lan"
            }
        },
        {
            "name": "Check interface validation",
            "status": 400,
            "return": 4111,
            "req_data": {
                "interface": "INVALID"
            }
        },
    ]


APIE2ETestFirewallTrafficShaper()
