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

class APIE2ETestDiagnosticsCommandPrompt(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/diagnostics/command_prompt"
    post_tests = [
        {
            "name": "Execute shell command",
            "payload": {
                "shell_cmd": "curl -s http://ipinfo.io/ip"
            }
        },
        {
            "name": "Test shell command requirement",
            "status": 400,
            "return": 7000
        }
    ]

APIE2ETestDiagnosticsCommandPrompt()
