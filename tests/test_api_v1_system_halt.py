"""Script used to test the /api/v1/system/halt endpoint."""
import e2e_test_framework


class APIE2ETestSystemHalt(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/halt endpoint."""
    uri = "/api/v1/system/halt"
    post_tests = [
        {
            "name": "Test system halt",
            "resp_data_empty": True,
            "req_data": {"debug": True}}

    ]


APIE2ETestSystemHalt()
