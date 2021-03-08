import unit_test_framework

class APIUnitTestSystemDNS(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/dns"
    get_tests = [{}]
    put_tests = [
        {
            "payload": {
                "dnsserver": ["8.8.4.4", "1.1.1.1", "8.8.8.8"],
                "dnslocalhost": False,
                "dnsallowoverride": False
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        },
        {
            "payload": {
                "dnsserver": ["8.8.8.8", "8.8.4.4"],
                "dnslocalhost": True,
                "dnsallowoverride": True
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        }
    ]

APIUnitTestSystemDNS()