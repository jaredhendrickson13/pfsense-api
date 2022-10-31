"""Script used to test the /api/v1/system/console endpoint."""
import e2e_test_framework


class APIE2ETestSystemConsole(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/console endpoint."""
    uri = "/api/v1/system/console"
    put_tests = [
        {
            "name": "Enable password protected console",
            "req_data": {"disableconsolemenu": True}
        },
        {
            "name": "Disable password protected console",
            "req_data": {"disableconsolemenu": False}
        }
    ]


APIE2ETestSystemConsole()
