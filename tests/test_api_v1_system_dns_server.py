"""Script used to test the /api/v1/system/dns/server endpoint."""
import e2e_test_framework


class APIE2ETestSystemDNSServer(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/dns/server endpoint."""
    uri = "/api/v1/system/dns/server"

    post_privileges = ["page-all", "page-system-generalsetup"]
    delete_privileges = ["page-all", "page-system-generalsetup"]

    post_tests = [
        {
            "name": "Add a system DNS server",
            "req_data": {
                "dnsserver": "1.1.1.1"
            },
            "resp_time": 15    # Allow a few seconds for DNS services to be reloaded
        },
        {
            "name": "Test DNS server IP validation",
            "status": 400,
            "return": 1007,
            "req_data": {
                "dnsserver": "INVALID"
            },
        }
    ]
    delete_tests = [
        {
            "name": "Delete a system DNS server",
            "req_data": {
                "dnsserver": "1.1.1.1"
            },
            "resp_time": 10    # Allow a few seconds for DNS services to be reloaded
        }
    ]


APIE2ETestSystemDNSServer()
