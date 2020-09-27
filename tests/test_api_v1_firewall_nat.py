import unit_test_framework

class APIUnitTestFirewallNAT(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/nat"
    get_payloads = [{}]

APIUnitTestFirewallNAT()