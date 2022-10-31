"""Script used to test the /api/v1/system/dns endpoint."""
import e2e_test_framework


class APIE2ETestSystemDNS(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/dns endpoint."""
    uri = "/api/v1/system/dns"
    get_tests = [{"name": "Read system DNS servers"}]
    put_tests = [
        {
            "name": "Update system DNS servers",
            "req_data": {
                "dnsserver": ["8.8.4.4", "1.1.1.1", "8.8.8.8"],
                "dnslocalhost": False,
                "dnsallowoverride": False
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        },
    ]


APIE2ETestSystemDNS()
