"""Script used to test the /api/v1/system/version/upgrade endpoint."""
import e2e_test_framework


class APIE2ETestSystemVersion(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/version/upgrade endpoint."""
    uri = "/api/v1/system/version/upgrade"
    get_tests = [
        {
            "name": "Read available pfSense upgrades"
        },
        {
            "name": "Read available pfSense upgrades using cache",
            "payload": {"use_cache": True}
        },
        {
            "name": "Read available pfSense upgrades without cache",
            "payload": {"use_cache": False},
            "resp_time": 20
        }
    ]


APIE2ETestSystemVersion()
