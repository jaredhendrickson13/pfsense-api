import e2e_test_framework


class APIE2ETestUserGroupMember(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/user/group/member"
    post_tests = [
        {
            "name": "Check username requirement",
            "status": 400,
            "return": 5000,
        },
        {
            "name": "Check username exists constraint",
            "status": 400,
            "return": 5001,
            "payload": {
                "username": "new_user"
            }
        },
        {
            "name": "Check group requirement",
            "status": 400,
            "return": 5007,
            "payload": {
                "username": "admin"
            }
        },
        {
            "name": "Check group exists constraint",
            "status": 400,
            "return": 5008,
            "payload": {
                "username": "admin",
                "group": "INVALID"
            }
        },
        {
            "name": "Add user to admins group",
            "payload": {
                "username": "admin",
                "group": ["admins"]
            }
        },
        {
            "name": "Check ability to add single group as string",
            "payload": {
                "username": "admin",
                "group": "admins"
            }
        }
    ]
    delete_tests = [
        {
            "name": "Check username requirement",
            "status": 400,
            "return": 5000,
        },
        {
            "name": "Check username exists constraint",
            "status": 400,
            "return": 5001,
            "payload": {
                "username": "new_user"
            }
        },
        {
            "name": "Check group requirement",
            "status": 400,
            "return": 5007,
            "payload": {
                "username": "admin"
            }
        },
        {
            "name": "Check group exists constraint",
            "status": 400,
            "return": 5008,
            "payload": {
                "username": "admin",
                "group": "INVALID"
            }
        },
        {
            "name": "Remove user from admins group",
            "payload": {
                "username": "admin",
                "group": ["admins"]
            }
        },
        {
            "name": "Check ability to remove single group as string",
            "payload": {
                "username": "admin",
                "group": "admins"
            }
        }
    ]


APIE2ETestUserGroupMember()
