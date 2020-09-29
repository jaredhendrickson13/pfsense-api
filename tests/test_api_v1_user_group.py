import unit_test_framework

class APIUnitTestUserGroup(unit_test_framework.APIUnitTest):
    url = "/api/v1/user/group"
    post_payloads = [
        {
            "username": "admin",
            "group": ["admins"]
        }
    ]
    delete_payloads = [
        {
            "username": "admin",
            "group": ["admins"]
        }
    ]

APIUnitTestUserGroup()