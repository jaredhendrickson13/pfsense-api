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
"""Script used to test the /api/v1/services/unbound endpoint."""
import e2e_test_framework

CUSTOM_OPTIONS_UPDATE = 'server:\nlocal-zone: "example.com" redirect\nlocal-data: "example.com 86400 IN A 10.1.2.3"'

class APIE2ETestServicesUnbound(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/unbound endpoint."""
    uri = "/api/v1/services/unbound"
    get_privileges = ["page-all", "page-services-dnsresolver"]
    get_tests = [{"name": "Read the Unbound configuration"}]
    put_tests = [
        {
            "name": "Check updating Unbound options",
            "post_test_callable": "is_custom_options_updated",
            "resp_time": 10,
            "req_data": {
                "enable": True,
                "dnssec": True,
                "custom_options": CUSTOM_OPTIONS_UPDATE,
                "apply": True
            }
        },
    ]

    def is_custom_options_updated(self):
        # Read the unbound.conf file
        unbound_conf = self.pfsense_shell("cat /var/unbound/unbound.conf")

        # Ensure custom options are found in the configuration
        if CUSTOM_OPTIONS_UPDATE not in unbound_conf:
            raise AssertionError("Expected custom_options to be present in /var/unbound/unbound.conf")



APIE2ETestServicesUnbound()
