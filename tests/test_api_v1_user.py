"""Script used to test the /api/v1/user endpoint."""
import random
import secrets

import e2e_test_framework
from e2e_test_framework.tools import generate_random_future_date

# Constants
LOCAL_USER_USERNAME = f"exampleuser{random.randint(1, 9)}"
LOCAL_USER_DESCR_CREATE = "Example User"
LOCAL_USER_DESCR_UPDATE = "New Example User"
LOCAL_USER_PASSWD_CREATE = secrets.token_urlsafe(12)
LOCAL_USER_PASSWD_UPDATE = secrets.token_urlsafe(12)
LOCAL_USER_AUTHKEY_CREATE = f"ssh-rsa {secrets.token_urlsafe(256)} {LOCAL_USER_USERNAME}@example"
LOCAL_USER_AUTHKEY_UPDATE = f"ssh-rsa {secrets.token_urlsafe(256)} {LOCAL_USER_USERNAME}@example"
LOCAL_USER_PSK_CREATE = secrets.token_urlsafe(32)
LOCAL_USER_PSK_UPDATE = secrets.token_urlsafe(32)
LOCAL_USER_EXP_CREATE = generate_random_future_date()
LOCAL_USER_EXP_UPDATE = generate_random_future_date()
DISABLED_USER_USERNAME = f"disableduser{random.randint(1,9)}"
DISABLED_USER_DESCR = "Example User"
DISABED_USER_PASSWD = secrets.token_urlsafe(12)


class APIE2ETestUser(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/user endpoint."""
    uri = "/api/v1/user"
    get_tests = [{"name": "Read local users"}]
    post_tests = [
        {
            "name": "Create RSA internal CA",
            "uri": "/api/v1/system/ca",
            "post_test_callable": "set_caref",
            "req_data": {
                "method": "internal",
                "descr": "INTERNAL_CA_RSA",
                "trust": True,
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "internal-ca-e2e-test.example.com"
            },
        },
        {
            "name": "Create user certificate with RSA key",
            "uri": "/api/v1/system/certificate",
            "post_test_callable": "set_user_certref",
            "req_data_callable": "get_caref",
            "req_data": {
                "method": "internal",
                "descr": "USER_CERT",
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "new_user",
                "type": "user"
            }
        },
        {
            "name": "Create server certificate with RSA key",
            "uri": "/api/v1/system/certificate",
            "post_test_callable": "set_server_certref",
            "req_data_callable": "get_caref",
            "req_data": {
                "method": "internal",
                "descr": "SERVER_CERT",
                "keytype": "RSA",
                "keylen": 2048,
                "digest_alg": "sha256",
                "lifetime": 3650,
                "dn_commonname": "internal-cert-e2e-test.example.com",
                "type": "server"
            }
        },
        {
            "name": "Create local user",
            "resp_time": 2,     # Allow a couple seconds for user database to be updated
            "post_test_callable": "is_local_user_created",
            "req_data_callable": "get_user_certref",
            "req_data": {
                "disabled": False,
                "username": LOCAL_USER_USERNAME,
                "password": LOCAL_USER_PASSWD_CREATE,
                "descr": LOCAL_USER_DESCR_CREATE,
                "authorizedkeys": LOCAL_USER_AUTHKEY_CREATE,
                "ipsecpsk": LOCAL_USER_PSK_CREATE,
                "expires": LOCAL_USER_EXP_CREATE,
                "priv": ["page-system-usermanager"]
            },
        },
        {
            "name": "Create a disabled local user using the previously created local user",
            "resp_time": 2,  # Allow a couple seconds for user database to be updated
            "username": LOCAL_USER_USERNAME,
            "password": LOCAL_USER_PASSWD_CREATE,
            "req_data": {
                "disabled": True,
                "username": DISABLED_USER_USERNAME,
                "password": DISABED_USER_PASSWD,
            },
        },
        {
            "name": "Disable API login protection for failed authentication checks",
            "method": "PUT",
            "uri": "/api/v1/system/api",
            "req_data": {"enable_login_protection": False}
        },
        {
            "name": "Check disabled user's inability to authenticate",
            "status": 401,
            "return": 3,
            "username": DISABLED_USER_USERNAME,
            "password": DISABED_USER_PASSWD
        },
        {
            "name": "Re-enable API login protection after failed authentication checks",
            "method": "PUT",
            "uri": "/api/v1/system/api",
            "req_data": {"enable_login_protection": True}
        },
        {
            "name": "Check username requirement and login using created user",
            "status": 400,
            "return": 5000,
        },
        {
            "name": "Check username unique constraint",
            "status": 400,
            "return": 5002,
            "req_data": {
                "username": LOCAL_USER_USERNAME
            }
        },
        {
            "name": "Check username character constraint",
            "status": 400,
            "return": 5036,
            "req_data": {
                "username": "!@#"
            }
        },
        {
            "name": "Check reserved username constraint",
            "status": 400,
            "return": 5037,
            "req_data": {
                "username": "root"
            }
        },
        {
            "name": "Check username length constraint",
            "status": 400,
            "return": 5038,
            "req_data": {
                "username": "THIS_USERNAME_IS_TOO_LONG_FOR_PFSENSE_TO_HANDLE"
            }
        },
        {
            "name": "Check password requirement",
            "status": 400,
            "return": 5003,
            "req_data": {
                "username": "another_user"
            }
        },
        {
            "name": "Check privilege validation",
            "status": 400,
            "return": 5006,
            "req_data": {
                "username": "another_user",
                "password": "changeme",
                "priv": ["INVALID"]
            }
        },
        {
            "name": "Check user expiration date validation",
            "status": 400,
            "return": 5040,
            "req_data": {
                "username": "another_user",
                "password": "changeme",
                "expires": "INVALID"
            }
        },
        {
            "name": "Check user certificate exists constraint",
            "status": 400,
            "return": 5041,
            "req_data": {
                "username": "another_user",
                "password": "changeme",
                "cert": "INVALID"
            }
        }
    ]
    put_tests = [
        {
            "name": "Update local user",
            "post_test_callable": "is_local_user_updated",
            "req_data": {
                "disabled": False,
                "username": LOCAL_USER_USERNAME,
                "password": LOCAL_USER_PASSWD_UPDATE,
                "descr": LOCAL_USER_DESCR_UPDATE,
                "authorizedkeys": LOCAL_USER_AUTHKEY_UPDATE,
                "ipsecpsk": LOCAL_USER_PSK_UPDATE,
                "expires": LOCAL_USER_EXP_UPDATE,
                "cert": []
            },
            "resp_time": 2    # Allow a couple seconds for user database to be updated
        },
        {
            "name": "Check auth with updated local user",
            "username": LOCAL_USER_USERNAME,
            "password": LOCAL_USER_PASSWD_UPDATE,
            "req_data": {
                "username": LOCAL_USER_USERNAME
            },
            "resp_time": 2  # Allow a couple seconds for user database to be updated
        },
        {
            "name": "Check privilege validation",
            "status": 400,
            "return": 5006,
            "req_data": {
                "username": LOCAL_USER_USERNAME,
                "password": "changeme",
                "priv": ["INVALID"]
            }
        },
        {
            "name": "Check user expiration date validation",
            "status": 400,
            "return": 5040,
            "req_data": {
                "username": LOCAL_USER_USERNAME,
                "password": "changeme",
                "expires": "INVALID"
            }
        },
        {
            "name": "Check user certificate exists constraint",
            "status": 400,
            "return": 5041,
            "req_data": {
                "username": LOCAL_USER_USERNAME,
                "password": "changeme",
                "cert": "INVALID"
            }
        },
        {
            "name": "Check ability to add server certificate as a user certificate",
            "server_cert": True,
            "req_data_callable": "get_server_certref",
            "req_data": {
                "username": LOCAL_USER_USERNAME,
                "password": "changeme"
            }
        },
        {
            "name": "Update local user to re-add user certificate",
            "req_data_callable": "get_user_certref",
            "req_data": {
                "username": LOCAL_USER_USERNAME,
            },
            "resp_time": 2    # Allow a couple seconds for user database to be updated
        },
    ]
    delete_tests = [
        {
            "name": "Check inability to delete user certificate while in use",
            "uri": "/api/v1/system/certificate",
            "status": 400,
            "return": 1005,
            "req_data": {"descr": "USER_CERT"}
        },
        {
            "name": "Delete local user",
            "post_test_callable": "is_local_user_deleted",
            "req_data": {"username": LOCAL_USER_USERNAME}
        },
        {
            "name": "Delete disabled user",
            "req_data": {"username": DISABLED_USER_USERNAME}
        },
        {
            "name": "Check deletion of non-existing user",
            "status": 400,
            "return": 5001,
            "req_data": {"username": "INVALID"}
        },
        {
            "name": "Check deletion of system users",
            "status": 400,
            "return": 5005,
            "req_data": {"username": "admin"}
        },
        {
            "name": "Check ability to delete user certificate after user was deleted",
            "uri": "/api/v1/system/certificate",
            "req_data": {"descr": "USER_CERT"}
        },
        {
            "name": "Delete server certificate used for testing",
            "uri": "/api/v1/system/certificate",
            "req_data": {"descr": "SERVER_CERT"}
        },
        {
            "name": "Delete CA used for testing",
            "uri": "/api/v1/system/ca",
            "req_data": {"descr": "INTERNAL_CA_RSA"}
        }
    ]

    def __init__(self):
        """Initialize this test."""
        # Add caref and certref attributes so tests can populate/reference them dynamically
        self.caref = None
        self.user_certref = None
        self.server_certref = None

        super().__init__()

    def set_caref(self):
        """Sets the caref attribute to the refid from the last response so other tests can use it."""
        self.caref = self.last_response["data"]["refid"]

    def set_user_certref(self):
        """Sets the user certref attribute to the refid from the last response so other tests can use it."""
        self.user_certref = self.last_response["data"]["refid"]

    def set_server_certref(self):
        """Sets the server certref attribute to the refid from the last response so other tests can use it."""
        self.server_certref = self.last_response["data"]["refid"]

    def get_caref(self):
        """Gets the caref attribute as a req_data item. Intended for use as a req_data_callable."""
        return {"caref": self.caref}

    def get_user_certref(self):
        """Gets the user certref attribute. Intended for use as a req_data_callable."""
        return {"cert": self.user_certref}

    def get_server_certref(self):
        """Gets the server certref attribute. Intended for use as a req_data_callable."""
        return {"cert": self.server_certref}

    def is_local_user_created(self):
        """Checks if the local user was correctly created in the POST tests"""
        # Local variables
        auth_keys_out = self.pfsense_shell(f"cat /home/{LOCAL_USER_USERNAME}/.ssh/authorized_keys")
        uid = self.last_response.get("data", {}).get("uid", "")    # Get the UID from the last API response

        # Ensure user is present in /etc/passwd
        user_not_in_etc_passwd_out = self.user_not_in_etc_passwd(LOCAL_USER_USERNAME, uid, LOCAL_USER_DESCR_CREATE)
        if user_not_in_etc_passwd_out:
            raise AssertionError(
                f"Expected user '{LOCAL_USER_USERNAME}' with UID '{uid}' and descr '{LOCAL_USER_DESCR_CREATE}' in "
                f"/etc/passwd, got: {user_not_in_etc_passwd_out}"
            )

        # Ensure user's SSH key is correctly added
        if LOCAL_USER_AUTHKEY_CREATE != auth_keys_out:
            raise AssertionError(f"Expected user's authorized key at /home/{LOCAL_USER_USERNAME}/.ssh/authorized_keys")

    def is_local_user_updated(self):
        """Checks if the local user was correctly updated in the PUT tests"""
        # Local variables
        auth_keys_out = self.pfsense_shell(f"cat /home/{LOCAL_USER_USERNAME}/.ssh/authorized_keys")
        uid = self.last_response.get("data", {}).get("uid", "")    # Get the UID from the last API response

        # Ensure user is present in /etc/passwd
        user_not_in_etc_passwd_out = self.user_not_in_etc_passwd(LOCAL_USER_USERNAME, uid, LOCAL_USER_DESCR_UPDATE)
        if user_not_in_etc_passwd_out:
            raise AssertionError(
                f"Expected user '{LOCAL_USER_USERNAME}' with UID '{uid}' and descr '{LOCAL_USER_DESCR_UPDATE}' in "
                f"/etc/passwd, got: {user_not_in_etc_passwd_out}"
            )

        # Ensure user's SSH key is correctly added
        if LOCAL_USER_AUTHKEY_UPDATE != auth_keys_out:
            raise AssertionError(f"Expected user's authorized key at /home/{LOCAL_USER_USERNAME}/.ssh/authorized_keys")

    def is_local_user_deleted(self):
        """Checks if the user local was correctly removed from /etc/passwd in DELETE tests"""
        # Ensure the local user's username is no longer present in /etc/passwd
        user_not_in_etc_passwd_out = self.user_not_in_etc_passwd(LOCAL_USER_USERNAME, username_only=True)
        if not user_not_in_etc_passwd_out:
            raise AssertionError(f"Expected '{LOCAL_USER_USERNAME}' to be removed from /etc/passwd")

    def user_not_in_etc_passwd(self, username: str, uid="", full_name="", username_only=False):
        """
        Checks if a specified user is not present in /etc/passwd
        :param username: the username to check for
        :param uid: the user's uid to check for
        :param full_name: the user's full name to check for
        :param username_only: only check the file for the username, not uid or full_name
        :return: the output for /etc/passwd if user was not in the file, otherwise returns an empty string
        """
        # Local variables
        etc_passwd_out = self.pfsense_shell("cat /etc/passwd")
        etc_passwd_line = f"{username}:*:{uid}:65534:{full_name}:/home/{username}:/sbin/nologin"

        # Only check for the username if username_only was set
        if username_only and username not in etc_passwd_out:
            return etc_passwd_out
        # Otherwise, check for an exact match
        if not username_only and etc_passwd_line not in etc_passwd_out:
            return etc_passwd_out

        # Return false if there were no matches
        return ""





APIE2ETestUser()
