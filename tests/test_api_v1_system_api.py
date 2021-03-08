import unit_test_framework

class APIUnitTestSystemAPI(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/api"
    get_tests = [{}]
    put_tests = [
        {
            "payload": {
                "persist": False,
                "jwt_exp": 86400,
                "hashalgo": "sha512",
                "keybytes": 64,
                "allowed_interfaces": ["WAN"]
            },
        },
        {
            "payload": {
                "persist": True,
                "jwt_exp": 300,
                "hashalgo": "sha256",
                "keybytes": 16,
                "allowed_interfaces": ["any"]
            },
        }
    ]

APIUnitTestSystemAPI()