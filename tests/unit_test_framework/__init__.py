import requests
import argparse
import json
import uuid
import sys
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
    auth_payload = {}
    get_payloads = []
    post_payloads = []
    put_payloads = []
    delete_payloads = []

    # CLASS METHODS #
    def __init__(self):
        self.__start_argparse__()
        self.auth_payload = {"client-id": self.args.username, "client-token": self.args.password}
        self.url = self.args.scheme + "://" + self.args.host + ":" + str(self.args.port) + self.url
        self.post()
        self.get()
        self.put()
        self.delete()
        sys.exit(self.exit_code)

    def get(self):
        # Loop through each GET payload and check that it's response is expected
        for payload in self.get_payloads:
            self.make_request("GET", payload)
            time.sleep(self.time_delay)

    def post(self):
        # Loop through each POST payload and check that it's response is expected
        for payload in self.post_payloads:
            self.make_request("POST", payload)
            time.sleep(self.time_delay)

    def put(self):
        # Loop through each PUT payload and check that it's response is expected
        for payload in self.put_payloads:
            self.make_request("PUT", payload)
            time.sleep(self.time_delay)

    def delete(self):
        # Loop through each DELETE payload and check that it's response is expected
        for payload in self.delete_payloads:
            self.make_request("DELETE", payload)
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
        payload.update(self.auth_payload)
        success = False
        try:
            req = requests.request(
                method,
                url=self.url,
                data=json.dumps(payload),
                verify=False,
                timeout=self.args.timeout
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
