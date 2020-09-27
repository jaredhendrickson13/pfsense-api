import unit_test_framework

class APIUnitTestAccessToken(unit_test_framework.APIUnitTest):
    url = "/api/v1/access_token"
    post_payloads = [{}]

APIUnitTestAccessToken()