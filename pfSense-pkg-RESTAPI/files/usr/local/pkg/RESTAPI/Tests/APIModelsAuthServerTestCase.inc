<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\AuthServer;

class APIModelsAuthServerTestCase extends TestCase
{
    /**
     * Checks that RADIUS authentication servers can be created, updated and deleted.
     */
    public function test_radius_crud(): void {
        # Create a new RADIUS server
        $radius = new AuthServer(
            type: "radius",
            name: "TEST_RADIUS",
            host: "radius.example.com",
            radius_auth_port: "1812",
            radius_acct_port: "1813",
            radius_secret: "secret",
            radius_protocol: "MSCHAPv2",
            radius_timeout: 30,
            radius_nasip_attribute: "wan"
        );
        $radius->create();
        
        # Ensure we can read the created AuthServer and ensure the values match. Also ensure it has a UID.
        $read_radius = new AuthServer(id: $radius->id);
        $this->assert_equals($radius->to_representation(), $read_radius->to_representation());
        $this->assert_is_not_empty($radius->refid);
        
        # Update the AuthServer and ensure we can read the updated values
        $radius = new AuthServer(
            id: $radius->id,
            type: "radius",
            name: "TEST_RADIUS",
            host: "new.radius.example.com",
            radius_auth_port: "1218",
            radius_acct_port: "1318",
            radius_secret: "new.secret",
            radius_protocol: "PAP",
            radius_timeout: 10,
            radius_nasip_attribute: "lan"
        );
        $radius->update();

        # Ensure we can read the created AuthServer and ensure the values match. Also ensure it has a UID.
        $read_radius = new AuthServer(id: $radius->id);
        $this->assert_equals($radius->to_representation(), $read_radius->to_representation());
        
        # Delete the AuthServer and ensure we can no longer read it.
        $radius->delete();
        $this->assert_is_false($radius->query(id: $radius->id)->exists());
    }
    
    /**
     * Checks that LDAP authentication servers can be created, updated and deleted.
     */
    public function test_ldap_crud(): void {
        # Create a new LDAP server
        $ldap = new AuthServer(
            type: "ldap",
            name: "TEST_LDAP",
            host: "ldap.example.com",
            ldap_port: "389",
            ldap_urltype: "Standard TCP",
            ldap_protver: 3,
            ldap_timeout: 30,
            ldap_caref: "global",
            ldap_scope: "subtree",
            ldap_basedn: "DC=example,DC=com",
            ldap_authcn: " CN=Users;DC=example,DC=com",
            ldap_extended_enabled: true,
            ldap_extended_query: "memberOf=CN=Groupname,OU=MyGroups,DC=example,DC=com",
            ldap_binddn: "CN=user,OU=group,DC=example,DC=com",
            ldap_bindpw: "secret",
            ldap_attr_user: "cn",
            ldap_attr_group: "ou",
            ldap_attr_member: "member",
            ldap_rfc2307: true,
            ldap_rfc2307_userdn: true,
            ldap_attr_groupobj: "posixGroup",
            ldap_pam_groupdn: "shellGroup",
            ldap_utf8: true,
            ldap_nostrip_at: true,
            ldap_allow_unauthenticated: true,
        );
        $ldap->create();

        # Ensure we can read the created AuthServer and ensure the values match. Also ensure it has a UID.
        $read_ldap = new AuthServer(id: $ldap->id);
        $this->assert_equals($ldap->to_representation(), $read_ldap->to_representation());
        $this->assert_is_not_empty($ldap->refid);

        # Update the AuthServer and ensure we can read the updated values
        $ldap = new AuthServer(
            id: $ldap->id,
            type: "ldap",
            name: "TEST_LDAP",
            host: "new.ldap.example.com",
            ldap_port: "636",
            ldap_urltype: "SSL/TLS Encrypted",
            ldap_protver: 2,
            ldap_timeout: 15,
            ldap_caref: "global",
            ldap_scope: "one",
            ldap_basedn: "DC=new.example,DC=com",
            ldap_authcn: " CN=NewUsers;DC=new.example,DC=com",
            ldap_extended_enabled: false,
            ldap_binddn: null,
            ldap_attr_user: "CN",
            ldap_attr_group: "OU",
            ldap_attr_member: "MemberOf",
            ldap_rfc2307: false,
            ldap_rfc2307_userdn: false,
            ldap_pam_groupdn: "ShellGroup",
            ldap_utf8: false,
            ldap_nostrip_at: false,
            ldap_allow_unauthenticated: false,
        );
        $ldap->update();

        # Ensure we can read the created AuthServer and ensure the values match. Also ensure it has a UID.
        $read_ldap = new AuthServer(id: $ldap->id);
        $this->assert_equals($ldap->to_representation(), $read_ldap->to_representation());

        # Delete the AuthServer and ensure we can no longer read it.
        $ldap->delete();
        $this->assert_is_false($ldap->query(id: $ldap->id)->exists());
    }
}
