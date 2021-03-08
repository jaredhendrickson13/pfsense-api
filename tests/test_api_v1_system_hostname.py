import unit_test_framework

class APIUnitTestSystemHostname(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/hostname"
    get_tests = [{}]
    put_tests = [
        {
            "payload": {"hostname": "pfsense-api", "domain": "test.com"},
            "resp_time": 10    # Allow a few seconds for DNS services to reload
        }
    ]

APIUnitTestSystemHostname()