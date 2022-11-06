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
    # Many methods needed for common test cases
    # pylint: disable=too-many-public-methods

    # CLASS PROPERTIES #
    args = {}
    uid = str(uuid.uuid4())
    url = ""
    uri = ""
    exit_code = 0
    last_request = {}
    last_response = {}
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
        # Loop through each GET req_data and check that it's response is expected
        for test_params in self.get_tests:
            self.pre_get()
            self.get_responses.append(self.make_request("GET", test_params))
            self.post_get()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.args.delay)

    def post(self):
        """Makes a POST request for every POST test found in the 'post_tests' attribute."""
        # Loop through each POST req_data and check that it's response is expected
        for test_params in self.post_tests:
            self.pre_post()
            self.post_responses.append(self.make_request("POST", test_params))
            self.post_post()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.args.delay)

    def put(self):
        """Makes a PUT request for every PUT test found in the 'put_tests' attribute."""
        # Loop through each PUT req_data and check that it's response is expected
        for test_params in self.put_tests:
            self.pre_put()
            self.put_responses.append(self.make_request("PUT", test_params))
            self.post_put()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.args.delay)

    def delete(self):
        """Makes a DELETE request for every DELETE test found in the 'delete_tests' attribute."""
        # Loop through each DELETE req_data and check that it's response is expected
        for test_params in self.delete_tests:
            self.pre_delete()
            self.delete_responses.append(self.make_request("DELETE", test_params))
            self.post_delete()
            # For speed, only delay time if test is a non-200 OK test
            if test_params.get("status", 200) == 200:
                time.sleep(self.args.delay)

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
        # pylint: disable=broad-except    # We don't want tests to exit and prevent later tests
        # pylint: disable=too-many-locals  # Many variables needed for test customization
        # pylint: disable=too-many-branches    # Many branches needed for test customization
        # pylint: disable=too-many-statements    # Many statements needed for test customization

        # Local variables
        method = test_params.get("method", method)    # Allow custom method override
        req_data = test_params.get("req_data", {})
        req_data_callable = getattr(self, test_params.get("req_data_callable", ""), None)
        pre_test_callable = getattr(self, test_params.get("pre_test_callable", ""), None)
        pre_test_exc = None
        post_test_callable = getattr(self, test_params.get("post_test_callable", ""), None)
        post_test_exc = None
        username = test_params.get("username", self.args.username)
        password = test_params.get("password", self.args.password)
        auth_mode = test_params.get("auth_mode", self.args.auth_mode)
        headers = {}
        auth = None
        resp = None

        # Delay this test if the 'delay' parameter is set
        time.sleep(test_params.get("delay", 0))

        # Set authentication headers for local authentication
        if auth_mode == "local":
            auth = (username, password)
        # Set authentication headers for token authentication
        if auth_mode == "token":
            headers = {"Authorization": username + " " + password}
        # Set authentication headers for JWT authentication
        if auth_mode == "jwt":
            headers = {"Authorization": "Bearer " + self.get_jwt(username, password)}

        # When a callable req_data is defined, ensure it is a callable and run the function
        if test_params.get("req_data_callable", ""):
            # Ensure the test is callable, otherwise raise an error
            if callable(req_data_callable):
                # Try to run the callable, if an exception occurs capture it so it can be checked in __check_resp__
                req_data.update(req_data_callable())
            else:
                raise ValueError("Expected req_data_callable to be a valid callable name")

        # When a pre-test callable is defined, ensure it is a callable and run the function
        if test_params.get("pre_test_callable", ""):
            # Ensure the test is callable, otherwise raise an error
            if callable(pre_test_callable):
                # Try to run the callable, if an exception occurs capture it so it can be checked in __check_resp__
                try:
                    pre_test_callable()
                except Exception as exc:
                    pre_test_exc = exc
            else:
                raise ValueError("Expected pre_test_callable to be a valid callable name")

        # Attempt to make the API call, if the request times out print timeout error
        try:
            resp = requests.request(
                method,
                url=self.format_url(test_params.get("uri", self.uri)),
                data=json.dumps(req_data),
                verify=False,
                timeout=self.args.timeout,
                headers=headers,
                auth=auth
            )
        except (requests.exceptions.ConnectTimeout, requests.exceptions.ReadTimeout):
            print(self.__format_msg__(method, test_params, f"Exceeded timeout of {self.args.timeout}s"))

        # If this is a request only execution, just return the request/response object
        if req_only:
            return resp

        # Capture this test's request as the last request
        self.last_request = test_params

        # Try to set the last response, set an empty dict if we couldn't.
        try:
            self.last_response = resp.json()
        except requests.exceptions.JSONDecodeError:
            self.last_response = {}

        # Pause this test if the 'pause' parameter is set
        time.sleep(test_params.get("pause", 0))

        # Check for a post test callable
        if test_params.get("post_test_callable", ""):
            # Ensure the test is callable, otherwise raise an error
            if callable(post_test_callable):
                # Try to run the callable, if an exception occurs capture it so it can be checked in __check_resp__
                try:
                    post_test_callable()
                except Exception as exc:
                    post_test_exc = exc
            else:
                raise ValueError("Expected post_test_callable to be a valid callable name")

        # Otherwise, check if the response is valid
        response_valid = self.__check_resp__(resp, test_params, pre_test_exc=pre_test_exc, post_test_exc=post_test_exc)

        # Return the JSON response when successful
        if response_valid:
            return resp.json()

        return None

    def get_jwt(self, username, password):
        """Requests a new JWT to use for JWT authentication."""
        resp = requests.request(
            "POST",
            url=self.args.scheme + "://" + self.args.host + ":" + str(self.args.port) + "/api/v1/access_token",
            verify=False,
            timeout=self.args.timeout,
            auth=(username, password)
        )

        # Check if the response indicates that JWT is not enabled
        if resp.json().get("return") == 9:
            msg = "JWT IS REQUESTED BUT IS NOT ENABLED AS THE API AUTH MODE"
            print(self.__format_msg__(resp.request.method, {"name": "BUILT-IN JWT REQUEST"}, msg, mode="warning"))
            return ""

        # Return the token if the request was successful
        if resp.json().get("return") != 0:
            # Fail on unexpected response code
            msg = "UNEXPECTED JWT RESPONSE"
            print(self.__format_msg__(resp.request.method, {"name": "BUILT-IN JWT REQUEST"}, msg))
            return ""

        return resp.json()["data"]["token"]

    def pfsense_shell(self, cmd: str):
        """
        Runs a specified shell command on the target pfSense using the /api/v1/diagnostics/command_prompt endpoint
        and returns it's output.
        :param cmd: the shell command to run
        :return: the stdout from the shell command
        """
        # Local variables
        test_params = {
            "uri": "/api/v1/diagnostics/command_prompt",
            "req_data": {"shell_cmd": cmd}
        }

        # Run the API request
        resp = self.make_request("POST", test_params, req_only=True)

        # Only return the response if it was successful
        if self.has_json_response(resp) and resp.status_code == 200:
            return resp.json().get("data").get("cmd_output")

        # Otherwise, raise an error
        raise ConnectionError(f"Failed to run '{cmd}' at '{self.format_url(test_params['uri'])}'")

    @staticmethod
    def has_json_response(resp):
        """Checks that our request's response is valid a JSON string."""
        try:
            resp.json()
            return True
        except json.decoder.JSONDecodeError:
            return False

    @staticmethod
    def has_correct_http_status(resp, test_params):
        """Checks if the HTTP status was the expected value."""
        if resp is not None and int(resp.status_code) == int(test_params.get("status", 200)):
            return True

        return False

    @staticmethod
    def has_correct_return_code(resp, test_params):
        """Checks if the API return code was the expected value."""
        if APIE2ETest.has_json_response(resp) and resp.json()["return"] == test_params.get("return", 0):
            return True

        return False

    @staticmethod
    def has_correct_resp_time(resp, test_params):
        """Checks if response time is within an acceptable threshold. Allow within 1 second variance."""
        if resp.elapsed.total_seconds() < test_params.get("resp_time", 1) + 1:
            return True

        return False

    @staticmethod
    def has_correct_resp_data(resp, test_params):
        """Checks if the response data matches the expected response data set in the test_params."""
        # Check if the resp_data is specified in the test_params
        if test_params.get("resp_data", None):
            # Loop through each of the expected response data values and ensure they're present in the response
            for key, val in test_params.get("resp_data").items():
                # Ensure response data is a dict to proceed
                if isinstance(resp.json().get("data"), dict):
                    # Ensure key is present in the dictionary
                    if key not in resp.json().get("data").keys():
                        return False
                    # Ensure value matches
                    if val == resp.json().get("data").get(key):
                        return False
        # Pass this test if we did not find a mismatch in the response data vs the expected response data
        return True

    @staticmethod
    def has_empty_resp_data(resp, test_params):
        """Checks if a successful request resulted in an empty response data body"""
        # Only check for this if request was successful
        if resp.status_code == 200:
            # Check if response data is emtpy and it was not expected
            if not resp.json().get("data", []) and not test_params.get("resp_data_empty", False):
                return True

        return False

    def __check_resp__(self, resp, test_params, pre_test_exc=None, post_test_exc=None):
        """Checks if the API response is within the test parameters."""
        # Local variables
        valid = False

        # Ensure we received a JSON response
        if not APIE2ETest.has_json_response(resp):
            msg = f"Expected JSON response, received {resp.content}"
            print(self.__format_msg__(resp.request.method, test_params, msg))
        # Ensure response has the correct HTTP status code
        elif not APIE2ETest.has_correct_http_status(resp, test_params):
            received_status = resp.status_code
            expected_status = test_params.get("status", 200)
            msg = f"Expected status code {expected_status}, received {received_status}"
            print(self.__format_msg__(resp.request.method, test_params, msg))
        # Ensure response has the correct API return code
        elif not APIE2ETest.has_correct_return_code(resp, test_params):
            received_return = resp.json()["return"]
            expected_return = test_params.get("return", 0)
            msg = f"Expected return code {expected_return}, received {received_return}"
            print(self.__format_msg__(resp.request.method, test_params, msg))
        # Ensure no exceptions occurred during the pre-test callable
        elif pre_test_exc:
            msg = f"Encountered exception during pre-test callable, received: {pre_test_exc}"
            print(self.__format_msg__(resp.request.method, test_params, msg))
        # Ensure no exceptions occurred during the post-test callable
        elif post_test_exc:
            msg = f"Encountered exception during post-test callable, received: {post_test_exc}"
            print(self.__format_msg__(resp.request.method, test_params, msg))
        # Ensure response data is correct type
        elif not isinstance(resp.json().get("data"), list) and not isinstance(resp.json().get("data"), dict):
            msg = "Expected API response data to be type list or dict"
            print(self.__format_msg__(resp.request.method, test_params, msg))
        # Ensure response data is expected
        elif not APIE2ETest.has_correct_resp_data(resp, test_params):
            msg = "API response did not contain expected response data values"
            print(self.__format_msg__(resp.request.method, test_params, msg))
        # Ensure response data is not empty if request is successful, warn otherwise
        elif APIE2ETest.has_empty_resp_data(resp, test_params):
            msg = "Request was successful but response data was empty"
            print(self.__format_msg__(resp.request.method, test_params, msg, mode="warning"))
            valid = True
        # Ensure response time was within threshold, prints a warning if the response time to was too long
        elif not APIE2ETest.has_correct_resp_time(resp, test_params):
            received_resp_time = resp.elapsed.total_seconds()
            expected_resp_time = test_params.get("resp_time", 1)
            msg = f"Expected response time within {expected_resp_time}s, received {received_resp_time}s"
            print(self.__format_msg__(resp.request.method, test_params, msg, mode="warning"))
            valid = True
        # Otherwise, the response is valid. Print successful test.
        else:
            print(self.__format_msg__(resp.request.method, test_params, "Response is valid", mode="ok"))
            valid = True

        # Print detailed response information if test failed or verbose mode is enabled
        if not valid or self.args.verbose:
            print("RESPONSE STATUS: " + str(resp.status_code))
            print("RESPONSE TIME: " + str(resp.elapsed.total_seconds()) + "s")
            print("RESPONSE DATA: " + resp.content.decode())

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
            '--delay',
            dest="delay",
            type=int,
            default=0,
            help="Delay between tests in seconds"
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
        # Set ASCII color text for the method used
        methods = {
            "GET": "\33[32mGET\33[0m",
            'POST': "\33[33mPOST\33[0m",
            'PUT': "\33[34mPUT\33[0m",
            'DELETE': "\33[31mDELETE\33[0m"
        }

        # Set the URL to the actual URL used in the test, respecting the 'uri' test_param
        url = self.format_url(test_params.get("uri", self.uri))

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
        msg = msg + f" [ {methods[method]} {url} ][{test_params.get('name', 'Unnamed test')}]: {result}"
        return msg
