import unit_test_framework

class APIUnitTestFirewallStates(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/states"
    get_payloads = [{}]

APIUnitTestFirewallStates()