import e2e_test_framework


class APIE2ETestSystemTable(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/table"
    get_tests = [{"name": "Read system tables"}]


APIE2ETestSystemTable()
