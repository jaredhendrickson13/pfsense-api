import unit_test_framework

class APIUnitTestFirewallVirtualIP(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/virtual_ip"
    get_payloads = [{}]
    post_payloads = [
        {
            "mode": "carp",
            "interface": "wan",
            "subnet": "172.16.77.239/32",
            "password": "testpass",
            "descr": "Unit Test"
        },
        {
            "mode": "proxyarp",
            "interface": "wan",
            "subnet": "172.16.77.240/32",
            "descr": "Unit Test"
        }
    ]
    put_payloads = [
        {
            "id": 0,
            "mode": "carp",
            "interface": "wan",
            "subnet": "172.16.77.229/32",
            "password": "newtestpass",
            "descr": "Updated unit Test"
        },
        {
            "id": 1,
            "mode": "proxyarp",
            "interface": "wan",
            "subnet": "172.16.77.230/32",
            "descr": "Updated unit Test"
        }
    ]
    delete_payloads = [
        {"id": 0},
        {"id": 0}
    ]

APIUnitTestFirewallVirtualIP()