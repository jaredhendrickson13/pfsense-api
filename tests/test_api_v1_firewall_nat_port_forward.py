import unit_test_framework

class APIUnitTestFirewallNATPortForward(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/nat/port_forward"
    get_payloads = [{}]
    post_payloads = [
        {
            "interface": "WAN",
            "protocol": "tcp",
            "src": "any",
            "srcport": "433",
            "dst": "wanip",
            "dstport": "443",
            "target": "192.168.1.123",
            "local-port": "443",
            "natreflection": "purenat",
            "descr": "Unit Test",
            "nosync": True,
            "top": True
        }
    ]
    delete_payloads = [
        {"id": 0}
    ]

APIUnitTestFirewallNATPortForward()