"""Script used to test the /api/v1/system/hostname endpoint."""
import random

import e2e_test_framework

# Constants
HOSTNAME_UPDATE = f"{random.randint(10, 99)}-pfsense-api-e2e"
DOMAIN_UPDATE = "example.com"


class APIE2ETestSystemHostname(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/hostname endpoint."""
    uri = "/api/v1/system/hostname"
    get_tests = [{"name": "Read the system hostname"}]
    put_tests = [
        {
            "name": "Update system hostname",
            "post_test_callable": "is_hostname_updated",
            "req_data": {"hostname": HOSTNAME_UPDATE, "domain": DOMAIN_UPDATE},
            "resp_time": 10    # Allow a few seconds for DNS services to reload
        },
        {
            "name": "Test hostname validation",
            "status": 400,
            "return": 1000,
            "req_data": {"hostname": "!@#$", "domain": "test.com"}
        },
        {
            "name": "Test domain validation",
            "status": 400,
            "return": 1001,
            "req_data": {"hostname": "pfsense-api", "domain": "!@#$"}
        }
    ]

    def is_hostname_updated(self):
        """Checks if the system hostname was actually update using the 'hostname' shell command"""
        # Local variables
        hostname_out = self.pfsense_shell("hostname")

        # Ensure the expected hostname is set
        if hostname_out != f"{HOSTNAME_UPDATE}.{DOMAIN_UPDATE}":
            raise AssertionError(f"Expected hostname to be set to {HOSTNAME_UPDATE}.{DOMAIN_UPDATE}")


APIE2ETestSystemHostname()
