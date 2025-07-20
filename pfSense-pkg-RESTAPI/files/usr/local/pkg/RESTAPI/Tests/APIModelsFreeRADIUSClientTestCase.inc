<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FreeRADIUSClient;

class APIModelsFreeRADIUSClientTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-freeradius3'];

    /**
     * Checks that we can create, read and delete FreeRADIUSClient models.
     */
    public function test_crud(): void {
        # Create a new FreeRADIUSClient
        $client = new FreeRADIUSClient(
            addr: '1.2.3.4',
            ip_version: 'ipaddr',
            shortname: 'testclient',
            secret: 'testsecret',
            proto: 'udp',
            nastype: 'dot1x',
            msgauth: false,
            maxconn: 16,
            naslogin: 'testlogin',
            naspassword: 'testpassword',
            description: 'Test client',
        );
        $client->create();
        $raddb = file_get_contents('/usr/local/etc/raddb/clients.conf');
        $this->assert_str_contains($raddb, 'client "testclient" {');
        $this->assert_str_contains($raddb, 'ipaddr = 1.2.3.4');
        $this->assert_str_contains($raddb, 'proto = udp');
        $this->assert_str_contains($raddb, 'nas_type = dot1x');
        $this->assert_str_contains($raddb, 'login = testlogin');
        $this->assert_str_contains($raddb, 'password = testpassword');
        $this->assert_str_contains($raddb, 'require_message_authenticator = no');
        $this->assert_str_contains($raddb, 'max_connections = 16');

        # Ensure we can read the created user from the config
        $read_client = new FreeRADIUSClient(id: $client->id);
        $this->assert_equals($read_client->addr->value, '1.2.3.4');
        $this->assert_equals($read_client->ip_version->value, 'ipaddr');
        $this->assert_equals($read_client->shortname->value, 'testclient');
        $this->assert_str_contains($read_client->proto->value, 'udp');
        $this->assert_str_contains($read_client->nastype->value, 'dot1x');
        $this->assert_equals($read_client->msgauth->value, false);
        $this->assert_equals($read_client->maxconn->value, 16);
        $this->assert_equals($read_client->naslogin->value, 'testlogin');
        $this->assert_equals($read_client->naspassword->value, 'testpassword');
        $this->assert_equals($read_client->description->value, 'Test client');

        # Ensure we can update the user
        $client = new FreeRADIUSClient(
            id: $read_client->id,
            addr: '4321::1',
            ip_version: 'ipv6addr',
            shortname: 'newtestclient',
            secret: 'newtestsecret',
            proto: 'tcp',
            nastype: 'cisco',
            msgauth: true,
            maxconn: 32,
            naslogin: 'newtestlogin',
            naspassword: 'newtestpassword',
            description: 'New test client',
        );
        $client->update();
        $raddb = file_get_contents('/usr/local/etc/raddb/clients.conf');
        $this->assert_str_does_not_contain($raddb, 'client "testclient" {');
        $this->assert_str_contains($raddb, 'client "newtestclient" {');
        $this->assert_str_contains($raddb, 'ipv6addr = 4321::1');
        $this->assert_str_contains($raddb, 'proto = tcp');
        $this->assert_str_contains($raddb, 'nas_type = cisco');
        $this->assert_str_contains($raddb, 'login = newtestlogin');
        $this->assert_str_contains($raddb, 'password = newtestpassword');
        $this->assert_str_contains($raddb, 'require_message_authenticator = yes');
        $this->assert_str_contains($raddb, 'max_connections = 32');

        # Delete the user and ensure it is removed from the database
        $client->delete();
        $raddb = file_get_contents('/usr/local/etc/raddb/clients.conf');
        $this->assert_str_does_not_contain($raddb, 'client "newtestclient" {');
    }

    /**
     * Checks that an error is thrown if the ip_version does not match the value provided
     * in the addr field.
     */
    public function test_ip_version_mismatch(): void {
        $this->assert_throws_response(
            response_id: 'FREERADIUS_CLIENT_ADDR_IPV4_NOT_ALLOWED',
            code: 400,
            callable: function () {
                $client = new FreeRADIUSClient(ip_version: 'ipv6addr');
                $client->validate_addr('1.2.3.4');
            },
        );
        $this->assert_throws_response(
            response_id: 'FREERADIUS_CLIENT_ADDR_IPV6_NOT_ALLOWED',
            code: 400,
            callable: function () {
                $client = new FreeRADIUSClient(ip_version: 'ipaddr');
                $client->validate_addr('1234::1');
            },
        );

        # Ensure * is always allowed
        $this->assert_does_not_throw(
            callable: function () {
                $client = new FreeRADIUSClient(ip_version: 'ipaddr');
                $client->validate_addr('*');
                $client = new FreeRADIUSClient(ip_version: 'ipv6addr');
                $client->validate_addr('*');
            },
        );
    }
}
