import unit_test_framework

class APIUnitTestTunable(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/tunable"
    get_payloads = [{}]
    post_payloads = [{"tunable": unit_test_framework.APIUnitTest.uid, "value": 1, "descr": "Unit test"}]
    put_payloads = [{"id": unit_test_framework.APIUnitTest.uid, "value": 2, "descr": "Unit test updated"}]
    delete_payloads = [{"id": unit_test_framework.APIUnitTest.uid}]

APIUnitTestTunable()