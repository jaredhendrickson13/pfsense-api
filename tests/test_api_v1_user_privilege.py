import unit_test_framework


class APIUnitTestUserPrivilege(unit_test_framework.APIUnitTest):
    url = "/api/v1/user/privilege"
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


APIUnitTestUserPrivilege()
