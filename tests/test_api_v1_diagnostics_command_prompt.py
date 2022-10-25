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
"""Script used to test the /api/v1/diagnostics/command_prompt endpoint."""
import e2e_test_framework


class APIE2ETestDiagnosticsCommandPrompt(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/diagnostics/command_prompt endpoint."""
    uri = "/api/v1/diagnostics/command_prompt"
    post_tests = [
        {
            "name": "Execute shell command",
            "post_test_callable": "check_shell_cmd_response",
            "payload": {
                "shell_cmd": "whoami"
            }
        },
        {
            "name": "Test shell command requirement",
            "status": 400,
            "return": 7000
        }
    ]

    def check_shell_cmd_response(self):
        """Checks that the shell command we run responds with the expected output."""
        # Ensure the 'whoami' we run responds with 'root'
        if self.last_response.get("data", {}).get("cmd_output") != "root":
            raise AssertionError("expected shell_cmd 'whoami' to respond with 'root'")


APIE2ETestDiagnosticsCommandPrompt()
