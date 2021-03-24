import unit_test_framework


class APIUnitTestSystemTable(unit_test_framework.APIUnitTest):
    uri = "/api/v1/system/table"
    get_tests = [{"name": "Read system tables"}]


APIUnitTestSystemTable()
