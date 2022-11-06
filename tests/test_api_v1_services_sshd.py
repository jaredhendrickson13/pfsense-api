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
import random
import socket
import e2e_test_framework

# Constants
SSH_PORT_TEST_1 = random.randint(20000, 22222)
SSH_KEYONLY_TEST_1 = random.choice(["both", "enabled"])
SSH_AGENTFORWARDING_TEST_1 = True
SSH_PORT_TEST_2 = 22
SSH_KEYONLY_TEST_2 = "disabled"
SSH_AGENTFORWARDING_TEST_2 = False


class APIE2ETestServicesSSHd(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/services/sshd endpoint."""
    uri = "/api/v1/services/sshd"
    get_tests = [{"name": "Read SSHd configuration"}]
    put_tests = [
        {
            "name": "Update SSHD",
            "post_test_callable": "is_sshd_set_test_1",
            "pause": 3,    # Wait a bit for ssh to restart
            "req_data": {
                "enable": True,
                "sshdkeyonly": SSH_KEYONLY_TEST_1,
                "sshdagentforwarding": SSH_AGENTFORWARDING_TEST_1,
                "port": SSH_PORT_TEST_1
            }
        },
        {
            "name": "Disable SSHd",
            "post_test_callable": "is_ssh_disabled",
            "pause": 3,  # Wait a bit for ssh to restart
            "req_data": {
                "enable": False
            }
        },
        {
            "name": "Update and re-enable SSHD",
            "post_test_callable": "is_sshd_set_test_2",
            "pause": 3,  # Wait a bit for ssh to restart
            "req_data": {
                "enable": True,
                "sshdkeyonly": SSH_KEYONLY_TEST_2,
                "sshdagentforwarding": SSH_AGENTFORWARDING_TEST_2,
                "port": SSH_PORT_TEST_2
            }
        },
        {
            "name": "Check enable options constraint",
            "status": 400,
            "return": 2000,
            "req_data": {"enable": "INVALID"}
        },
        {
            "name": "Check port is TCP port constraint",
            "status": 400,
            "return": 2003,
            "req_data": {"port": "INVALID"}
        },
        {
            "name": "Check sshdkeyonly options constraint",
            "status": 400,
            "return": 2001,
            "req_data": {"sshdkeyonly": "INVALID"}
        },
        {
            "name": "Check sshdagentforwarding options constraint",
            "status": 400,
            "return": 2002,
            "req_data": {"sshdagentforwarding": "INVALID"}
        }
    ]

    def is_sshd_set_test_1(self):
        """Checks if the first SSH update we do correctly configures sshd and it is reachable"""
        # Many sshd_config fields need to be checked
        # pylint: disable=too-many-branches

        # Local variables
        sshd_config = self.pfsense_shell("cat /etc/ssh/sshd_config")

        # Ensure the port is correctly set in the sshd_config
        if f"Port {SSH_PORT_TEST_1}" not in sshd_config:
            raise AssertionError(f"Expected 'Port {SSH_PORT_TEST_1}' in sshd_config, got: {sshd_config}")

        # Check if port 22 is not open like expected
        if not self.is_port_open(SSH_PORT_TEST_1):
            raise AssertionError("Expected SSH to be running on port 22")

        # Ensure password or pubkey is allowed if 'disabled' keyonly
        if SSH_KEYONLY_TEST_1 == "disabled":
            # Ensure both password and pubkey are allowed without both being required
            if "PasswordAuthentication yes" not in sshd_config:
                raise AssertionError(f"Expected 'PasswordAuthentication yes' in sshd_config, got: {sshd_config}")
            if "PubkeyAuthentication yes" not in sshd_config:
                raise AssertionError(f"Expected 'PubkeyAuthentication yes' in sshd_config, got: {sshd_config}")
            if "AuthenticationMethods publickey,password" in sshd_config:
                raise AssertionError("Unexpected 'AuthenticationMethods publickey,password' in sshd_config")
        # Ensure password or pubkey is allowed if 'both' keyonly
        if SSH_KEYONLY_TEST_1 == "both":
            # Ensure both password and pubkey are allowed without both being required
            if "AuthenticationMethods publickey,password" not in sshd_config:
                raise AssertionError(
                    f"Expected 'AuthenticationMethods publickey,password' in sshd_config, got: {sshd_config}"
                )
        # Ensure only pubkey is allowed if 'enabled' keyonly
        if SSH_KEYONLY_TEST_1 == "enabled":
            # Ensure both password and pubkey are allowed without both being required
            if "PasswordAuthentication no" not in sshd_config:
                raise AssertionError(f"Expected 'PasswordAuthentication no' in sshd_config, got: {sshd_config}")
            if "PubkeyAuthentication yes" not in sshd_config:
                raise AssertionError(f"Expected 'PubkeyAuthentication yes' in sshd_config, got: {sshd_config}")

        # Ensure AllowAgentForwarding is properly set
        if SSH_AGENTFORWARDING_TEST_1 and "AllowAgentForwarding yes" not in sshd_config:
            raise AssertionError(f"Expected 'AllowAgentForwarding yes' in sshd_config, got: {sshd_config}")
        if not SSH_AGENTFORWARDING_TEST_1 and "AllowAgentForwarding no" not in sshd_config:
            raise AssertionError(f"Expected 'AllowAgentForwarding no' in sshd_config, got: {sshd_config}")

    def is_sshd_set_test_2(self):
        """Checks if the second SSH update we do correctly configures sshd and it is reachable"""
        # Many sshd_config fields need to be checked
        # pylint: disable=too-many-branches

        # Local variables
        sshd_config = self.pfsense_shell("cat /etc/ssh/sshd_config")

        # Ensure the port is correctly set in the sshd_config
        if f"Port {SSH_PORT_TEST_2}" not in sshd_config:
            raise AssertionError(f"Expected 'Port {SSH_PORT_TEST_2}' in sshd_config, got: {sshd_config}")

        # Check if port 22 is not open like expected
        if not self.is_port_open(SSH_PORT_TEST_2):
            raise AssertionError("Expected SSH to be running on port 22")

        # Ensure password or pubkey is allowed if 'disabled' keyonly
        if SSH_KEYONLY_TEST_2 == "disabled":
            # Ensure both password and pubkey are allowed without both being required
            if "PasswordAuthentication yes" not in sshd_config:
                raise AssertionError(f"Expected 'PasswordAuthentication yes' in sshd_config, got: {sshd_config}")
            if "PubkeyAuthentication yes" not in sshd_config:
                raise AssertionError(f"Expected 'PubkeyAuthentication yes' in sshd_config, got: {sshd_config}")
            if "AuthenticationMethods publickey,password" in sshd_config:
                raise AssertionError("Unexpected 'AuthenticationMethods publickey,password' in sshd_config")
        # Ensure password or pubkey is allowed if 'both' keyonly
        if SSH_KEYONLY_TEST_2 == "both":
            # Ensure both password and pubkey are allowed without both being required
            if "AuthenticationMethods publickey,password" not in sshd_config:
                raise AssertionError(
                    f"Expected 'AuthenticationMethods publickey,password' in sshd_config, got: {sshd_config}"
                )
        # Ensure only pubkey is allowed if 'enabled' keyonly
        if SSH_KEYONLY_TEST_2 == "enabled":
            # Ensure both password and pubkey are allowed without both being required
            if "PasswordAuthentication no" not in sshd_config:
                raise AssertionError(f"Expected 'PasswordAuthentication no' in sshd_config, got: {sshd_config}")
            if "PubkeyAuthentication yes" not in sshd_config:
                raise AssertionError(f"Expected 'PubkeyAuthentication yes' in sshd_config, got: {sshd_config}")

        # Ensure AllowAgentForwarding is properly set
        if SSH_AGENTFORWARDING_TEST_2 and "AllowAgentForwarding yes" not in sshd_config:
            raise AssertionError(f"Expected 'AllowAgentForwarding yes' in sshd_config, got: {sshd_config}")
        if not SSH_AGENTFORWARDING_TEST_2 and "AllowAgentForwarding no" not in sshd_config:
            raise AssertionError(f"Expected 'AllowAgentForwarding no' in sshd_config, got: {sshd_config}")

    def is_ssh_disabled(self):
        """Checks if SSH is unreachable"""
        # Ensure neither of the previous ports we set are reachable.
        if self.is_port_open(SSH_PORT_TEST_1) or self.is_port_open(SSH_PORT_TEST_2):
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
