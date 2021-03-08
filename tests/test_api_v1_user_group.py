import unit_test_framework

class APIUnitTestUserGroup(unit_test_framework.APIUnitTest):
    url = "/api/v1/user/group"
    post_tests = [
        {
            "payload": {
                "username": "admin",
                "group": ["admins"]
            }
        }
    ]
    delete_tests = [
        {
            "payload": {
                "username": "admin",
                "group": ["admins"]
            }
        }
    ]

APIUnitTestUserGroup()