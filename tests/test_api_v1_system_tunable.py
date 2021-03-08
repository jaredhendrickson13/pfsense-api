import unit_test_framework


class APIUnitTestTunable(unit_test_framework.APIUnitTest):
    url = "/api/v1/system/tunable"
    get_tests = [{"name": "Read system tunables"}]
    post_tests = [
        {
            "name": "Create system tunable",
            "payload": {"tunable": unit_test_framework.APIUnitTest.uid, "value": 1, "descr": "Unit test"}
        }
    ]
    put_tests = [
        {
            "name": "Update system tunable",
            "payload": {"id": unit_test_framework.APIUnitTest.uid, "value": 2, "descr": "Unit test updated"}
        }
    ]
    delete_tests = [
        {
            "name": "Delete system tunable",
            "payload": {"id": unit_test_framework.APIUnitTest.uid}
        }
    ]


APIUnitTestTunable()
