import unit_test_framework

class APIUnitTestSystemDNS(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/dns"
    get_payloads = [{}]
    put_payloads = [
        {
            "dnsserver": ["8.8.4.4", "1.1.1.1", "8.8.8.8"],
            "dnslocalhost": False,
            "dnsallowoverride": False
        },
        {
            "dnsserver": ["8.8.8.8", "8.8.4.4"],
            "dnslocalhost": True,
            "dnsallowoverride": True
        }
    ]

APIUnitTestSystemDNS()