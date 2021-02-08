# Copyright 2021 Jared Hendrickson
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

import unit_test_framework

class APIUnitTestServicesNTPdTimeServer(unit_test_framework.APIUnitTest):
    url = "/api/v1/services/ntpd/time_server"
    post_payloads = [
        {"timeserver": "time.google.com", "ispool": False, "noselect": True, "prefer": False},
        {"timeserver": "1.pfsense.pool.ntp.org", "ispool": True, "noselect": False, "prefer": True}
    ]
    delete_payloads = [
        {"timeserver": "time.google.com"},
        {"timeserver": "1.pfsense.pool.ntp.org"}
    ]


APIUnitTestServicesNTPdTimeServer()