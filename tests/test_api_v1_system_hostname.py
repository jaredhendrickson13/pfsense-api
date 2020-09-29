import unit_test_framework

class APIUnitTestSystemHostname(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/hostname"
    get_payloads = [{}]
    put_payloads = [{"hostname": "pfsense-api", "domain": "test.com", }]

APIUnitTestSystemHostname()