import unit_test_framework

class APIUnitTestUser(unit_test_framework.APIUnitTest):
    url = "/api/v1/user"
    get_tests = [{"name": "Read local users"}]
    post_tests = [
        {
            "name": "Create local user",
            "payload": {
                "disabled": True,
                "username": "new_user",
                "password": "changeme",
                "descr": "NEW USER",
                "authorizedkeys": "test auth key",
                "ipsecpsk": "test psk",
                "expires": "11/22/2050"
            },
            "resp_time": 2    # Allow a couple seconds for user database to be updated
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
                "expires": "11/22/2051"
            },
            "resp_time": 2    # Allow a couple seconds for user database to be updated
        },

    ]
    delete_tests = [
        {
            "name": "Delete local user",
            "payload": {"username": "new_user"}
        }
    ]


APIUnitTestUser()
