import unit_test_framework

class APIUnitTestInterface(unit_test_framework.APIUnitTest):
    url = "/api/v1/interface"
    get_payloads = [{}]
    post_payloads = [
        {
            "if": "em1",
            "descr": "asdf",
            "enable": "",
            "type": "staticv4",
            "ipaddr": "10.250.0.1",
            "subnet": "24",
            "blockbogons": True
        }
    ]
    delete_payloads = [
        {
            "if": "em1"
        }
    ]

APIUnitTestInterface()