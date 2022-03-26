import e2e_test_framework


class APIE2ETestUserGroupMember(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/user/group/member"
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


APIE2ETestUserGroupMember()
