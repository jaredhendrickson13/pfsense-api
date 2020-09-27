import unit_test_framework

class APIUnitTestRoutingGateway(unit_test_framework.APIUnitTest):
    url = "/api/v1/routing/gateway"
    get_payloads = [{}]

APIUnitTestRoutingGateway()