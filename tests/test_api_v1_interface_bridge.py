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
"""Script used to test the /api/v1/interface/bridge endpoint."""
import e2e_test_framework


class APIE2ETestInterfaceBridge(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/interface/bridge endpoint."""
    uri = "/api/v1/interface/bridge"
    get_tests = [{"name": "Read all interface VLANs"}]
    post_tests = [
        {
            "name": "Create bridge for LAN",
            "payload": {
                "members": ["LAN"],
                "descr": "Test bridge"
            }
        },
        {
            "name": "Check members required constraint",
            "status": 400,
            "return": 3066
        },
        {
            "name": "Check members minimum constraint",
            "status": 400,
            "return": 3066,
            "payload": {
                "members": []
            }
        },
        {
            "name": "Check non-existent member interface constraint",
            "status": 400,
            "return": 3067,
            "payload": {
                "members": ["NonexistingIface"]
            }
        },
        {
            "name": "Check 1 bridge per member constraint",
            "status": 400,
            "return": 3068,
            "payload": {
                "members": ["lan"]
            }
        }
    ]
    put_tests = [
        {
            "name": "Update bridge for LAN",
            "payload": {
                "id": "bridge0",
                "members": ["lan"],
                "descr": "Updated test bridge"
            }
        },
        {
            "name": "Check bridge ID required constraint",
            "status": 400,
            "return": 3071
        },
        {
            "name": "Check bridge ID exists constraint",
            "status": 400,
            "return": 3072,
            "payload": {
                "id": "invalidbridgeid"
            }
        },
        {
            "name": "Check members minimum constraint",
            "status": 400,
            "return": 3066,
            "payload": {
                "id": "bridge0",
                "members": []
            }
        },
        {
            "name": "Check non-existent member interface constraint",
            "status": 400,
            "return": 3067,
            "payload": {
                "id": "bridge0",
                "members": ["NonexistingIface"]
            }
        }
    ]
    delete_tests = [
        {
            "name": "Check bridge ID required constraint",
            "status": 400,
            "return": 3071
        },
        {
            "name": "Check bridge ID exists constraint",
            "status": 400,
            "return": 3072,
            "payload": {
                "id": "invalidbridgeid"
            }
        },
        {
            "name": "Check bridge deletion",
            "payload": {
                "id": "bridge0"
            }
        }
    ]


APIE2ETestInterfaceBridge()
