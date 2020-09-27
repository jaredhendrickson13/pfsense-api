import unit_test_framework

class APIUnitTestStatusLogFirewall(unit_test_framework.APIUnitTest):
    url = "/api/v1/status/log/firewall"
    get_payloads = [{}]

APIUnitTestStatusLogFirewall()