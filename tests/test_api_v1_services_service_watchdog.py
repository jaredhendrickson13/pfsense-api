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
"""Class used to test the /api/v1/services/service_watchdog endpoint."""
import time

import e2e_test_framework


class APIE2ETestServicesServiceWatchdog(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/service_watchdog endpoint."""
    uri = "/api/v1/services/service_watchdog"

    get_privileges = ["page-all", "api-services-servicewatchdog"]
    put_privileges = ["page-all", "api-services-servicewatchdog"]

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
            "resp_data_empty": True,
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
            "name": "Update watched services and ensure services get restarted when stopped",
            "req_data": {"services": [{"name": "unbound", "notify": True}]},
            "post_test_callable": "is_unbound_restarted"
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
            "resp_data_empty": True,
            "req_data": {
                "name": "pfSense-pkg-Service_Watchdog"
            }
        },
    ]

    def is_unbound_restarted(self):
        """Checks if the unbound service was restarted by service watchdog"""
        # Local variables
        unbound_running = False

        # First, kill the unbound process
        self.pfsense_shell("pkill unbound")

        # Check up to 90 times, service watchdog only runs every minute by default
        for _ in range(0, 90):
            # Check active processes
            unbound_processes = self.pfsense_shell("ps aux | grep /usr/local/sbin/unbound")

            # Check if the main unbound process is running
            if "/usr/local/sbin/unbound -c /var/unbound/unbound.conf" in unbound_processes:
                unbound_running = True
                break

            # Wait 1 second before retrying
            time.sleep(1)

        # Raise an error if unbound wasn't running by the end of the loop
        if not unbound_running:
            raise AssertionError("Expected Service Watchdog to restart Unbound service")


APIE2ETestServicesServiceWatchdog()
