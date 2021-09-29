import unit_test_framework


class APIUnitTestSystemAPIVersion(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/api/version"
    get_tests = [{"name": "Read API version"}]


APIUnitTestSystemAPIVersion()
