"""Script used to test the /api/v1/user/auth_server/radius endpoint."""
import e2e_test_framework


class APIE2ETestUserAuthServerRadius(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/user/auth_server/radius endpoint."""
    uri = "/api/v1/user/auth_server/radius"

    get_privileges = ["page-all", "page-system-authservers"]
    post_privileges = ["page-all", "page-system-authservers"]
    delete_privileges = ["page-all", "page-system-authservers"]

    get_tests = [{"name": "Read RADIUS auth servers"}]
    post_tests = [
        {
            "name": "Create RADIUS auth server",
            "req_data": {
                "name": "e2e_test_RADIUS",
                "host": "123",
                "radius_secret": "testsecret",
                "radius_auth_port": 1812,
                "radius_acct_port": 1813,
                "radius_protocol": "MSCHAPv2",
                "radius_timeout": 5,
                "radius_nasip_attribute": "wan"
            }
        },
        {
            "name": "Test name requirement",
            "status": 400,
            "return": 5035
        },
        {
            "name": "Test name unique constraint",
            "status": 400,
            "return": 5026,
            "req_data": {"name": "e2e_test_RADIUS"}
        },
        {
            "name": "Test host requirement",
            "status": 400,
            "return": 5011,
            "req_data": {
                "name": "e2e_test_RADIUS_2"
            }
        },
        {
            "name": "Test host validation",
            "status": 400,
            "return": 5012,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "!@#$@#$$#@!"
            }
        },
        {
            "name": "Test RADIUS secret requirement",
            "status": 400,
            "return": 5028,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com"
            }
        },
        {
            "name": "Test RADIUS secret validation",
            "status": 400,
            "return": 5029,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": ["INVALID"]
            }
        },
        {
            "name": "Test RADIUS auth/acct port requirement",
            "status": 400,
            "return": 5032,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret"
            }
        },
        {
            "name": "Test RADIUS auth port minimum threshold",
            "status": 400,
            "return": 5030,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret",
                "radius_auth_port": 0
            }
        },
        {
            "name": "Test RADIUS auth port minimum threshold",
            "status": 400,
            "return": 5030,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret",
                "radius_auth_port": 65536
            }
        },
        {
            "name": "Test RADIUS acct port minimum threshold",
            "status": 400,
            "return": 5031,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret",
                "radius_acct_port": 0
            }
        },
        {
            "name": "Test RADIUS acct port minimum threshold",
            "status": 400,
            "return": 5031,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret",
                "radius_acct_port": 65536
            }
        },
        {
            "name": "Test RADIUS protocol validation",
            "status": 400,
            "return": 5027,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret",
                "radius_auth_port": 1812,
                "radius_protocol": "INVALID"
            }
        },
        {
            "name": "Test RADIUS timeout minimum threshold",
            "status": 400,
            "return": 5033,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret",
                "radius_auth_port": 1812,
                "radius_timeout": 0
            }
        },
        {
            "name": "Test RADIUS NAS IP attribute validation",
            "status": 400,
            "return": 5034,
            "req_data": {
                "name": "e2e_test_RADIUS_2",
                "host": "example.com",
                "radius_secret": "testsecret",
                "radius_auth_port": 1812,
                "radius_nasip_attribute": "INVALID"
            }
        },
    ]
    delete_tests = [
        {
            "name": "Delete LDAP auth server",
            "req_data": {"name": "e2e_test_RADIUS"}
        }
    ]


APIE2ETestUserAuthServerRadius()
