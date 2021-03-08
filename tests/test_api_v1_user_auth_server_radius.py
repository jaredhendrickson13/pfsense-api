import unit_test_framework

class APIUnitTestUserAuthServerRadius(unit_test_framework.APIUnitTest):
    url = "/api/v1/user/auth_server/radius"
    get_tests = [{}]
    post_tests = [
        {
            "payload": {
                "name": "UNIT_TEST_RADIUS",
                "host": "123",
                "radius_secret": "testsecret",
                "radius_auth_port": 1812,
                "radius_acct_port": 1813,
                "radius_protocol": "MSCHAPv2",
                "radius_timeout": 5,
                "radius_nasip_attribute": "wan"
            }
        }
    ]
    delete_tests = [
        {"payload": {"name": "UNIT_TEST_RADIUS"}}
    ]

APIUnitTestUserAuthServerRadius()