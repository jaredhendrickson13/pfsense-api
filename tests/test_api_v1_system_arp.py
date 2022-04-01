import e2e_test_framework


class APIE2ETestSystemARP(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/arp"
    get_tests = [{"name": "Read the ARP table"}]
    delete_tests = [
        {
            "name": "Delete ARP table entry",
            "payload": {"ip": "127.0.0.1"}
        }
    ]


APIE2ETestSystemARP()
