import e2e_test_framework


class APIE2ETestSystemAPIError(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/api/error"
    get_tests = [{"name": "Read API errors"}]


APIE2ETestSystemAPIError()
