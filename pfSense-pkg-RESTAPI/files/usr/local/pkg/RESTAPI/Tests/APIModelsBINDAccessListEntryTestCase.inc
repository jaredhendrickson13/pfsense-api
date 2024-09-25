<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDAccessList;
use RESTAPI\Models\BINDAccessListEntry;

class APIModelsBINDAccessListEntryTestCase extends TestCase
{
    public array $required_packages = ['pfSense-pkg-bind'];
    public BINDAccessList $bind_acl;

    /**
     * Setup the test case by create a new BIND access list.
     */
    public function setup(): void {
        # Create a new access list
        $this->bind_acl = new BINDAccessList(
            async: false,
            name: 'test_acl',
            description: 'A test access list.',
            entries: [['value' => '127.0.0.1/32']]
        );
        $this->bind_acl->create(apply: true);
    }

    /**
     * Tear down the test case by deleting the BIND access list.
     */
    public function teardown(): void {
        $this->bind_acl->delete(apply: true);
    }

    /**
     * Check that we can create, read, update and delete BIND access list entries.
     */
    public function test_crud(): void
    {
        # Create a new access list entry
        $bind_acl_entry = new BINDAccessListEntry(
            parent_id: $this->bind_acl->id,
            async: false,
            value: '1.2.3.4/32',
            description: 'A test entry.'
        );
        $bind_acl_entry->create(apply: true);

        # Ensure the entry is present in the ACL within named.conf
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_contains($named_conf, '1.2.3.4/32');

        # Update the access list entry
        $bind_acl_entry->value->value = '4.3.2.1/24';
        $bind_acl_entry->update(apply: true);

        # Ensure the entry was updated
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_contains($named_conf, '4.3.2.1/24');
        $this->assert_str_does_not_contain($named_conf, '1.2.3.4/32');

        # Delete the access list entry
        $bind_acl_entry->delete(apply: true);

        # Ensure the entry was deleted
        $named_conf = file_get_contents('/var/etc/named/etc/namedb/named.conf');
        $this->assert_str_does_not_contain($named_conf, '4.3.2.1/24');
    }
}