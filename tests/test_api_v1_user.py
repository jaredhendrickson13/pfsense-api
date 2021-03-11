import unit_test_framework


class APIUnitTestUser(unit_test_framework.APIUnitTest):
    url = "/api/v1/user"
    get_tests = [{"name": "Read local users"}]
    post_tests = [
        {
            "name": "Create local user",
            "resp_time": 2,  # Allow a couple seconds for user database to be updated
            "payload": {
                "disabled": False,
                "username": "new_user",
                "password": "changeme",
                "descr": "NEW USER",
                "authorizedkeys": "test auth key",
                "ipsecpsk": "test psk",
                "expires": "11/22/2050",
                "priv": ["page-system-usermanager"]
            },
        },
        {
            "name": "Create a disabled local user using the previously created local user",
            "resp_time": 2,  # Allow a couple seconds for user database to be updated
            "auth_payload": {"client-id": "new_user", "client-token": "changeme"},
            "payload": {
                "disabled": True,
                "username": "disabled_user",
                "password": "changeme",
            },
        },
        {
            "name": "Check disabled user's inability to authenticate",
            "status": 401,
            "return": 3,
            "auth_payload": {"client-id": "disabled_user", "client-token": "changeme"},
        },
        {
            "name": "Check username requirement and login using created user",
            "status": 400,
            "return": 5000,
        },
        {
            "name": "Check username unique constraint",
            "status": 400,
            "return": 5002,
            "payload": {
                "username": "new_user"
            }
        },
        {
            "name": "Check username character constraint",
            "status": 400,
            "return": 5036,
            "payload": {
                "username": "!@#"
            }
        },
        {
            "name": "Check reserved username constraint",
            "status": 400,
            "return": 5037,
            "payload": {
                "username": "root"
            }
        },
        {
            "name": "Check username length constraint",
            "status": 400,
            "return": 5038,
            "payload": {
                "username": "THIS_USERNAME_IS_TOO_LONG_FOR_PFSENSE_TO_HANDLE"
            }
        },
        {
            "name": "Check password requirement",
            "status": 400,
            "return": 5003,
            "payload": {
                "username": "another_user"
            }
        },
        {
            "name": "Check privilege validation",
            "status": 400,
            "return": 5006,
            "payload": {
                "username": "another_user",
                "password": "changeme",
                "priv": ["INVALID"]
            }
        },
        {
            "name": "Check user expiration date validation",
            "status": 400,
            "return": 5040,
            "payload": {
                "username": "another_user",
                "password": "changeme",
                "expires": "INVALID"
            }
        },

    ]
    put_tests = [
        {
            "name": "Update local user",
            "payload": {
                "disabled": False,
                "username": "new_user",
                "password": "changeme_again",
                "descr": "UPDATED NEW USER",
                "authorizedkeys": "updated test auth key",
                "ipsecpsk": "updated test psk",
                "expires": "11/22/2051"
            },
            "resp_time": 2    # Allow a couple seconds for user database to be updated
        },
        {
            "name": "Check privilege validation",
            "status": 400,
            "return": 5006,
            "payload": {
                "username": "new_user",
                "password": "changeme",
                "priv": ["INVALID"]
            }
        },
        {
            "name": "Check user expiration date validation",
            "status": 400,
            "return": 5040,
            "payload": {
                "username": "new_user",
                "password": "changeme",
                "expires": "INVALID"
            }
        },

    ]
    delete_tests = [
        {
            "name": "Delete local user",
            "payload": {"username": "new_user"}
        },
        {
            "name": "Delete disabled user",
            "payload": {"username": "disabled_user"}
        },
        {
            "name": "Check deletion of non-existing user",
            "status": 400,
            "return": 5001,
            "payload": {"username": "INVALID"}
        },
        {
            "name": "Check deletion of system users",
            "status": 400,
            "return": 5005,
            "payload": {"username": "admin"}
        }
    ]


APIUnitTestUser()
