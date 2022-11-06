"""Script used to test the /api/v1/system/dns endpoint."""
import e2e_test_framework

# Constants
DNSSERVER_TEST_1 = ["1.1.1.1", "1.2.3.4", "4.3.2.1"]
DNSLOCALHOST_TEST_1 = False
DNSSERVER_TEST_2 = ["8.8.8.8", "8.8.4.4", "1.1.1.1"]
DNSLOCALHOST_TEST_2 = True


class APIE2ETestSystemDNS(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/dns endpoint."""
    uri = "/api/v1/system/dns"
    get_tests = [{"name": "Read system DNS servers"}]
    put_tests = [
        {
            "name": "Update system DNS servers and ensure resolv.conf is set properly",
            "resp_time": 10,  # Allow a few seconds for DNS services to be reloaded
            "post_test_callable": "is_dns_set_test_1",
            "req_data": {
                "dnsserver": DNSSERVER_TEST_1,
                "dnslocalhost": DNSLOCALHOST_TEST_1,
                "dnsallowoverride": False
            },
        },
        {
            "name": "Update system DNS servers again and ensure resolv.conf is set properly",
            "resp_time": 10,  # Allow a few seconds for DNS services to be reloaded
            "post_test_callable": "is_dns_set_test_2",
            "req_data": {
                "dnsserver": DNSSERVER_TEST_2,
                "dnslocalhost": DNSLOCALHOST_TEST_2,
                "dnsallowoverride": False
            },
        },
    ]

    def is_dns_set_test_1(self):
        """Checks if the first DNS update is applied correctly in resolv.conf"""
        # Local variables
        resolvconf_out = self.pfsense_shell("cat /etc/resolv.conf")

        # Ensure localhost is present as nameserver if dnslocalhost is enabled
        if DNSLOCALHOST_TEST_1 and "nameserver 127.0.0.1" not in resolvconf_out:
            raise AssertionError(f"Expected nameserver 127.0.0.1 in /etc/resolv.conf, got: {resolvconf_out}")
        # Ensure localhost is not present if dnslocalhost is not enabled
        if not DNSLOCALHOST_TEST_1 and "nameserver 127.0.0.1" in resolvconf_out:
            raise AssertionError(f"Unexpected nameserver 127.0.0.1 in /etc/resolv.conf, got: {resolvconf_out}")

        # Loop through each requested nameserver and ensure it is present in resolv.conf
        for ns_ip in DNSSERVER_TEST_1:
            if f"nameserver {ns_ip}" not in resolvconf_out:
                raise AssertionError(f"Expected nameserver {ns_ip} in /etc/resolv.conf, got: {resolvconf_out}")

    def is_dns_set_test_2(self):
        """Checks if the second DNS update is applied correctly in resolv.conf"""
        # Local variables
        resolvconf_out = self.pfsense_shell("cat /etc/resolv.conf")

        # Ensure localhost is present as nameserver if dnslocalhost is enabled
        if DNSLOCALHOST_TEST_2 and "nameserver 127.0.0.1" not in resolvconf_out:
            raise AssertionError(f"Expected nameserver 127.0.0.1 in /etc/resolv.conf, got: {resolvconf_out}")
        # Ensure localhost is not present if dnslocalhost is not enabled
        if not DNSLOCALHOST_TEST_2 and "nameserver 127.0.0.1" in resolvconf_out:
            raise AssertionError(f"Unexpected nameserver 127.0.0.1 in /etc/resolv.conf, got: {resolvconf_out}")

        # Loop through each requested nameserver and ensure it is present in resolv.conf
        for ns_ip in DNSSERVER_TEST_2:
            if f"nameserver {ns_ip}" not in resolvconf_out:
                raise AssertionError(f"Expected nameserver {ns_ip} in /etc/resolv.conf, but got: {resolvconf_out}")


APIE2ETestSystemDNS()
