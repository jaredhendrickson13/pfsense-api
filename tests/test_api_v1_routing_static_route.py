import unit_test_framework

class APIUnitTestRoutingStaticRoute(unit_test_framework.APIUnitTest):
    url = "/api/v1/routing/static_route"
    get_payloads = [{}]
    post_payloads = [
        {"network": "172.16.172.0/29", "gateway": "WAN_DHCP", "disabled": True, "descr": "Unit Test"}
    ]
    put_payloads = [
        {"id": 0, "network": "172.16.173.0/29", "gateway": "WAN_DHCP", "disabled": True, "descr": "Updated Unit Test"}
    ]
    delete_payloads = [
        {"id": 0}
    ]

APIUnitTestRoutingStaticRoute()