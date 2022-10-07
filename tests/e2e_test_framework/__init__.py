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
"""Module for the e2e test framework that is used to test pfSense-API."""
import argparse
import json
import sys
import time
import uuid
import requests
import urllib3

# Disable insecure request warnings as they cause a lot of noise in the tests.
urllib3.disable_warnings(category=urllib3.exceptions.InsecureRequestWarning)


class APIE2ETest:
    """Base class for the e2e test framework that is used to test pfSense-API."""
    # CLASS PROPERTIES #
    args = {}
    uid = str(uuid.uuid4())
    url = ""
    uri = ""
    time_delay = 1
    exit_code = 0
    get_tests = []
    post_tests = []
    put_tests = []
    delete_tests = []
    get_responses = []
    post_responses = []
    put_responses = []
    delete_responses = []

    # CLASS METHODS #
    def __init__(self):
        self.__start_argparse__()
        self.url = self.format_url(self.uri)
        self.auth_payload = {"client-id": self.args.username, "client-token": self.args.password}

        # Run E2E tests and exit on corresponding status code
        try:
            self.post()
            self.get()
            self.put()
            self.delete()
            self.custom_tests()
            sys.exit(self.exit_code)
        except KeyboardInterrupt:
            sys.exit(1)

    def format_url(self, uri):
        """Formats the object's attributes into a fully formatted URL"""
        return self.args.scheme + "://" + self.args.host + ":" + str(self.args.port) + uri

    def get(self):
        """Makes a GET request for every GET test found in the 'get_tests' attribute."""
        # Loop through each GET payload and check that it's response is expected
        for test_params in self.get_tests:
            self.pre_get()
            self.get_responses.append(self.make_request("GET", test_params))
            self.post_get()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.time_delay)

    def post(self):
        """Makes a POST request for every POST test found in the 'post_tests' attribute."""
        # Loop through each POST payload and check that it's response is expected
        for test_params in self.post_tests:
            self.pre_post()
            self.post_responses.append(self.make_request("POST", test_params))
            self.post_post()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.time_delay)

    def put(self):
        """Makes a PUT request for every PUT test found in the 'put_tests' attribute."""
        # Loop through each PUT payload and check that it's response is expected
        for test_params in self.put_tests:
            self.pre_put()
            self.put_responses.append(self.make_request("PUT", test_params))
            self.post_put()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.time_delay)

    def delete(self):
        """Makes a DELETE request for every DELETE test found in the 'delete_tests' attribute."""
        # Loop through each DELETE payload and check that it's response is expected
        for test_params in self.delete_tests:
            self.pre_delete()
            self.delete_responses.append(self.make_request("DELETE", test_params))
            self.post_delete()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.time_delay)

    def custom_tests(self):
        """Allows child classes to specify custom tests. This is inteded to be overwritten by the child class."""

    # PRE/POST REQUEST METHODS. These are intended to be overwritten by a child class.
    def pre_post(self):
        """
        Method that is run BEFORE any POST request is called. This method is intended to be overwritten by a subclass.
        """

    def post_post(self):
        """
        Method that is run AFTER any POST request is called. This method is intended to be overwritten by a subclass.
        """

    def pre_get(self):
        """
        Method that is run BEFORE any GET request is called. This method is intended to be overwritten by a subclass.
        """

    def post_get(self):
        """
        Method that is run AFTER any GET request is called. This method is intended to be overwritten by a subclass.
        """

    def pre_put(self):
        """
        Method that is run BEFORE any PUT request is called. This method is intended to be overwritten by a subclass.
        """

    def post_put(self):
        """
        Method that is run AFTER any PUT request is called. This method is intended to be overwritten by a subclass.
        """

    def pre_delete(self):
        """
        Method that is run BEFORE any DELETE request is called. This method is intended to be overwritten by a subclass.
        """

    def post_delete(self):
        """
        Method that is run AFTER any DELETE request is called. This method is intended to be overwritten by a subclass.
        """

    def make_request(self, method, test_params, req_only=False):
        """Makes an API request based on the test's parameters."""
        # Local variables
        method = test_params.get("method", method)    # Allow custom method override

        # Create authentication payload for local authentication
        if self.args.auth_mode == "local":
            test_params["payload"] = test_params.get("payload", {})
            test_params["payload"].update(test_params.get("auth_payload", self.auth_payload))
            headers = {}
        # Create authentication headers for token authentication
        elif self.args.auth_mode == "token":
            headers = {"Authorization": self.args.username + " " + self.args.password}
        # Create authentication headers for JWT authentication
        elif self.args.auth_mode == "jwt":
            headers = {
                "Authorization": "Bearer " + self.__request_jwt__(test_params.get("auth_payload", self.auth_payload))
            }

        # Attempt to make the API call, if the request times out print timeout error
        try:
            req = requests.request(
                test_params.get("method", method),
                url=self.format_url(test_params.get("uri", self.uri)),
                data=json.dumps(test_params.get("payload", {})),
                verify=False,
                timeout=self.args.timeout,
                headers=headers
            )
        except (requests.exceptions.ConnectTimeout, requests.exceptions.ReadTimeout):
            print(self.__format_msg__(method, test_params, f"Exceeded timeout of {self.args.timeout}s"))

        # If this is a request only execution, just return the request/response object
        if req_only:
            return req
        # Otherwise, check if the response is valid
        if self.check_response(req, test_params, verbose=self.args.verbose):
            return req.json()

        return None

    @staticmethod
    def has_json_response(req):
        """Checks that our request's response is valid a JSON string."""
        try:
            req.json()
            return True
        except json.decoder.JSONDecodeError:
            return False

    @staticmethod
    def has_correct_http_status(req, test_params):
        """Checks if the HTTP status was the expected value."""
        if req is not None and int(req.status_code) == int(test_params.get("status", 200)):
            return True

        return False

    @staticmethod
    def has_correct_return_code(req, test_params):
        """Checks if the API return code was the expected value."""
        if APIE2ETest.has_json_response(req) and req.json()["return"] == test_params.get("return", 0):
            return True

        return False

    @staticmethod
    def has_correct_resp_time(req, test_params):
        """Checks if response time is within an acceptable threshold. Allow within 1 second variance."""
        if req.elapsed.total_seconds() < test_params.get("resp_time", 1) + 1:
            return True

        return False

    def check_response(self, req, test_params, verbose=False):
        """Checks if the API response is within the test parameters."""
        # Local variables
        valid = False

        # Run each check and print the results
        if not APIE2ETest.has_json_response(req):
            msg = f"Expected JSON response, received {req.content}"
            print(self.__format_msg__(req.request.method, test_params, msg))
        elif not APIE2ETest.has_correct_http_status(req, test_params):
            received_status = req.status_code
            expected_status = test_params.get("status", 200)
            msg = f"Expected status code {expected_status}, received {received_status}"
            print(self.__format_msg__(req.request.method, test_params, msg))
        elif not APIE2ETest.has_correct_return_code(req, test_params):
            received_return = req.json()["return"]
            expected_return = test_params.get("return", 0)
            msg = f"Expected return code {expected_return}, received {received_return}"
            print(self.__format_msg__(req.request.method, test_params, msg))
        elif not APIE2ETest.has_correct_resp_time(req, test_params):
            received_resp_time = req.elapsed.total_seconds()
            expected_resp_time = test_params.get("resp_time", 1)
            msg = f"Expected response time within {expected_resp_time}s, received {received_resp_time}s"
            print(self.__format_msg__(req.request.method, test_params, msg, mode="warning"))
            valid = True
        else:
            print(self.__format_msg__(req.request.method, test_params, "Response is valid", mode="ok"))
            valid = True

        # Print detailed response information if test failed or verbose mode is enabled
        if not valid or verbose:
            print("RESPONSE STATUS: " + str(req.status_code))
            print("RESPONSE TIME: " + str(req.elapsed.total_seconds()) + "s")
            print("RESPONSE DATA: " + req.content.decode())

        self.exit_code = 1 if not valid else self.exit_code
        return valid

    def __start_argparse__(self):
        # Custom port type for argparse
        def port(value_string):
            value = int(value_string)
            if value not in range(1, 65535):
                raise argparse.ArgumentTypeError(f"{value} is out of range, choose from [1-65535]")
            return value

        parser = argparse.ArgumentParser(
            description="Check pfSense API's '" + self.url + "' endpoint for correct functionality."
        )
        parser.add_argument(
            '--host',
            dest="host",
            type=str,
            required=True,
            help="The host to connect to"
        )
        parser.add_argument(
            '--port',
            dest="port",
            type=port,
            default=443,
            help="The port to use when connecting",
            metavar="{1-65535}"
        )
        parser.add_argument(
            '--scheme',
            dest="scheme",
            type=str,
            choices=["http", "https"],
            default="https",
            help="The URL scheme to use when connecting"
        )
        parser.add_argument(
            '--auth_mode',
            dest="auth_mode",
            type=str,
            choices=["local", "token", "jwt"],
            default="local",
            help="The API authentication mode to use."
        )
        parser.add_argument(
            '--username',
            dest="username",
            type=str,
            default="admin",
            help='Username to authenticate as.'
        )
        parser.add_argument(
            '--password',
            dest="password",
            type=str,
            default="pfsense",
            help='Password to authenticate with'
        )
        parser.add_argument(
            '--timeout',
            dest="timeout",
            type=int,
            default=35,
            help="Connection timeout limit in seconds"
        )
        parser.add_argument(
            '--verbose',
            dest="verbose",
            action="store_true",
            required=False,
            help='Display verbose output'
        )
        self.args = parser.parse_args()

    def __format_msg__(self, method, test_params, result, mode="failed"):
        methods = {
            "GET": "\33[32mGET\33[0m",
            'POST': "\33[33mPOST\33[0m",
            'PUT': "\33[34mPUT\33[0m",
            'DELETE': "\33[31mDELETE\33[0m"
        }

        # Check the mode and format the message accordingly
        if mode == "failed":
            msg = "\33[31mFAILED -->\33[0m"
        elif mode == "ok":
            msg = "\33[32mOK ------>\33[0m"
        elif mode == "warning":
            msg = "\33[33mWARNING ->\33[0m"
        else:
            raise ValueError("Unknown `mode` provided to APIE2ETest.__format_msg__")

        # Piece the message together
        msg = msg + f" [ {methods[method]} {self.url} ][{test_params.get('name', 'Unnamed test')}]: {result}"
        return msg

    def __request_jwt__(self, auth_payload):
        req = requests.request(
            "POST",
            url=self.args.scheme + "://" + self.args.host + ":" + str(self.args.port) + "/api/v1/access_token",
            data=json.dumps(auth_payload),
            verify=False,
            timeout=self.args.timeout
        )

        # Check if the response indicates that JWT is not enabled
        if req.json().get("return") == 9:
            msg = "JWT IS REQUESTED BUT IS NOT ENABLED AS THE API AUTH MODE"
            print(self.__format_msg__(req.request.method, {"name": "BUILT-IN JWT REQUEST"}, msg, mode="warning"))
            return ""

        return req.json()["data"]["token"]
