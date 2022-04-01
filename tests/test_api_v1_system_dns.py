import e2e_test_framework


class APIE2ETestSystemDNS(e2e_test_framework.APIE2ETest):
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


APIE2ETestSystemDNS()
