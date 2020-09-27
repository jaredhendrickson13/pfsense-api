import unit_test_framework

class APIUnitTestStatusLogDHCP(unit_test_framework.APIUnitTest):
    url = "/api/v1/status/log/dhcp"
    get_payloads = [{}]

APIUnitTestStatusLogDHCP()