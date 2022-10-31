"""Script used to test the /api/v1/user/auth_server endpoint."""
import e2e_test_framework


class APIE2ETestUserAuthServer(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/user/auth_server endpoint."""
    uri = "/api/v1/user/auth_server"
    get_tests = [{"name": "Read all auth servers", "resp_data_empty": True}]


APIE2ETestUserAuthServer()
