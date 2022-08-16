"""Script used to test the /api/v1/system/api/error endpoint."""
import e2e_test_framework


class APIE2ETestSystemAPIError(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/api/error endpoint."""
    uri = "/api/v1/system/api/error"
    get_tests = [{"name": "Read API errors"}]


APIE2ETestSystemAPIError()
