"""Class used to test the /api/v1/system/config endpoint."""
import e2e_test_framework


class APIE2ETestSystemConfig(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/config endpoint."""
    uri = "/api/v1/system/config"
    get_privileges = ["page-all", "page-diagnostics-backup-restore", "page-diagnostics-command"]
    get_tests = [{"name": "Read the entire pfSense configuration"}]


APIE2ETestSystemConfig()
