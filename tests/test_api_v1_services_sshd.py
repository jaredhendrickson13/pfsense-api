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
"""Script used to test the /api/v1/services/sshd endpoint."""
import socket
import time
import e2e_test_framework


class APIE2ETestServicesSSHd(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/sshd endpoint."""
    uri = "/api/v1/services/sshd"
    get_tests = [{"name": "Read SSHd configuration"}]
    put_tests = [
        {
            "name": "Start SSH on port 2222",
            "post_test_callable": "check_ssh_reachable_on_2222",
            "req_data": {
                "enable": True,
                "sshdkeyonly": "both",
                "sshdagentforwarding": False,
                "port": 2222
            }
        },
        {
            "name": "Disable SSHd",
            "post_test_callable": "check_ssh_reachable",
            "req_data": {
                "enable": False
            }
        },
        {
            "name": "Update and enable SSH on port 22",
            "post_test_callable": "check_ssh_reachable_on_22",
            "req_data": {
                "enable": True,
                "sshdkeyonly": "disabled",
                "sshdagentforwarding": True,
                "port": 22
            }
        }
    ]

    def check_ssh_reachable_on_22(self):
        """Checks if SSH is reachable on port 22"""
        # Allow a few seconds for SSH to start
        time.sleep(3)

        # Check if port 22 is not open like expected
        if not self.is_port_open(22):
            raise AssertionError("Expected SSH to be running on port 22")

    def check_ssh_reachable_on_2222(self):
        """Checks if SSH is reachable on port 2222"""
        # Allow a few seconds for SSH to start
        time.sleep(3)

        # Check if port 2222 is not open like expected
        if not self.is_port_open(2222):
            raise AssertionError("Expected SSH to be running on port 2222")

    def check_ssh_unreachable(self):
        """Checks if SSH is unreachable"""
        # Ensure neither of the previous ports we set are reachable.
        if self.is_port_open(22) or self.is_port_open(2222):
            raise AssertionError("Expected SSH to be unreachable because it is disabled.")

    def is_port_open(self, port):
        """Checks if a specified port is open on the target host."""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(1)
        try:
            sock.connect((self.args.host, int(port)))
            sock.shutdown(2)
            return True
        except (TimeoutError, ConnectionError):
            return False


APIE2ETestServicesSSHd()
