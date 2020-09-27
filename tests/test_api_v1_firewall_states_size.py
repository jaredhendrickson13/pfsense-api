import unit_test_framework

class APIUnitTestFirewallStatesSize(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/states/size"
    get_payloads = [{}]
    put_payloads = [
        {"maximumstates": 20000},
        {"maximumstates": "default"}
    ]

APIUnitTestFirewallStatesSize()