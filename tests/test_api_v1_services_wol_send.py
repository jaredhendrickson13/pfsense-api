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

class APIE2ETestServicesWOLSend(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/services/wol/send/"
    post_tests = [
        {
            "name": "Send magic WOL packet",
            "payload": {
                "interface": "lan",
                "mac": "2C:54:91:88:C9:E3"
            }
        },
        {
            "name": "Send magic WOL packet with missing interface",
            "payload": {
                "mac": "2C:54:91:88:C9:E3",
            },
            "status": 400,
            "return": 2099
        },
        {
            "name": "Send magic WOL packet with invalid interface",
            "payload": {
                "interface": "INVALID",
                "mac": "2C:54:91:88:C9:E3"
            },
            "status": 400,
            "return": 2100
        },
        {
            "name": "Send magic WOL packet with missing mac",
            "payload": {
                "interface": "lan"
            },
            "status": 400,
            "return": 2101
        },
        {
            "name": "Send magic WOL packet with invalid mac",
            "payload": {
                "interface": "lan",
                "mac": "INVALID"
            },
            "status": 400,
            "return": 2102
        },
    ]

APIE2ETestServicesWOLSend()
