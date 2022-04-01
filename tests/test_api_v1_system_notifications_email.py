import e2e_test_framework


class APIE2ETestSystemNotificationsEmail(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/notifications/email"
    get_tests = [{"name": "Read the system email notification settings"}]
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
            "payload": {"ipaddress": "!@#INVALID#@!"}
        },
        {
            "name": "Check port requirement",
            "status": 400,
            "return": 1063,
            "payload": {"ipaddress": "test.example.com"}
        },
        {
            "name": "Check port validation",
            "status": 400,
            "return": 1064,
            "payload": {"ipaddress": "test.example.com", "port": 1000000}
        },
        {
            "name": "Check timeout validation",
            "status": 400,
            "return": 1065,
            "payload": {"ipaddress": "test.example.com", "port": 25, "timeout": 0}
        },
        {
            "name": "Check from address requirement",
            "status": 400,
            "return": 1066,
            "payload": {"ipaddress": "test.example.com", "port": 25, "timeout": 25}
        },
        {
            "name": "Check from address validation",
            "status": 400,
            "return": 1067,
            "payload": {"ipaddress": "test.example.com", "port": 25, "timeout": 25, "fromaddress": "NOT A VALID EMAIL"}
        },
        {
            "name": "Check notify address requirement",
            "status": 400,
            "return": 1068,
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
            "payload": {
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
