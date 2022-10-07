"""Script used to test the login protection API integration"""
import sys
import requests
import e2e_test_framework


class APIE2ETestLoginProtection(e2e_test_framework.APIE2ETest):
    """Class used to test the login protection API integration."""
    def custom_tests(self):
        self.test_login_protection()

    def test_login_protection(self):
        """Custom test method to ensure login protection locks out too many failed auth attempts."""
        # Variables
        test_params = {"name": "Ensure login protection blocks many failed auth attempts"}

        # Fail authentication many times to initiate the lockout
        for _ in range(0, 5):
            try:
                requests.get(
                    self.format_url("/api/v1/system/api"),
                    auth=("bad_username", "bad_password"),
                    verify=False,
                    timeout=(5, 5)
                )
            except (requests.exceptions.ReadTimeout, requests.exceptions.ConnectTimeout):
                print(self.__format_msg__("GET", test_params, "Response is valid", mode="ok"))
                sys.exit(0)

        # Test fails if we were not locked out
        print(self.__format_msg__("GET", test_params, "Expected lockout for too many failed requests"))
        sys.exit(1)


APIE2ETestLoginProtection()
