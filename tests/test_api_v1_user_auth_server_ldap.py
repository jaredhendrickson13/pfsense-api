"""Script used to test the /api/v1/user/auth_server/ldap endpoint."""
import e2e_test_framework


class APIE2ETestUserAuthServerLDAP(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/user/auth_server/ldap endpoint."""
    uri = "/api/v1/user/auth_server/ldap"
    get_tests = [{"name": "Read LDAP auth servers"}]
    post_tests = [
        {
            "name": "Create LDAP auth server",
            "req_data": {
                "name": "TEST_AUTHSERVER",
                "host": "ldap.com",
                "ldap_port": 636,
                "ldap_urltype": "encrypted",
                "ldap_protver": "3",
                "ldap_timeout": 12,
                "ldap_scope": "subtree",
                "ldap_basedn": "dc=test,dc=com",
                "ldap_authcn": "ou=people,dc=test,dc=com;ou=groups,dc=test,dc=com",
                "ldap_attr_user": "uid",
                "ldap_attr_group": "cn",
                "ldap_attr_member": "memberof",
                "ldap_attr_groupobj": "posixGroup",
                "ldap_binddn": "cn=pfsense,dc=test,dc=com",
                "ldap_bindpw": "asdf",
                "active": False
            }
        }
    ]
    delete_tests = [
        {
            "name": "Delete LDAP auth server",
            "req_data": {"name": "TEST_AUTHSERVER"}
        }
    ]


APIE2ETestUserAuthServerLDAP()
