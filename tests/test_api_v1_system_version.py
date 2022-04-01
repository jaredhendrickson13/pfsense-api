import e2e_test_framework


class APIE2ETestSystemVersion(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/version"
    get_tests = [{"name": "Read pfSense version"}]


APIE2ETestSystemVersion()
