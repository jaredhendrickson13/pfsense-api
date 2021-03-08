import unit_test_framework

class APIUnitTestSystemDNSServer(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/dns/server"
    post_tests = [
        {
            "payload": {
                "dnsserver": "1.1.1.1"
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        }
    ]
    delete_tests = [
        {
            "payload": {
                "dnsserver": "1.1.1.1"
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        }
    ]


APIUnitTestSystemDNSServer()