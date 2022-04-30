import e2e_test_framework


class APIE2ETestUserGroup(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/user/group"
    get_tests = [
        {
            "name": "Read groups"
        }
    ]
    post_tests = [
        {
            "name": "Check name requirement",
            "status": 400,
            "return": 5042,
        },
        {
            "name": "Check name minimum length constraint",
            "status": 400,
            "return": 5044,
            "payload": {
                "name": ""
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 5044,
            "payload": {
                "name": "THISNAMEISTOOLONGFORAGROUP"
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 5044,
            "payload": {
                "name": "THISNAMEISTOOLONGFORAGROUP"
            }
        },
        {
            "name": "Check name exists constraint",
            "status": 400,
            "return": 5049,
            "payload": {
                "name": "admins"
            }
        },
        {
            "name": "Check name charset constraint",
            "status": 400,
            "return": 5045,
            "payload": {
                "name": "!!!INVALID@@@"
            }
        },
        {
            "name": "Check scope options constraint",
            "status": 400,
            "return": 5046,
            "payload": {
                "name": "TEST_GROUP",
                "scope": "INVALID_CHOICE"
            }
        },
        {
            "name": "Check member exists constraint",
            "status": 400,
            "return": 5047,
            "payload": {
                "name": "TEST_GROUP",
                "scope": "local",
                "member": ["NOT_A_VALID_UID"]
            }
        },
        {
            "name": "Check privilege exists constraint",
            "status": 400,
            "return": 5048,
            "payload": {
                "name": "TEST_GROUP",
                "scope": "remote",
                "member": [],
                "priv": ["NOT_A_VALID_PRIVILEGE"]
            }
        },
        {
            "name": "Create group",
            "payload": {
                "name": "TEST_GROUP",
                "scope": "local",
                "member": [0],
                "priv": ["page-all"]
            }
        }
    ]
    put_tests = [
        {
            "name": "Check ID requirement",
            "status": 400,
            "return": 5050,
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 5051,
            "payload": {
                "id": "THIS_GROUP_DOES_NOT_EXIST"
            }
        },
        {
            "name": "Check name minimum length constraint",
            "status": 400,
            "return": 5044,
            "payload": {
                "id": "TEST_GROUP",
                "name": ""
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 5044,
            "payload": {
                "id": "TEST_GROUP",
                "name": "THISNAMEISTOOLONGFORAGROUP"
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 5044,
            "payload": {
                "id": "TEST_GROUP",
                "name": "THISNAMEISTOOLONGFORAGROUP"
            }
        },
        {
            "name": "Check name exists constraint",
            "status": 400,
            "return": 5049,
            "payload": {
                "id": "TEST_GROUP",
                "name": "admins"
            }
        },
        {
            "name": "Check name charset constraint",
            "status": 400,
            "return": 5045,
            "payload": {
                "id": "TEST_GROUP",
                "name": "!!!INVALID@@@"
            }
        },
        {
            "name": "Check scope options constraint",
            "status": 400,
            "return": 5046,
            "payload": {
                "id": "TEST_GROUP",
                "name": "TEST_GROUP",
                "scope": "INVALID_CHOICE"
            }
        },
        {
            "name": "Check member exists constraint",
            "status": 400,
            "return": 5047,
            "payload": {
                "id": "TEST_GROUP",
                "name": "TEST_GROUP",
                "scope": "local",
                "member": ["NOT_A_VALID_UID"]
            }
        },
        {
            "name": "Check privilege exists constraint",
            "status": 400,
            "return": 5048,
            "payload": {
                "id": "TEST_GROUP",
                "name": "TEST_GROUP",
                "scope": "remote",
                "member": [],
                "priv": ["NOT_A_VALID_PRIVILEGE"]
            }
        },
        {
            "name": "Update group",
            "payload": {
                "id": "TEST_GROUP",
                "name": "TEST_GROUP",
                "scope": "remote",
                "member": [],
                "priv": []
            }
        }
    ]
    delete_tests = [
        {
            "name": "Check ID requirement",
            "status": 400,
            "return": 5050,
        },
        {
            "name": "Check ID exists constraint",
            "status": 400,
            "return": 5051,
            "payload": {
                "id": "THIS_GROUP_DOES_NOT_EXIST"
            }
        },
        {
            "name": "Delete group",
            "payload": {
                "id": "TEST_GROUP"
            }
        }
    ]


APIE2ETestUserGroup()
