import unit_test_framework


class APIUnitTestSystemDNSServer(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/dns/server"
    post_tests = [
        {
            "name": "Add a system DNS server",
            "payload": {
                "dnsserver": "1.1.1.1"
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        },
        {
            "name": "Test DNS server IP validation",
            "status": 400,
            "return": 1007,
            "payload": {
                "dnsserver": "INVALID"
            },
        }
    ]
    delete_tests = [
        {
            "name": "Delete a system DNS server",
            "payload": {
                "dnsserver": "1.1.1.1"
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        }
    ]


APIUnitTestSystemDNSServer()
