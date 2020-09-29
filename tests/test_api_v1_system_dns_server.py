import unit_test_framework

class APIUnitTestSystemDNSServer(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/dns/server"
    post_payloads = [
        {
            "dnsserver": "1.1.1.1"
        }
    ]
    delete_payloads = [
        {
            "dnsserver": "1.1.1.1"
        }
    ]


APIUnitTestSystemDNSServer()