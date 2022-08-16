"""Script used to test the /api/v1/user/privilege endpoint."""
import e2e_test_framework


class APIE2ETestUserPrivilege(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/user/privilege endpoint."""
    uri = "/api/v1/user/privilege"
    post_tests = [
        {
            "name": "Grant user privileges",
            "payload": {
                "username": "admin",
                "priv": ["page-all", "page-system-api"]
            }
        },
        {
            "name": "Check username requirement",
            "status": 400,
            "return": 5000,
        },
        {
            "name": "Check non-existing username",
            "status": 400,
            "return": 5001,
            "payload": {
                "username": "INVALID"
            }
        },
        {
            "name": "Check non-existing privileges",
            "status": 400,
            "return": 5006,
            "payload": {
                "username": "admin",
                "priv": ["INVALID"]
            }
        }
    ]
    delete_tests = [
        {
            "name": "Delete user privileges",
            "payload": {
                "username": "admin",
                "priv": ["page-system-api"]
            }
        }
    ]


APIE2ETestUserPrivilege()
