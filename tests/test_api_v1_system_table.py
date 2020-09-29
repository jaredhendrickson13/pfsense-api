import unit_test_framework

class APIUnitTestSystemTable(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/table"
    get_payloads = [{}]

APIUnitTestSystemTable()