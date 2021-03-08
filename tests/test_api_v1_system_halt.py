import unit_test_framework

class APIUnitTestSystemHalt(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/halt"
    post_tests = [
        {"payload": {"debug": True}}
    ]

APIUnitTestSystemHalt()