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
"""Script used to test the /api/v1/access_token endpoint."""
import sys
import e2e_test_framework


class APIE2ETestAccessToken(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/access_token endpoint."""
    uri = "/api/v1/access_token"
    post_tests = [
        {
            "name": "Change auth mode to local to test token-based auth restriction",
            "uri": "/api/v1/system/api",
            "method": "PUT",
            "pre_test_callable": "check_for_token_auth",
            "post_test_callable": "set_auth_mode_to_local",
            "req_data": {"authmode": "local"}
        },
        {
            "name": "Ensure clients cannot request token when auth mode is not jwt or token",
            "status": 403,
            "return": 9
        },
        {
            "name": "Change auth mode to jwt to test JWT authentication",
            "uri": "/api/v1/system/api",
            "method": "PUT",
            "post_test_callable": "set_auth_mode_to_jwt",
            "req_data": {"authmode": "jwt"}
        },
        {
            "name": "Test JWT authentication and change auth mode to token to test API Token authentication",
            "uri": "/api/v1/system/api",
            "method": "PUT",
            "post_test_callable": "set_auth_mode_to_token",
            "req_data": {"authmode": "token"}
        },
        {
            "name": "Request new API token to authenticate with",
            "auth_mode": "local",
            "post_test_callable": "set_username_password_to_token"
        },
        {
            "name": "Test API token authentication and change authentication mode back to original value",
            "uri": "/api/v1/system/api",
            "method": "PUT",
            "req_data_callable": "set_req_data_to_initial_auth",
            "post_test_callable": "set_auth_mode_to_initial"
        }
    ]

    def __init__(self):
        self.test_access_token_orig_auth_mode = None
        self.test_access_token_orig_username = None
        self.test_access_token_orig_password = None
        super().__init__()

    def check_for_token_auth(self):
        """Sets a warning if token authentication is used initially. This test case requires a username."""
        # Skip these tests if auth_mode was 'token' to begin. It will not work without knowing a username/password.
        if self.args.auth_mode == "token" and not self.post_responses:
            # Print a warning before exiting
            print(
                self.__format_msg__(
                    "POST",
                    {"name": "N/A"},
                    "Tests not available when 'auth_mode' is set to 'token' at start. Skipping!",
                    mode="warning"
                )
            )
            sys.exit(0)

    def set_auth_mode_to_local(self):
        """Sets the E2E frameworks authentication mode to local and saves initial values."""
        self.test_access_token_orig_auth_mode = self.args.auth_mode
        self.test_access_token_orig_username = self.args.username
        self.test_access_token_orig_password = self.args.password
        self.args.auth_mode = "local"

    def set_auth_mode_to_jwt(self):
        """Sets the E2E frameworks authentication mode to JWT."""
        self.args.auth_mode = "jwt"

    def set_auth_mode_to_token(self):
        """Sets the E2E frameworks auth mode to token."""
        self.args.auth_mode = "token"

    def set_username_password_to_token(self):
        """Sets the E2E frameworks username/password to the newly generated API token."""
        self.args.username = self.last_response["data"]["client-id"]
        self.args.password = self.last_response["data"]["client-token"]

    def set_auth_mode_to_initial(self):
        """Sets the E2E frameworks authentication mode back to it's initial value."""
        self.args.auth_mode = self.test_access_token_orig_auth_mode
        self.args.username = self.test_access_token_orig_username
        self.args.password = self.test_access_token_orig_password

    def set_req_data_to_initial_auth(self):
        """Sets the req_data value needed to rever the API settings back to the initial auth mode."""
        return {"authmode": self.test_access_token_orig_auth_mode}


APIE2ETestAccessToken()
