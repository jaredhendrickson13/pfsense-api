import unit_test_framework


class APIUnitTestSystemReboot(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/reboot"
    post_tests = [
        {
            "name": "Test system reboot",
            "payload": {"debug": True}
        }
    ]


APIUnitTestSystemReboot()
