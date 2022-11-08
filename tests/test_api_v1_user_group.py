"""Script used to test the /api/v1/user/group endpoint."""
import e2e_test_framework


class APIE2ETestUserGroup(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/user/group endpoint."""
    uri = "/api/v1/user/group"

    post_privileges = ["page-all", "page-system-groupmanager"]
    put_privileges = ["page-all", "page-system-groupmanager"]
    delete_privileges = ["page-all", "page-system-groupmanager"]
    get_privileges = ["page-all", "page-system-groupmanager"]

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
            "req_data": {
                "name": ""
            }
        },
        {
            "name": "Check local group name maximum length constraint",
            "status": 400,
            "return": 5044,
            "req_data": {
                "name": "THISNAMEISTOOLONGFORAGROUP",
                "scope": "local"
            }
        },
        {
            "name": "Check remote group name maximum length constraint",
            "status": 400,
            "return": 5044,
            "req_data": {
                "name": "SDPUMWFf76c1NuDFmCgt28htYERm8SUOZJldH2VPzZXj446X9T0ijTXC935KcSax2"
            }
        },
        {
            "name": "Check name exists constraint",
            "status": 400,
            "return": 5049,
            "req_data": {
                "name": "admins"
            }
        },
        {
            "name": "Check name charset constraint",
            "status": 400,
            "return": 5045,
            "req_data": {
                "name": "!!!INVALID@@@"
            }
        },
        {
            "name": "Check scope options constraint",
            "status": 400,
            "return": 5046,
            "req_data": {
                "name": "TEST_GROUP",
                "scope": "INVALID_CHOICE"
            }
        },
        {
            "name": "Check member exists constraint",
            "status": 400,
            "return": 5047,
            "req_data": {
                "name": "TEST_GROUP",
                "scope": "local",
                "member": ["NOT_A_VALID_UID"]
            }
        },
        {
            "name": "Check privilege exists constraint",
            "status": 400,
            "return": 5048,
            "req_data": {
                "name": "TEST_GROUP",
                "scope": "remote",
                "member": [],
                "priv": ["NOT_A_VALID_PRIVILEGE"]
            }
        },
        {
            "name": "Create group",
            "req_data": {
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
            "req_data": {
                "id": "THIS_GROUP_DOES_NOT_EXIST"
            }
        },
        {
            "name": "Check name minimum length constraint",
            "status": 400,
            "return": 5044,
            "req_data": {
                "id": "TEST_GROUP",
                "name": ""
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 5044,
            "req_data": {
                "id": "TEST_GROUP",
                "name": "THISNAMEISTOOLONGFORAGROUP"
            }
        },
        {
            "name": "Check name maximum length constraint",
            "status": 400,
            "return": 5044,
            "req_data": {
                "id": "TEST_GROUP",
                "name": "THISNAMEISTOOLONGFORAGROUP"
            }
        },
        {
            "name": "Check name exists constraint",
            "status": 400,
            "return": 5049,
            "req_data": {
                "id": "TEST_GROUP",
                "name": "admins"
            }
        },
        {
            "name": "Check name charset constraint",
            "status": 400,
            "return": 5045,
            "req_data": {
                "id": "TEST_GROUP",
                "name": "!!!INVALID@@@"
            }
        },
        {
            "name": "Check scope options constraint",
            "status": 400,
            "return": 5046,
            "req_data": {
                "id": "TEST_GROUP",
                "name": "TEST_GROUP",
                "scope": "INVALID_CHOICE"
            }
        },
        {
            "name": "Check member exists constraint",
            "status": 400,
            "return": 5047,
            "req_data": {
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
            "req_data": {
                "id": "TEST_GROUP",
                "name": "TEST_GROUP",
                "scope": "remote",
                "member": [],
                "priv": ["NOT_A_VALID_PRIVILEGE"]
            }
        },
        {
            "name": "Update group",
            "req_data": {
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
            "req_data": {
                "id": "THIS_GROUP_DOES_NOT_EXIST"
            }
        },
        {
            "name": "Delete group",
            "req_data": {
                "id": "TEST_GROUP"
            }
        }
    ]


APIE2ETestUserGroup()
