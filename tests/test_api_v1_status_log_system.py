import unit_test_framework

class APIUnitTestStatusLogSystem(unit_test_framework.APIUnitTest):
    url = "/api/v1/status/log/system"
    get_payloads = [{}]

APIUnitTestStatusLogSystem()