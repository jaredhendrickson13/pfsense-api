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
"""Class used to test the /api/v1/services/service_watchdog endpoint."""
import e2e_test_framework


class APIE2ETestServicesServiceWatchdog(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/service_watchdog endpoint."""
    uri = "/api/v1/services/service_watchdog"
    put_tests = [
        {
            "name": "Check pfSense-pkg-Service_Watchdog installed constraint",
            "status": 500,
            "return": 12
        },
        {
            "name": "Install pfSense-pkg-Service_Watchdog so we can test further",
            "method": "POST",
            "uri": "/api/v1/system/package",
            "resp_time": 30,
            "req_data": {
                "name": "pfSense-pkg-Service_Watchdog"
            }
        },
        {
            "name": "Check service 'name' required constraint",
            "status": 400,
            "return": 2259,
            "req_data": {"services": [{}]}
        },
        {
            "name": "Check service 'name' options constraint",
            "status": 400,
            "return": 2260,
            "req_data": {"services": [{"name": "INVALID"}]}
        },
        {
            "name": "Check service 'name' no duplicates constraint",
            "status": 400,
            "return": 2261,
            "req_data": {"services": [{"name": "unbound"}, {"name": "unbound"}]}
        },
        {
            "name": "Update watched services",
            "req_data": {"services": [{"name": "unbound", "notify": True}]}
        },
        {
            "name": "Read the Service Watchdog configuration",
            "method": "GET"
        },
        {
            "name": "Uninstall pfSense-pkg-Service_Watchdog",
            "method": "DELETE",
            "uri": "/api/v1/system/package",
            "resp_time": 30,
            "req_data": {
                "name": "pfSense-pkg-Service_Watchdog"
            }
        },
    ]


APIE2ETestServicesServiceWatchdog()
