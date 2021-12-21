import e2e_test_framework


class APIE2ETestSystemConsole(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/console"
    put_tests = [
        {
            "name": "Enable password protected console",
            "payload": {"disableconsolemenu": True}
        },
        {
            "name": "Disable password protected console",
            "payload": {"disableconsolemenu": False}
        }
    ]

APIE2ETestSystemConsole()
