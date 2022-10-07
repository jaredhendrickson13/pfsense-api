"""Script used to test the login protection API integration"""
import e2e_test_framework
import requests
import sys


class APIE2ETestLoginProtection(e2e_test_framework.APIE2ETest):
    """Class used to test the login protection API integration."""
    def custom_tests(self):
        self.test_login_protection()

    def test_login_protection(self):
        """Custom test method to ensure login protection locks out too many failed auth attempts."""
        # Variables
        test_params = {"name": "Ensure login protection blocks many failed auth attempts"}

        # Fail authentication 3 times to initiate the lockout
        for _ in range(0, 3):
            requests.get(
                self.format_url("/api/v1/system/api"),
                auth=("bad_username", "bad_password"),
                verify=False
            )

        # Try another request and ensure it times out as expected
        try:
            requests.get(
                self.format_url("/api/v1/system/api"),
                auth=("bad_username", "bad_password"),
                verify=False,
                timeout=(5, 5)
            )
            print(self.__format_msg__("GET", test_params, "Expected lockout for too many failed requests"))
            sys.exit(1)
        except (requests.exceptions.ReadTimeout, requests.exceptions.ConnectTimeout):
            print(self.__format_msg__("GET", test_params, "Response is valid", mode="ok"))


APIE2ETestLoginProtection()
