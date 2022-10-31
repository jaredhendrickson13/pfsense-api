"""Script used to test the /api/v1/user/group/member endpoint."""
import e2e_test_framework


class APIE2ETestUserGroupMember(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/user/group/member endpoint."""
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
            "req_data": {
                "username": "new_user"
            }
        },
        {
            "name": "Check group requirement",
            "status": 400,
            "return": 5007,
            "req_data": {
                "username": "admin"
            }
        },
        {
            "name": "Check group exists constraint",
            "status": 400,
            "return": 5008,
            "req_data": {
                "username": "admin",
                "group": "INVALID"
            }
        },
        {
            "name": "Create group for use in tests",
            "uri": "/api/v1/user/group",
            "req_data": {
                "name": "TEST_GROUP",
                "scope": "local",
                "member": [],
                "priv": ["page-all"]
            }
        },
        {
            "name": "Add user to admins group",
            "req_data": {
                "username": "admin",
                "group": ["TEST_GROUP"]
            }
        },
        {
            "name": "Check ability to add single group as string",
            "req_data": {
                "username": "admin",
                "group": "TEST_GROUP"
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
            "req_data": {
                "username": "new_user"
            }
        },
        {
            "name": "Check group requirement",
            "status": 400,
            "return": 5007,
            "req_data": {
                "username": "admin"
            }
        },
        {
            "name": "Check group exists constraint",
            "status": 400,
            "return": 5008,
            "req_data": {
                "username": "admin",
                "group": "INVALID"
            }
        },
        {
            "name": "Remove user from admins group",
            "req_data": {
                "username": "admin",
                "group": ["TEST_GROUP"]
            }
        },
        {
            "name": "Re-add user to test group",
            "method": "POST",
            "req_data": {
                "username": "admin",
                "group": ["TEST_GROUP"]
            }
        },
        {
            "name": "Check ability to remove single group as string",
            "req_data": {
                "username": "admin",
                "group": "TEST_GROUP"
            }
        },
        {
            "name": "Delete test group",
            "uri": "/api/v1/user/group",
            "req_data": {
                "id": "TEST_GROUP"
            }
        }
    ]


APIE2ETestUserGroupMember()
