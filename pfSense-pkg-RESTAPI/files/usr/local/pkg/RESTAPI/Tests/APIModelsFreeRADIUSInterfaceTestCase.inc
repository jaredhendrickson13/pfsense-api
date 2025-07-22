<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FreeRADIUSfr_interface;
use RESTAPI\Models\FreeRADIUSInterface;

class APIModelsFreeRADIUSInterfaceTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-freeradius3'];

    /**
     * Checks that we can create, read and delete FreeRADIUSInterface models.
     */
    public function test_crud(): void {
        # Create a new FreeRADIUSInterface
        $fr_interface = new FreeRADIUSInterface(
            addr: '*',
            port: '1812',
            type: 'auth',
            ip_version: 'ipv6addr',
            description: 'Test interface',
        );
        $fr_interface->create();
        $raddb = file_get_contents('/usr/local/etc/raddb/sites-enabled/default');
        $this->assert_str_contains($raddb, 'type = auth');
        $this->assert_str_contains($raddb, 'ipv6addr = *');
        $this->assert_str_contains($raddb, 'port = 1812');

        # Ensure we can read the created user from the config
        $read_fr_interface = new FreeRADIUSInterface(id: $fr_interface->id);
        $this->assert_equals($read_fr_interface->addr->value, '*');
        $this->assert_equals($read_fr_interface->port->value, '1812');
        $this->assert_equals($read_fr_interface->type->value, 'auth');
        $this->assert_equals($read_fr_interface->ip_version->value, 'ipv6addr');
        $this->assert_equals($read_fr_interface->description->value, 'Test interface');

        # Ensure we can update the user
        $fr_interface = new FreeRADIUSInterface(
            id: $fr_interface->id,
            addr: '127.0.0.1',
            port: '1813',
            type: 'acct',
            ip_version: 'ipaddr',
            description: 'New test interface',
        );
        $fr_interface->update();
        $raddb = file_get_contents('/usr/local/etc/raddb/sites-enabled/default');
        $this->assert_str_does_not_contain($raddb, 'type = auth');
        $this->assert_str_does_not_contain($raddb, 'ipv6addr = *');
        $this->assert_str_does_not_contain($raddb, 'port = 1812');
        $this->assert_str_contains($raddb, 'type = acct');
        $this->assert_str_contains($raddb, 'ipaddr = 127.0.0.1');
        $this->assert_str_contains($raddb, 'port = 1813');

        # Delete the user and ensure it is removed from the database
        $fr_interface->delete();
        $raddb = file_get_contents('/usr/local/etc/raddb/sites-enabled/default');
        $this->assert_str_does_not_contain($raddb, 'listen {');
    }

    /**
     * Checks that an error is thrown if the ip_version does not match the value provided
     * in the addr field.
     */
    public function test_ip_version_mismatch(): void {
        $this->assert_throws_response(
            response_id: 'FREERADIUS_INTERFACE_ADDR_IPV4_NOT_ALLOWED',
            code: 400,
            callable: function () {
                $fr_interface = new FreeRADIUSInterface(addr: '1.2.3.4', ip_version: 'ipv6addr');
                $fr_interface->addr->validate(); # Validate to so labels are populated
                $fr_interface->validate_addr('1.2.3.4');
            },
        );
        $this->assert_throws_response(
            response_id: 'FREERADIUS_INTERFACE_ADDR_IPV6_NOT_ALLOWED',
            code: 400,
            callable: function () {
                $fr_interface = new FreeRADIUSInterface(addr: '1234::1', ip_version: 'ipaddr');
                $fr_interface->addr->validate(); # Validate to so labels are populated
                $fr_interface->validate_addr('1234::1');
            },
        );

        # Ensure * is always allowed
        $this->assert_does_not_throw(
            callable: function () {
                $fr_interface = new FreeRADIUSInterface(ip_version: 'ipaddr');
                $fr_interface->validate_addr('*');
                $fr_interface = new FreeRADIUSInterface(ip_version: 'ipv6addr');
                $fr_interface->validate_addr('*');
            },
        );
    }
}
