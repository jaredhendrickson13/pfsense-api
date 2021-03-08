import unit_test_framework

class APIUnitTestTunable(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/tunable"
    get_tests = [{}]
    post_tests = [
        {"payload": {"tunable": unit_test_framework.APIUnitTest.uid, "value": 1, "descr": "Unit test"}}
    ]
    put_tests = [
        {"payload": {"id": unit_test_framework.APIUnitTest.uid, "value": 2, "descr": "Unit test updated"}}
    ]
    delete_tests = [
        {"payload": {"id": unit_test_framework.APIUnitTest.uid}}
    ]

APIUnitTestTunable()