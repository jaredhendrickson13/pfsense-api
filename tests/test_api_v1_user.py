import unit_test_framework

class APIUnitTestUser(unit_test_framework.APIUnitTest):
    url = "/api/v1/user"
    get_payloads = [{}]
    post_payloads = [
        {
            "disabled": True,
            "username": "new_user",
            "password": "changeme",
            "descr": "NEW USER",
            "authorizedkeys": "test auth key",
            "ipsecpsk": "test psk",
            "expires": "11/22/2050"
        }
    ]
    put_payloads = [
        {
            "disabled": False,
            "username": "new_user",
            "password": "changeme_again",
            "descr": "UPDATED NEW USER",
            "authorizedkeys": "updated test auth key",
            "ipsecpsk": "updated test psk",
            "expires": "11/22/2051"
        }
    ]
    delete_payloads = [{"username": "new_user"}]

APIUnitTestUser()