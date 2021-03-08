import unit_test_framework


class APIUnitTestUserGroup(unit_test_framework.APIUnitTest):
    url = "/api/v1/user/group"
    post_tests = [
        {
            "name": "Add user to admins group",
            "payload": {
                "username": "admin",
                "group": ["admins"]
            }
        }
    ]
    delete_tests = [
        {
            "name": "Remove user from admins group",
            "payload": {
                "username": "admin",
                "group": ["admins"]
            }
        }
    ]


APIUnitTestUserGroup()