import unit_test_framework

class APIUnitTestStatusCARP(unit_test_framework.APIUnitTest):
    url = "/api/v1/status/carp"
    get_payloads = [{}]
    put_payloads = [{"enable": True, "maintenance_mode": False, }]

APIUnitTestStatusCARP()