import unit_test_framework

class APIUnitTestSystemReboot(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/reboot"
    post_tests = [
        {"payload": {"debug": True}}
    ]

APIUnitTestSystemReboot()