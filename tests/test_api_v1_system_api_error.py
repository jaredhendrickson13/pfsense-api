import unit_test_framework


class APIUnitTestSystemAPIError(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/api/error"
    get_tests = [{"name": "Read API errors"}]


APIUnitTestSystemAPIError()
