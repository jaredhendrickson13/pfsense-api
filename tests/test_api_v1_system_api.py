import unit_test_framework

class APIUnitTestSystemAPI(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/api"
    get_payloads = [{}]

APIUnitTestSystemAPI()