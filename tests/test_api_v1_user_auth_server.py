import unit_test_framework


class APIUnitTestUserAuthServer(unit_test_framework.APIUnitTest):
    url = "/api/v1/user/auth_server"
    get_tests = [{"name": "Read all auth servers"}]


APIUnitTestUserAuthServer()
