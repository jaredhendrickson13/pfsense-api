import unit_test_framework


class APIUnitTestSystemDNS(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/dns"
    get_tests = [{"name": "Read system DNS servers"}]
    put_tests = [
        {
            "name": "Update system DNS servers",
            "payload": {
                "dnsserver": ["8.8.4.4", "1.1.1.1", "8.8.8.8"],
                "dnslocalhost": False,
                "dnsallowoverride": False
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        },
    ]


APIUnitTestSystemDNS()