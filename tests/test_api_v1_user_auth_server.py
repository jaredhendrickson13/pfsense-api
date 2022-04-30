import e2e_test_framework


class APIE2ETestUserAuthServer(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/user/auth_server"
    get_tests = [{"name": "Read all auth servers"}]


APIE2ETestUserAuthServer()
