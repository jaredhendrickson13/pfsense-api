import unit_test_framework


class APIUnitTestSystemHostname(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/hostname"
    get_tests = [{"name": "Read the system hostname"}]
    put_tests = [
        {
            "name": "Update system hostname",
            "payload": {"hostname": "pfsense-api", "domain": "test.com"},
            "resp_time": 10    # Allow a few seconds for DNS services to reload
        },
        {
            "name": "Test hostname validation",
            "status": 400,
            "return": 1000,
            "payload": {"hostname": "!@#$", "domain": "test.com"}
        },
        {
            "name": "Test domain validation",
            "status": 400,
            "return": 1001,
            "payload": {"hostname": "pfsense-api", "domain": "!@#$"}
        }
    ]


APIUnitTestSystemHostname()
