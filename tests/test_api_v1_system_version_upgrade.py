"""Script used to test the /api/v1/system/version/upgrade endpoint."""
import e2e_test_framework


class APIE2ETestSystemVersion(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/version/upgrade endpoint."""
    uri = "/api/v1/system/version/upgrade"
    get_tests = [{"name": "Read available pfSense upgrades"}]


APIE2ETestSystemVersion()
