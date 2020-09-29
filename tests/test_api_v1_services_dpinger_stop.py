import unit_test_framework

class APIUnitTestServicesDpingerStop(unit_test_framework.APIUnitTest):
    url = "/api/v1/services/dpinger/stop"
    post_payloads = [{}]

APIUnitTestServicesDpingerStop()