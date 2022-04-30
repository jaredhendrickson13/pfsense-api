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

class APIE2ETestServicesNTPdTimeServer(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/services/ntpd/time_server"
    post_tests = [
        {
            "name": "Add an NTP time server",
            "payload": {"timeserver": "time.google.com", "ispool": False, "noselect": True, "prefer": False}
        },
        {
            "name": "Test timeserver host/IP validation",
            "status": 400,
            "return": 2051,
            "payload": {"timeserver": 0.123, "ispool": True, "noselect": False, "prefer": True}
        }
    ]
    delete_tests = [
        {
            "name": "Delete existing timeserver",
            "payload": {"timeserver": "time.google.com"}
        },
        {
            "name": "Test deleting non-existing timeserver",
            "status": 400,
            "return": 2053,
            "payload": {"timeserver": "1.pfsense.pool.ntp.org"}
        }
    ]


APIE2ETestServicesNTPdTimeServer()
