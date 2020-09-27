import unit_test_framework

class APIUnitTestFirewallRule(unit_test_framework.APIUnitTest):
    url = "/api/v1/firewall/rule"
    get_payloads = [{}]
    post_payloads = [
        {
            "type": "block",
            "interface": "wan",
            "ipprotocol": "inet",
            "protocol": "tcp/udp",
            "src": "172.16.77.121",
            "srcport": "any",
            "dst": "127.0.0.1",
            "dstport": "443",
            "descr": "Unit test",
            "top": True
        }
    ]
    delete_payloads = [{"id": 0}]

APIUnitTestFirewallRule()