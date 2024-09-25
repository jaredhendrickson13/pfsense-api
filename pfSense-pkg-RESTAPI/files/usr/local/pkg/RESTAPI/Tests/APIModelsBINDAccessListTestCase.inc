<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDAccessList;

class APIModelsBINDAccessListTestCase extends TestCase
{
    public array $required_packages = ['pfSense-pkg-bind'];

    /**
     * Check that we can create, read, update and delete BIND access lists.
     */
    public function test_crud(): void {
        # Create a new access list
        $bind_acl = new BINDAccessList(
            async: false,
            name: 'test_acl',
            description: 'A test access list.',
            entries: [
                ["value" => "1.2.3.4/32", "description" => "A test entry."]
            ]
        );
        $bind_acl->create(apply: true);

        # Ensure we can read the access list
        $acl_q = BINDAccessList::query(name: 'test_acl');
        $this->assert_is_true($acl_q->exists());
        $this->assert_equals($acl_q->first()->name->value, 'test_acl');
        $this->assert_equals($acl_q->first()->description->value, 'A test access list.');
        $this->assert_equals($acl_q->first()->entries->value[0]['value'], '1.2.3.4/32');
        $this->assert_equals($acl_q->first()->entries->value[0]['description'], 'A test entry.');


        # Ensure the ACL is configured in the named.conf file
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_contains($named_conf, 'acl "test_acl" {');
        $this->assert_str_contains($named_conf, '1.2.3.4/32;');

        # Update the access list
        $bind_acl->description->value = 'An updated test access list.';
        $bind_acl->entries->value = [
            ["value" => "4.3.2.1/24", "description" => "An updated test entry."]
        ];
        $bind_acl->update(apply: true);

        # Ensure the access list was updated
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_contains($named_conf, '4.3.2.1/24');

        # Delete the access list
        $bind_acl->delete(apply: true);

        # Ensure the access list was deleted
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_does_not_contain($named_conf, 'test_acl');
    }
}