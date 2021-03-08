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

import requests
import argparse
import json
import uuid
import sys
import os
import signal
import time
from urllib3.exceptions import InsecureRequestWarning
requests.packages.urllib3.disable_warnings(category=InsecureRequestWarning)


class APIUnitTest:
    # CLASS PROPERTIES #
    args = {}
    uid = str(uuid.uuid4())
    url = ""
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
        self.url = self.args.scheme + "://" + self.args.host + ":" + str(self.args.port) + self.url
        self.auth_payload = {"client-id": self.args.username, "client-token": self.args.password}

        # Run unit tests and exit on corresponding status code
        try:
            self.post()
            self.get()
            self.put()
            self.delete()
            sys.exit(self.exit_code)
        except KeyboardInterrupt:
            sys.exit(1)

    def get(self):
        # Loop through each GET payload and check that it's response is expected
        for test_params in self.get_tests:
            self.pre_get()
            self.get_responses.append(self.make_request("GET", test_params))
            self.post_get()
            time.sleep(self.time_delay)

    def post(self):
        # Loop through each POST payload and check that it's response is expected
        for test_params in self.post_tests:
            self.pre_post()
            self.post_responses.append(self.make_request("POST", test_params))
            self.post_post()
            time.sleep(self.time_delay)

    def put(self):
        # Loop through each PUT payload and check that it's response is expected
        for test_params in self.put_tests:
            self.pre_put()
            self.put_responses.append(self.make_request("PUT", test_params))
            self.post_put()
            time.sleep(self.time_delay)

    def delete(self):
        # Loop through each DELETE payload and check that it's response is expected
        for test_params in self.delete_tests:
            self.pre_delete()
            self.delete_responses.append(self.make_request("DELETE", test_params))
            self.post_delete()
            time.sleep(self.time_delay)

    # PRE/POST REQUEST METHODS. These are intended to be overwritten by a child class.
    def pre_post(self):
        pass

    def post_post(self):
        pass

    def pre_get(self):
        pass

    def post_get(self):
        pass

    def pre_put(self):
        pass

    def post_put(self):
        pass

    def pre_delete(self):
        pass

    def post_delete(self):
        pass

    def make_request(self, method, test_params):
        # Create authentication payload for local authentication
        if self.args.auth_mode == "local":
            test_params["payload"] = test_params.get("payload", {})
            test_params["payload"].update(self.auth_payload)
            headers = {}
        # Create authentication headers for token authentication
        elif self.args.auth_mode == "token":
            headers = {"Authorization": self.args.username + " " + self.args.password}
        # Create authentication headers for JWT authentication
        elif self.args.auth_mode == "jwt":
            headers = {"Authorization": "Bearer " + self.__request_jwt__()}

        # Attempt to make the API call, if the request times out print timeout error
        try:
            req = requests.request(
                method,
                url=self.url,
                data=json.dumps(test_params.get("payload", {})),
                verify=False,
                timeout=self.args.timeout,
                headers=headers
            )
        except (requests.exceptions.ConnectTimeout, requests.exceptions.ReadTimeout):
            print(self.__format_msg__(method, test_params, "Exceeded timeout of {t}s".format(t=self.args.timeout)))
            return None

        if self.check_response(req, test_params, verbose=self.args.verbose):
            return req.json()

    @staticmethod
    def has_json_response(req):
        # Check if our request's response is valid JSON
        try:
            req.json()
            return True
        except json.decoder.JSONDecodeError:
            return False

    @staticmethod
    def has_correct_http_status(req, test_params):
        # Check if our HTTP status was expect
        if req != None and int(req.status_code) == int(test_params.get("status", 200)):
            return True
        else:
            return False

    @staticmethod
    def has_correct_return_code(req, test_params):
        # Check if our HTTP status was expect
        if APIUnitTest.has_json_response(req) and req.json()["return"] == test_params.get("return", 0):
            return True
        else:
            return False

    @staticmethod
    def has_correct_resp_time(req, test_params):
        # Check if response time is within an acceptable threshold. Allow within 1 second variance.
        if req.elapsed.total_seconds() < test_params.get("resp_time", 1) + 1:
            return True
        else:
            return False

    def check_response(self, req, test_params, verbose=False):
        # Local variables
        valid = False

        # Run each check and print the results
        if not APIUnitTest.has_json_response(req):
            msg = "Expected JSON response, received {content}".format(content=req.content)
            print(self.__format_msg__(req.request.method, msg))
        elif not APIUnitTest.has_correct_http_status(req, test_params):
            received_status = req.status_code
            expected_status = test_params.get("status", 200)
            msg = "Expected status code {e}, received {r}".format(e=expected_status, r=received_status)
            print(self.__format_msg__(req.request.method, test_params, msg))
        elif not APIUnitTest.has_correct_return_code(req, test_params):
            received_return = req.json()["return"]
            expected_return = test_params.get("return", 0)
            msg = "Expected return code {e}, received {r}".format(e=expected_return, r=received_return)
            print(self.__format_msg__(req.request.method, test_params, msg))
        elif not APIUnitTest.has_correct_resp_time(req, test_params):
            received_resp_time = req.elapsed.total_seconds()
            expected_resp_time = test_params.get("resp_time", 1)
            msg = "Expected response time within {e}s, received {r}s".format(e=expected_resp_time, r=received_resp_time)
            print(self.__format_msg__(req.request.method, test_params, msg, mode="warning"))
        else:
            print(self.__format_msg__(req.request.method, test_params, "Response is valid", mode="ok"))
            valid = True

        # Print request output if verbose mode
        if verbose:
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
                raise argparse.ArgumentTypeError("%s is out of range, choose from [1-65535]" % value)
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
            default=12,
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
            raise ValueError("Unknown `mode` provided to APIUnitTest.__format_msg__")

        # Piece the message together
        msg = msg + " [ {m} {u} ][{n}]: {r}".format(
            m=methods[method],
            u=self.url,
            n=test_params.get("name", "Unnamed test"),
            r=result
        )
        return msg

    def __request_jwt__(self):
        try:
            req = requests.request(
                "POST",
                url=self.args.scheme + "://" + self.args.host + ":" + str(self.args.port) + "/api/v1/access_token",
                data=json.dumps({"client-id": self.args.username, "client-token": self.args.password}),
                verify=False,
                timeout=self.args.timeout
            )
            return req.json()["data"]["token"]
        except Exception:
            return ""
