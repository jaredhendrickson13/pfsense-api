"""Script used to test the /api/v1/system/version/upgrade endpoint."""
import e2e_test_framework


class APIE2ETestSystemVersion(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/version/upgrade endpoint."""
    uri = "/api/v1/system/version/upgrade"

    get_privileges = ["page-all", "page-dashboard-widgets", "page-diagnostics-command", "page-system-update-settings"]

    get_tests = [
        {
            "name": "Read available pfSense upgrades"
        },
        {
            "name": "Read available pfSense upgrades using cache",
            "req_data": {"use_cache": True}
        },
        {
            "name": "Read available pfSense upgrades without cache",
            "req_data": {"use_cache": False},
            "resp_time": 20
        }
    ]


APIE2ETestSystemVersion()
