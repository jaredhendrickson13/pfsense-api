import e2e_test_framework


class APIE2ETestSystemHalt(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/halt"
    post_tests = [
        {
            "name": "Test system halt",
            "payload": {"debug": True}}

    ]


APIE2ETestSystemHalt()
