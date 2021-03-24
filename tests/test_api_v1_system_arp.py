import unit_test_framework


class APIUnitTestSystemARP(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/arp"
    get_tests = [{"name": "Read the ARP table"}]
    delete_tests = [
        {
            "name": "Delete ARP table entry",
            "payload": {"ip": "127.0.0.1"}
        }
    ]


APIUnitTestSystemARP()
