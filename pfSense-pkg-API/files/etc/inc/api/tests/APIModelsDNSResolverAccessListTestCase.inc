<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\DNSResolverAccessList;

class APIModelsDNSResolverAccessListTestCase extends TestCase
{
    /**
     * Checks that the DNS Resolver (Unbound) access list is correctly configured after creation/update/delete
     */
    public function test_crud() {
        # Create an access list to test with
        $access_list = new DNSResolverAccessList(
            data: [
                "name" => "test",
                "action" => "allow",
                "networks" => [
                    ["network" => "1.2.3.4", "mask" => 32],
                    ["network" => "4.3.2.1", "mask" => 24],
                ]
            ],
            async: false
        );
        $access_list->create(apply: true);

        # Ensure the access list exists in /var/unbound/access_lists.conf
        $unbound_acls = file_get_contents("/var/unbound/access_lists.conf");
        $this->assert_str_contains(
            $unbound_acls,
            "#test\naccess-control: 1.2.3.4/32 allow\naccess-control: 4.3.2.1/24 allow"
        );

        # Update the access list with new values
        $access_list->from_representation(
            [
                "name" => "updated-test",
                "action" => "deny",
                "networks" => [
                    ["network" => "127.0.0.5", "mask" => 29],
                    ["network" => "127.0.0.1", "mask" => 25],
                ]
            ]
        );
        $access_list->update(apply: true);

        # Ensure the new access list exists and the previous one doesn't
        $unbound_acls = file_get_contents("/var/unbound/access_lists.conf");
        $this->assert_str_contains(
            $unbound_acls,
            "#test\naccess-control: 1.2.3.4/32 allow\naccess-control: 4.3.2.1/24 allow"
        );
        $this->assert_str_contains(
            $unbound_acls,
            "#updated-test\naccess-control: 127.0.0.5/29 deny\naccess-control: 127.0.0.1/25 deny"
        );

        # Delete the access list and ensure it no longer exists
        $access_list->delete(apply: true);
        $unbound_acls = file_get_contents("/var/unbound/access_lists.conf");
        $this->assert_str_does_not_contain(
            $unbound_acls,
            "#updated-test\naccess-control: 127.0.0.5/29 deny\naccess-control: 127.0.0.1/25 deny"
        );
    }
}