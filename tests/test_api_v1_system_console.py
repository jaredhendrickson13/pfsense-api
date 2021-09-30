import unit_test_framework


class APIUnitTestSystemConsole(unit_test_framework.APIUnitTest):
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

APIUnitTestSystemConsole()
