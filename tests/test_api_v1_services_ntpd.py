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

class APIE2ETestServicesNTPd(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/services/ntpd"
    get_tests = [{"name": "Read the NTPd configuration"}]
    put_tests = [
        {
            "name": "Update the NTPd configuration to enable all options",
            "resp_time": 3,
            "payload": {
                "interface": ["WAN", "em1", "lo0"],
                "orphan": 15,
                "timeservers": [{"timeserver": "2.pfsense.ntp.pool.org", "ispool": True}],
                "logpeer": True,
                "logsys": True,
                "clockstats": True,
                "peerstats": True,
                "loopstats": True,
                "statsgraph": True,
                "leapsec": "Test leap year configuration"
            },
        },
        {
            "name": "Update the NTPd configuration to disable all options",
            "resp_time": 3,
            "payload": {
                "interface": ["wan", "lo0"],
                "orphan": 12,
                "timeservers": [{"timeserver": "ntp.pool.org", "ispool": True}],
                "logpeer": False,
                "logsys": False,
                "clockstats": False,
                "peerstats": False,
                "loopstats": False,
                "statsgraph": False,
                "leapsec": ""
            },
        },
    ]

APIE2ETestServicesNTPd()
