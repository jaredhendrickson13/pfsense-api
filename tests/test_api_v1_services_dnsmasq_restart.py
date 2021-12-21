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

class APIE2ETestServicesDnsmasqRestart(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/services/dnsmasq/restart"
    post_tests = [
        {
            "name": "Restart the dnsmasq service",
            "resp_time": 5    # Allow 5 seconds for dnsmasq to restart
        }
    ]

APIE2ETestServicesDnsmasqRestart()
