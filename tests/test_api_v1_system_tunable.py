"""Script used to test the /api/v1/system/tunable endpoint."""
import random

import e2e_test_framework

# Constants
TUNABLE_OID = "net.link.bridge.pfil_bridge"
TUNABLE_VALUE_CREATE = random.randint(1, 9)
TUNABLE_VALUE_UPDATE = random.randint(1, 9)


class APIE2ETestTunable(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/tunable endpoint."""
    uri = "/api/v1/system/tunable"
    get_tests = [{"name": "Read system tunables"}]
    post_tests = [
        {
            "name": "Create system tunable",
            "post_test_callable": "is_tunable_created",
            "req_data": {"tunable": TUNABLE_OID, "value": TUNABLE_VALUE_CREATE, "descr": "E2E test"}
        }
    ]
    put_tests = [
        {
            "name": "Update system tunable",
            "post_test_callable": "is_tunable_updated",
            "req_data": {"id": TUNABLE_OID, "value": TUNABLE_VALUE_UPDATE, "descr": "E2E test updated"}
        }
    ]
    delete_tests = [
        {
            "name": "Delete system tunable",
            "post_test_callable": "is_tunable_deleted",
            "req_data": {"id": TUNABLE_OID}
        }
    ]

    def is_tunable_created(self):
        """Checks if a system tunable is correctly created via sysctl."""
        # Local variables
        sysctl_out = self.pfsense_shell(f"/sbin/sysctl {TUNABLE_OID}")

        # Ensure the OID is set to the correct value
        if sysctl_out != f"{TUNABLE_OID}: {TUNABLE_VALUE_CREATE}":
            raise AssertionError(f"Expected tunable '{TUNABLE_OID}' to be {TUNABLE_VALUE_CREATE}, got {sysctl_out}")

    def is_tunable_updated(self):
        """Checks if a system tunable is correctly updated via sysctl."""
        # Local variables
        sysctl_out = self.pfsense_shell(f"/sbin/sysctl {TUNABLE_OID}")

        # Ensure the OID is set to the correct value
        if sysctl_out != f"{TUNABLE_OID}: {TUNABLE_VALUE_UPDATE}":
            raise AssertionError(f"Expected tunable '{TUNABLE_OID}' to be {TUNABLE_VALUE_UPDATE}, got {sysctl_out}")

    def is_tunable_deleted(self):
        """Checks if a system tunable is successfully deleted via sysctl (or reverted to default)."""
        # Local variables
        sysctl_out = self.pfsense_shell(f"/sbin/sysctl {TUNABLE_OID}")

        # Ensure the OID is set to the correct value
        if sysctl_out == f"{TUNABLE_OID}: {TUNABLE_VALUE_UPDATE}":
            raise AssertionError(f"Expected tunable '{TUNABLE_OID}' to be deleted, got {sysctl_out}")


APIE2ETestTunable()
