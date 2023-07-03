# Copyright 2023 Jared Hendrickson
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
"""Script used to test the /api/v1/services/wol/send endpoint."""
import e2e_test_framework


class APIE2ETestServicesWOLSend(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/wol/send endpoint."""
    uri = "/api/v1/services/wol/send"
    post_privileges = ["page-all", "page-services-wakeonlan"]
    post_tests = [
        {
            "name": "Send magic WOL packet",
            "req_data": {
                "interface": "lan",
                "mac": "2C:54:91:88:C9:E3"
            }
        },
        {
            "name": "Send magic WOL packet with missing interface",
            "req_data": {
                "mac": "2C:54:91:88:C9:E3",
            },
            "status": 400,
            "return": 2099
        },
        {
            "name": "Send magic WOL packet with invalid interface",
            "req_data": {
                "interface": "INVALID",
                "mac": "2C:54:91:88:C9:E3"
            },
            "status": 400,
            "return": 2100
        },
        {
            "name": "Send magic WOL packet with missing mac",
            "req_data": {
                "interface": "lan"
            },
            "status": 400,
            "return": 2101
        },
        {
            "name": "Send magic WOL packet with invalid mac",
            "req_data": {
                "interface": "lan",
                "mac": "INVALID"
            },
            "status": 400,
            "return": 2102
        },
    ]


APIE2ETestServicesWOLSend()
