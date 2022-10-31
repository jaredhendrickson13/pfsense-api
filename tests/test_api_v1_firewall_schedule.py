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
"""Script used to test the /api/v1/firewall/schedule endpoint."""
import e2e_test_framework


class APIE2ETestFirewallSchedule(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/firewall/schedule endpoint."""
    uri = "/api/v1/firewall/schedule"
    get_tests = [
            {"name": "Read all firewall schedules"}
        ]
    post_tests = [
        {
            "name": "Create firewall schedule",
            "req_data": {
                "name": "Test_Create_Schedule",
                "descr": "E2E test",
                "timerange": [
                    {
                        "month": "1,3,5",
                        "day": "10,20,25",
                        "hour": "0:15-20:00",
                        "rangedescr": "E2E test"
                    },
                    {
                        "position": "1,3,5",
                        "hour": "10:15-12:00",
                        "rangedescr": "E2E test"
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
            "req_data": {
                "name": "THIS NAME IS INVALID!"
            }
        },
        {
            "name": "Check name minimum length constraint",
            "status": 400,
            "return": 4147,    # This triggers the regex validation to fail, use that error code
            "req_data": {
                "name": ""
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 4148,
            "req_data": {
                "name": "THIS_NAME_IS_TOO_LONG_FOR_PFSENSE_TO_HANDLE"
            }
        },
        {
            "name": "Check name unique constraint",
            "status": 400,
            "return": 4149,
            "req_data": {
                "name": "Test_Create_Schedule"
            }
        },
        {
            "name": "Check time range requirement",
            "status": 400,
            "return": 4161,
            "req_data": {
                "name": "New_Schedule"
            }
        },
        {
            "name": "Check time range minimum length constraint",
            "status": 400,
            "return": 4162,
            "req_data": {
                "name": "New_Schedule",
                "timerange": []
            }
        },
        {
            "name": "Create firewall rule using created schedule",
            "uri": "/api/v1/firewall/rule",
            "req_data": {
                "interface": "wan",
                "ipprotocol": "inet",
                "protocol": "any",
                "type": "pass",
                "src": "any",
                "dst": "any",
                "sched": "Test_Create_Schedule"
            }
        }
    ]
    put_tests = [
        {
            "name": "Update firewall schedule",
            "req_data": {
                "name": "Test_Create_Schedule",
                "descr": "Updated E2E test",
                "timerange": [
                    {
                        "month": "1,3,5,12",
                        "day": "10,20,25,25",
                        "hour": "0:15-23:00",
                        "rangedescr": "Updated E2E test"
                    },
                    {
                        "position": "1,3,5",
                        "hour": "10:15-12:00",
                        "rangedescr": "Updated E2E test"
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
            "name": "Check updating non-existent firewall schedule",
            "status": 400,
            "return": 4150,
            "req_data": {
                "name": "INVALID"
            }
        },
        {
            "name": "Check update with no changes",
            "req_data": {
                "name": "Test_Create_Schedule"
            }
        },
        {
            "name": "Check update with changed description only",
            "req_data": {
                "name": "Test_Create_Schedule",
                "descr": "Update E2E test description only"
            }
        },
        {
            "name": "Check time range minimum length constraint",
            "status": 400,
            "return": 4162,
            "req_data": {
                "name": "Test_Create_Schedule",
                "timerange": []
            }
        },
    ]
    delete_tests = [
        {
            "name": "Check deleting schedule in use",
            "status": 400,
            "return": 4166,
            "req_data": {
                "name": "Test_Create_Schedule"
            }
        },
        {
            "name": "Delete firewall rule using schedule",
            "uri": "/api/v1/firewall/rule",
            "req_data": {}
        },
        {
            "name": "Delete firewall schedule",
            "req_data": {
                "name": "Test_Create_Schedule"
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
            "req_data": {
                "name": "INVALID"
            }
        }
    ]

    def post_post(self):
        # Only proceed if we have a valid post response
        if self.post_responses:
            # Check if the last request was a successful firewall rule creation
            if self.post_responses[-1] and isinstance(self.post_responses[-1]["data"], dict):
                # Add the tracker to delete req_data that deletes the firewall rule using the schedule
                if self.post_responses[-1]["data"].get("tracker", None):
                    self.delete_tests[1]["req_data"]["tracker"] = self.post_responses[-1]["data"]["tracker"]


APIE2ETestFirewallSchedule()
