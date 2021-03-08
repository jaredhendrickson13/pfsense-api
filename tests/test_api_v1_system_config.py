import unit_test_framework

class APIUnitTestSystemConfig(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/config"
    get_tests = [{}]

APIUnitTestSystemConfig()