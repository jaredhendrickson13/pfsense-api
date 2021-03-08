import unit_test_framework

class APIUnitTestSystemARP(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/arp"
    get_tests = [{}]
    delete_tests = [
        {"payload": {"ip": "127.0.0.1"}}
    ]

APIUnitTestSystemARP()