import e2e_test_framework


class APIE2ETestSystemReboot(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/reboot"
    post_tests = [
        {
            "name": "Test system reboot",
            "payload": {"debug": True}
        }
    ]


APIE2ETestSystemReboot()
