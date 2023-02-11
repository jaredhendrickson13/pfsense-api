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
"""Script used to test the /api/v1/firewall/schedule/time_range endpoint."""
import e2e_test_framework


class APIE2ETestFirewallScheduleTimeRange(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/schedule/time_range endpoint."""
    uri = "/api/v1/firewall/schedule/time_range"

    post_privileges = ["page-all", "page-firewall-schedules-edit"]
    delete_privileges = ["page-all", "page-firewall-schedules-edit"]

    post_tests = [
        {
            "name": "Create parent firewall schedule",
            "uri": "/api/v1/firewall/schedule",
            "req_data": {
                "name": "Test_Schedule",
                "descr": "E2E test",
                "timerange": [
                    {
                        "position": "1,3,5",
                        "hour": "10:15-12:00",
                        "rangedescr": "E2E test"
                    }
                ]
            }
        },
        {
            "name": "Create month/day based time range",
            "req_data": {
                "name": "Test_Schedule",
                "month": "1,2",
                "day": "1,17",
                "hour": "0:00-22:59",
                "rangedescr": "E2E test"
            }
        },
        {
            "name": "Create weekly based time range",
            "req_data": {
                "name": "Test_Schedule",
                "position": "1,2,3,4,5",
                "hour": "0:00-22:59",
                "rangedescr": "E2E test"
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4146
        },
        {
            "name": "Check non-existing schedule name",
            "status": 400,
            "return": 4150,
            "req_data": {
                "name": "INVALID"
            }
        },
        {
            "name": "Check month requirement",
            "status": 400,
            "return": 4151,
            "req_data": {
                "name": "Test_Schedule"
            }
        },
        {
            "name": "Check month maximum constraint",
            "status": 400,
            "return": 4152,
            "req_data": {
                "name": "Test_Schedule",
                "month": "13"
            }
        },
        {
            "name": "Check month minimum constraint",
            "status": 400,
            "return": 4152,
            "req_data": {
                "name": "Test_Schedule",
                "month": "0"
            }
        },
        {
            "name": "Check day requirement",
            "status": 400,
            "return": 4153,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1"
            }
        },
        {
            "name": "Check day and month count mismatch",
            "status": 400,
            "return": 4154,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1,2,3",
                "day": "21,22"
            }
        },
        {
            "name": "Check day minimum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "0"
            }
        },
        {
            "name": "Check January day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "32"
            }
        },
        {
            "name": "Check February day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "2",
                "day": "30"
            }
        },
        {
            "name": "Check March day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "3",
                "day": "32"
            }
        },
        {
            "name": "Check April day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "4",
                "day": "31"
            }
        },
        {
            "name": "Check May day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "5",
                "day": "32"
            }
        },
        {
            "name": "Check June day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "6",
                "day": "31"
            }
        },
        {
            "name": "Check July day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "7",
                "day": "32"
            }
        },
        {
            "name": "Check August day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "8",
                "day": "32"
            }
        },
        {
            "name": "Check September day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "9",
                "day": "31"
            }
        },
        {
            "name": "Check October day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "10",
                "day": "32"
            }
        },
        {
            "name": "Check November day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "11",
                "day": "31"
            }
        },
        {
            "name": "Check December day maximum constraint",
            "status": 400,
            "return": 4155,
            "req_data": {
                "name": "Test_Schedule",
                "month": "12",
                "day": "32"
            }
        },
        {
            "name": "Check hour requirement",
            "status": 400,
            "return": 4156,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1"
            }
        },
        {
            "name": "Check hour start time validation",
            "status": 400,
            "return": 4157,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1",
                "hour": "INVALID-23:59"
            }
        },
        {
            "name": "Check hour good hour bad minute start time validation",
            "status": 400,
            "return": 4157,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1",
                "hour": "1:23-23:59"
            }
        },
        {
            "name": "Check hour bad hour good minute start time validation",
            "status": 400,
            "return": 4157,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1",
                "hour": "25:00-23:59"
            }
        },
        {
            "name": "Check hour end time validation",
            "status": 400,
            "return": 4158,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1",
                "hour": "0:00-INVALID"
            }
        },
        {
            "name": "Check good hour bad minute end time validation",
            "status": 400,
            "return": 4158,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1",
                "hour": "0:00-23:23"
            }
        },
        {
            "name": "Check bad hour good minute end time validation",
            "status": 400,
            "return": 4158,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1",
                "hour": "00:00-24:59"
            }
        },
        {
            "name": "Check start time greater than end time validation",
            "status": 400,
            "return": 4159,
            "req_data": {
                "name": "Test_Schedule",
                "month": "1",
                "day": "1",
                "hour": "23:00-22:59"
            }
        },
        {
            "name": "Check priority minimum constraint",
            "status": 400,
            "return": 4160,
            "req_data": {
                "name": "Test_Schedule",
                "position": "1,2,0",
                "hour": "23:00-22:59"
            }
        },
        {
            "name": "Check position maximum constraint",
            "status": 400,
            "return": 4160,
            "req_data": {
                "name": "Test_Schedule",
                "position": "1,2,8",
                "hour": "23:00-23:59"
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete weekly based schedule time range",
            "req_data": {
                "name": "Test_Schedule",
                "id": 2
            }
        },
        {
            "name": "Delete month/day based schedule time range",
            "req_data": {
                "name": "Test_Schedule",
                "id": 1
            }
        },
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 4146
        },
        {
            "name": "Check ID requirement",
            "status": 400,
            "return": 4163,
            "req_data": {
                "name": "Test_Schedule"
            }
        },
        {
            "name": "Check ID validation",
            "status": 400,
            "return": 4164,
            "req_data": {
                "name": "Test_Schedule",
                "id": "INVALID"
            }
        },
        {
            "name": "Check deleting time range from non-existing schedule",
            "status": 400,
            "return": 4150,
            "req_data": {
                "name": "INVALID",
                "id": 0
            }
        },
        {
            "name": "Check deleting last remaining time range",
            "status": 400,
            "return": 4165,
            "req_data": {
                "name": "Test_Schedule",
                "id": 0
            }
        },
        {
            "name": "Delete parent firewall schedule",
            "uri": "/api/v1/firewall/schedule",
            "req_data": {
                "name": "Test_Schedule"
            }
        },
    ]


APIE2ETestFirewallScheduleTimeRange()
