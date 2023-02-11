"""Script used to test the /api/v1/system/version endpoint."""
import e2e_test_framework


class APIE2ETestSystemVersion(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/version endpoint."""
    uri = "/api/v1/system/version"
    get_privileges = ["page-all", "page-dashboard-widgets", "page-diagnostics-command", "page-system-update-settings"]
    get_tests = [{"name": "Read pfSense version"}]


APIE2ETestSystemVersion()
