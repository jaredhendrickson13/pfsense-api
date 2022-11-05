"""Class used to test the /api/v1/system/notifications/email endpoint."""
import e2e_test_framework


class APIE2ETestSystemNotificationsEmail(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/notifications/email endpoint."""
    uri = "/api/v1/system/notifications/email"
    get_tests = [
        {"name": "Read the system email notification settings", "resp_data_empty": True}
    ]
    put_tests = [
        {
            "name": "Check IP address requirement",
            "status": 400,
            "return": 1061
        },
        {
            "name": "Check IP address validation",
            "status": 400,
            "return": 1062,
            "req_data": {"ipaddress": "!@#INVALID#@!"}
        },
        {
            "name": "Check port requirement",
            "status": 400,
            "return": 1063,
            "req_data": {"ipaddress": "test.example.com"}
        },
        {
            "name": "Check port validation",
            "status": 400,
            "return": 1064,
            "req_data": {"ipaddress": "test.example.com", "port": 1000000}
        },
        {
            "name": "Check timeout validation",
            "status": 400,
            "return": 1065,
            "req_data": {"ipaddress": "test.example.com", "port": 25, "timeout": 0}
        },
        {
            "name": "Check from address requirement",
            "status": 400,
            "return": 1066,
            "req_data": {"ipaddress": "test.example.com", "port": 25, "timeout": 25}
        },
        {
            "name": "Check from address validation",
            "status": 400,
            "return": 1067,
            "req_data": {"ipaddress": "test.example.com", "port": 25, "timeout": 25, "fromaddress": "NOT A VALID EMAIL"}
        },
        {
            "name": "Check notify address requirement",
            "status": 400,
            "return": 1068,
            "req_data": {
                "ipaddress": "test.example.com",
                "port": 25,
                "timeout": 25,
                "fromaddress": "noreply@example.com"
            }
        },
        {
            "name": "Check notify address validation",
            "status": 400,
            "return": 1069,
            "req_data": {
                "ipaddress": "test.example.com",
                "port": 25,
                "timeout": 25,
                "fromaddress": "noreply@example.com",
                "notifyemailaddress": "NOT A VALID EMAIL"
            }
        },
        {
            "name": "Check authentication mechanism requirement",
            "status": 400,
            "return": 1070,
            "req_data": {
                "ipaddress": "test.example.com",
                "port": 25,
                "timeout": 25,
                "fromaddress": "noreply@example.com",
                "notifyemailaddress": "recipient@example.com"
            }
        },
        {
            "name": "Check authentication mechanism choices",
            "status": 400,
            "return": 1071,
            "req_data": {
                "ipaddress": "test.example.com",
                "port": 25,
                "timeout": 25,
                "fromaddress": "noreply@example.com",
                "notifyemailaddress": "recipient@example.com",
                "authentication_mechanism": "INVALID"
            }
        },
        {
            "name": "Update email notification settings (LOGIN)",
            "req_data": {
                "disabled": True,
                "ipaddress": "smtp.example.com",
                "port": 25,
                "timeout": 10,
                "ssl": True,
                "sslvalidate": True,
                "fromaddress": "noreply@example.com",
                "notifyemailaddress": "recipient@example.com",
                "username": "testuser",
                "password": "testpassword",
                "authentication_mechanism": "LOGIN"
        }
        },
        {
            "name": "Update email notification settings (PLAIN)",
            "req_data": {
                "disabled": False,
                "ipaddress": "127.0.0.1",
                "port": 25,
                "timeout": 10,
                "ssl": False,
                "sslvalidate": False,
                "fromaddress": "noreply@example.com",
                "notifyemailaddress": "recipient@example.com",
                "authentication_mechanism": "PLAIN"
            }
        }
    ]


APIE2ETestSystemNotificationsEmail()
