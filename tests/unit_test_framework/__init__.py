# Copyright 2020 Jared Hendrickson
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
    exit_code = 1
    time_delay = 1
    get_payloads = []
    post_payloads = []
    put_payloads = []
    delete_payloads = []
    get_responses = []
    post_responses = []
    put_responses = []
    delete_responses = []

    # CLASS METHODS #
    def __init__(self):
        signal.signal(signal.SIGINT, APIUnitTest.__safe_escape__)
        self.__start_argparse__()
        self.url = self.args.scheme + "://" + self.args.host + ":" + str(self.args.port) + self.url
        self.auth_payload = {"client-id": self.args.username, "client-token": self.args.password}

        # Run unit tests and exit on corresponding status code
        self.post()
        self.get()
        self.put()
        self.delete()
        sys.exit(self.exit_code)

    def get(self):
        # Loop through each GET payload and check that it's response is expected
        for payload in self.get_payloads:
            self.pre_get()
            self.get_responses.append(self.make_request("GET", payload))
            self.post_get()
            time.sleep(self.time_delay)

    def post(self):
        # Loop through each POST payload and check that it's response is expected
        for payload in self.post_payloads:
            self.pre_post()
            self.post_responses.append(self.make_request("POST", payload))
            self.post_post()
            time.sleep(self.time_delay)

    def put(self):
        # Loop through each PUT payload and check that it's response is expected
        for payload in self.put_payloads:
            self.pre_put()
            self.put_responses.append(self.make_request("PUT", payload))
            self.post_put()
            time.sleep(self.time_delay)

    def delete(self):
        # Loop through each DELETE payload and check that it's response is expected
        for payload in self.delete_payloads:
            self.pre_delete()
            self.delete_responses.append(self.make_request("DELETE", payload))
            self.post_delete()
            time.sleep(self.time_delay)

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
            default=10,
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

    def make_request(self, method, payload):
        success = False

        # Add our authentication payload values if our auth mode requires
        if self.args.auth_mode != "jwt":
            payload.update(self.auth_payload)
            headers = {}
        else:
            headers = {"Authorization": "Bearer " + self.__request_jwt__()}

        try:
            req = requests.request(
                method,
                url=self.url,
                data=json.dumps(payload),
                verify=False,
                timeout=self.args.timeout,
                headers=headers
            )
        except requests.exceptions.ConnectTimeout:
            print(self.__format_msg__(method, "Connection timed out"))
            return None

        # Check if our HTTP status code is expected
        if req is not None and req.status_code == 200:
            # Try to decode our request as JSON
            try:
                req.json()
                is_json = True
            except json.decoder.JSONDecodeError:
                is_json = False

            # Check if our response is JSON, if so proceed. Otherwise set error.
            if is_json:
                # Check if our API responses return code is 0. Otherwise set error.
                if req.json()["return"] == 0:
                    msg = self.__format_msg__(method,  "Response is valid", error=False)
                    success = True
                else:
                    msg = self.__format_msg__(method, "Received non-zero return " + str(req.json()["return"]))
            else:
                msg = self.__format_msg__(method, "Expected JSON response, recieved " + str(req.content))
        else:
            msg = self.__format_msg__(method, "Expected status code 200, received " + str(req.status_code))

        # Print our message to the console, if an error occurred
        print(msg)

        # Print request output if verbose mode
        if self.args.verbose:
            print(req.content.decode())

        # Set exit code to one if this test failed
        if success:
            self.exit_code = 0
            return req.json()

    def __format_msg__(self, method, descr, error=True):
        methods = {
            "GET": "\33[32mGET\33[0m",
            'POST': "\33[33mPOST\33[0m",
            'PUT': "\33[34mPUT\33[0m",
            'DELETE': "\33[31mDELETE\33[0m"
        }
        msg = "\33[31mFAILED -->\33[0m" if error else "\33[32mOK ------>\33[0m"
        msg = msg + " [ " + methods[method] + " " + self.url + " ]: " + descr
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

    @staticmethod
    def __safe_escape__(signum, frame):
        try:
            os._exit(0)
        except OSError:
            sys.exit(0)

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