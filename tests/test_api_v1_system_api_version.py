"""Script used to test the /api/v1/system/api/version endpoint."""
import e2e_test_framework


class APIE2ETestSystemAPIVersion(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/api/version endpoint."""
    uri = "/api/v1/system/api/version"
    get_tests = [{"name": "Read API version"}]


APIE2ETestSystemAPIVersion()
