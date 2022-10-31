"""Script used to test the /api/v1/system/tunable endpoint."""
import e2e_test_framework


class APIE2ETestTunable(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/tunable endpoint."""
    uri = "/api/v1/system/tunable"
    get_tests = [{"name": "Read system tunables"}]
    post_tests = [
        {
            "name": "Create system tunable",
            "req_data": {"tunable": e2e_test_framework.APIE2ETest.uid, "value": 0, "descr": "E2E test"}
        }
    ]
    put_tests = [
        {
            "name": "Update system tunable",
            "req_data": {"id": e2e_test_framework.APIE2ETest.uid, "value": 2, "descr": "E2E test updated"}
        }
    ]
    delete_tests = [
        {
            "name": "Delete system tunable",
            "req_data": {"id": e2e_test_framework.APIE2ETest.uid}
        }
    ]


APIE2ETestTunable()
