"""Script used to test the /api/v1/system/hostname endpoint."""
import e2e_test_framework


class APIE2ETestSystemHostname(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/hostname endpoint."""
    uri = "/api/v1/system/hostname"
    get_tests = [{"name": "Read the system hostname"}]
    put_tests = [
        {
            "name": "Update system hostname",
            "payload": {"hostname": "pfsense-api", "domain": "test.com"},
            "resp_time": 10    # Allow a few seconds for DNS services to reload
        },
        {
            "name": "Test hostname validation",
            "status": 400,
            "return": 1000,
            "payload": {"hostname": "!@#$", "domain": "test.com"}
        },
        {
            "name": "Test domain validation",
            "status": 400,
            "return": 1001,
            "payload": {"hostname": "pfsense-api", "domain": "!@#$"}
        }
    ]


APIE2ETestSystemHostname()
