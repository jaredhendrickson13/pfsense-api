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

class APIE2ETestFirewallTrafficShaper(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/firewall/traffic_shaper"
    get_tests = [{"name": "Read all traffic shapers"}]
    post_tests = [
        {
            "name": "Create a traffic shaper",
            "payload": {
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
            "payload": {
                "interface": "INVALID"
            }
        },
        {
            "name": "Check interface unique constraint",
            "status": 400,
            "return": 4112,
            "payload": {
                "interface": "LAN"
            }
        },
        {
            "name": "Check scheduler requirement",
            "status": 400,
            "return": 4113,
            "payload": {
                "interface": "wan"
            }
        },
        {
            "name": "Check scheduler validation",
            "status": 400,
            "return": 4114,
            "payload": {
                "interface": "wan",
                "scheduler": "INVALID"
            }
        },
        {
            "name": "Check bandwidth type requirement",
            "status": 400,
            "return": 4115,
            "payload": {
                "interface": "wan",
                "scheduler": "HFSC"
            }
        },
        {
            "name": "Check bandwidth type validation",
            "status": 400,
            "return": 4116,
            "payload": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "INVALID"
            }
        },
        {
            "name": "Check bandwidth requirement",
            "status": 400,
            "return": 4117,
            "payload": {
                "interface": "wan",
                "scheduler": "HFSC",
                "bandwidthtype": "%"
            }
        },
        {
            "name": "Check bandwidth minimum constraint",
            "status": 400,
            "return": 4118,
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
                "interface": "INVALID"
            }
        },
        {
            "name": "Check non-existing traffic shaper",
            "status": 400,
            "return": 4122,
            "payload": {
                "interface": "wan"
            }
        },
        {
            "name": "Check scheduler validation",
            "status": 400,
            "return": 4114,
            "payload": {
                "interface": "lan",
                "scheduler": "INVALID"
            }
        },
        {
            "name": "Check bandwidth requirement when changing bandwidth types",
            "status": 400,
            "return": 4117,
            "payload": {
                "interface": "lan",
                "bandwidthtype": "b"
            }
        },
        {
            "name": "Check bandwidth type validation",
            "status": 400,
            "return": 4116,
            "payload": {
                "interface": "lan",
                "bandwidthtype": "INVALID"
            }
        },

        {
            "name": "Check bandwidth minimum constraint",
            "status": 400,
            "return": 4118,
            "payload": {
                "interface": "lan",
                "bandwidth": 0
            }
        },
        {
            "name": "Check bandwidth maximum constraint when percentage type is chosen",
            "status": 400,
            "return": 4119,
            "payload": {
                "interface": "lan",
                "bandwidthtype": "%",
                "bandwidth": 101
            }
        },
        {
            "name": "Check queue limit minimum constraint",
            "status": 400,
            "return": 4120,
            "payload": {
                "interface": "lan",
                "qlimit": 0
            }
        },
        {
            "name": "Check TBR size minimum constraint",
            "status": 400,
            "return": 4121,
            "payload": {
                "interface": "lan",
                "tbrconfig": 0
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete traffic shaper for LAN interface",
            "payload": {
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
            "payload": {
                "interface": "lan"
            }
        },
        {
            "name": "Check interface validation",
            "status": 400,
            "return": 4111,
            "payload": {
                "interface": "INVALID"
            }
        },
    ]


APIE2ETestFirewallTrafficShaper()
