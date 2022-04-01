import e2e_test_framework


class APIE2ETestSystemConfig(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/config"
    get_tests = [{"name": "Read the entire pfSense configuration"}]


APIE2ETestSystemConfig()
