import unit_test_framework

class APIUnitTestUserAuthServerRadius(unit_test_framework.APIUnitTest):
    url = "/api/v1/user/auth_server/radius"
    get_payloads = [{}]

APIUnitTestUserAuthServerRadius()