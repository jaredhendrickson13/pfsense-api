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

class APIUnitTestFirewallSchedule(unit_test_framework.APIUnitTest):
    uri = "/api/v1/firewall/schedule"
    post_tests = [
        {
            "name": "Create firewall schedule",
            "payload": {
                "name": "Test_Schedule",
                "descr": "Unit test",
                "timerange": [
                    {
                        "month": "1,3,5",
                        "day": "10,20,25",
                        "hour": "0:15-20:00",
                        "rangedescr": "Unit test"
                    },
                    {
                        "position": "1,3,5",
                        "hour": "10:15-12:00",
                        "rangedescr": "Unit test"
                    }
                ]
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4146
        },
        {
            "name": "Check name character validation",
            "status": 400,
            "return": 4147,
            "payload": {
                "name": "THIS NAME IS INVALID!"
            }
        },
        {
            "name": "Check name minimum length constraint",
            "status": 400,
            "return": 4147,    # This triggers the regex validation to fail, use that error code
            "payload": {
                "name": ""
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 4148,
            "payload": {
                "name": "THIS_NAME_IS_TOO_LONG_FOR_PFSENSE_TO_HANDLE"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 4149,
            "payload": {
                "name": "Test_Schedule"
            }
        },
        {
            "name": "Check time range requirement",
            "status": 400,
            "return": 4161,
            "payload": {
                "name": "New_Schedule"
            }
        },
        {
            "name": "Check time range minimum length constraint",
            "status": 400,
            "return": 4162,
            "payload": {
                "name": "New_Schedule",
                "timerange": []
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete firewall schedule",
            "payload": {
                "name": "Test_Schedule"
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4146
        },
        {
            "name": "Check delete non-existing schedule name",
            "status": 400,
            "return": 4150,
            "payload": {
                "name": "INVALID"
            }
        }
        # TODO: Add test to check inability to delete schedules while they're in use
    ]

APIUnitTestFirewallSchedule()