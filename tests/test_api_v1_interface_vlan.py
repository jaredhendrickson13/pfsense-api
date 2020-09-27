import unit_test_framework

class APIUnitTestInterfaceVLAN(unit_test_framework.APIUnitTest):
    url = "/api/v1/interface/vlan"
    get_payloads = [{}]
    post_payloads = [
        {
            "if": "lo0",
            "tag": 3,
            "pcp": 7,
            "descr": "Unit Test"
        }
    ]
    put_payloads = [
        {
            "vlanif": "lo0.3",
            "if": "lo0",
            "tag": 5,
            "pcp": 2,
            "descr": "Updated Unit Test"
        }
    ]
    delete_payloads = [
        {
            "vlanif": "lo0.5"
        }
    ]

APIUnitTestInterfaceVLAN()