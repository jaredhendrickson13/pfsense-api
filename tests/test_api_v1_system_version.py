import unit_test_framework


class APIUnitTestSystemVersion(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/version"
    get_tests = [{"name": "Read pfSense version"}]


APIUnitTestSystemVersion()
