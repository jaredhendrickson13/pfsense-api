import unit_test_framework


class APIUnitTestUser(unit_test_framework.APIUnitTest):
    uri = "/api/v1/user"
    get_tests = [{"name": "Read local users"}]
    post_tests = [
        {
            "name": "Create RSA internal CA",
            "uri": "/api/v1/system/ca",
            "payload": {
                "method": "internal",
                "descr": "INTERNAL_CA_RSA",
                "trust": True,
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "internal-ca-unit-test.example.com"
            },
        },
        {
            "name": "Create user certificate with RSA key",
            "uri": "/api/v1/system/certificate",
            "caref": True,    # Locator for tests that need a caref dynamically added in post_post()
            "payload": {
                "method": "internal",
                "descr": "USER_CERT",
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "new_user",
                "type": "user"
            }
        },
        {
            "name": "Create server certificate with RSA key",
            "uri": "/api/v1/system/certificate",
            "caref": True,    # Locator for tests that need a caref dynamically added in post_post()
            "payload": {
                "method": "internal",
                "descr": "SERVER_CERT",
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "internal-cert-unit-test.example.com",
                "type": "server"
            }
        },
        {
            "name": "Create local user",
            "resp_time": 2,     # Allow a couple seconds for user database to be updated
            "user_cert": True,       # Locator for tests that need a user cert ref ID dynamically added by post_post()
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
        {
            "name": "Check user certificate exists constraint",
            "status": 400,
            "return": 5041,
            "payload": {
                "username": "another_user",
                "password": "changeme",
                "cert": "INVALID"
            }
        }
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
                "expires": "11/22/2051",
                "cert": []
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
        {
            "name": "Check user certificate exists constraint",
            "status": 400,
            "return": 5041,
            "payload": {
                "username": "new_user",
                "password": "changeme",
                "cert": "INVALID"
            }
        },
        {
            "name": "Check inability to add server certificate as a user certificate",
            "status": 400,
            "return": 5041,
            "server_cert": True,
            "payload": {
                "username": "new_user",
                "password": "changeme"
            }
        },
        {
            "name": "Update local user to re-add user certificate",
            "user_cert": True,
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
    ]
    delete_tests = [
        {
            "name": "Check inability to delete user certificate while in use",
            "uri": "/api/v1/system/certificate",
            "status": 400,
            "return": 1005,
            "payload": {"descr": "USER_CERT"}
        },
        {
            "name": "Delete server certificate used for testing",
            "uri": "/api/v1/system/certificate",
            "payload": {"descr": "SERVER_CERT"}
        },
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
        },
        {
            "name": "Check ability to delete user certificate after user was deleted",
            "uri": "/api/v1/system/certificate",
            "payload": {"descr": "USER_CERT"}
        },
        {
            "name": "Delete CA used for testing",
            "uri": "/api/v1/system/ca",
            "payload": {"descr": "INTERNAL_CA_RSA"}
        }
    ]

    def post_post(self):
            # Check our first POST response for the created CA's refid
            if len(self.post_responses) == 1:
                # Variables
                counter = 0

                # Loop through all tests and auto-add the caref ID to tests that have the caref key set
                for test in self.post_tests:
                    if "payload" in test.keys() and "caref" in test.keys():
                        self.post_tests[counter]["payload"]["caref"] = self.post_responses[0]["data"]["refid"]
                    counter = counter + 1

            # Check the second and third POST responses for the user and server certificates
            if len(self.post_responses) == 3:
                # Variables
                post_counter = 0
                put_counter = 0

                # Loop through all tests and auto-add the refid to payloads that have the user_cert set
                for test in self.post_tests:
                    if "payload" in test.keys() and "user_cert" in test.keys():
                        self.post_tests[post_counter]["payload"]["cert"] = [self.post_responses[1]["data"]["refid"]]


                # Do the same for PUT tests
                for test in self.put_tests:
                    if "payload" in test.keys() and "user_cert" in test.keys():
                        self.put_tests[put_counter]["payload"]["cert"] = [self.post_responses[1]["data"]["refid"]]

APIUnitTestUser()
