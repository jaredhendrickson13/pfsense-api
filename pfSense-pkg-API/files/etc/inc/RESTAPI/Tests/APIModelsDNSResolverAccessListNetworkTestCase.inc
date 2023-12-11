<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSResolverAccessList;
use RESTAPI\Models\DNSResolverAccessListNetwork;

class APIModelsDNSResolverAccessListNetworkTestCase extends TestCase
{
    /**
     * Checks that the DNS Resolver (Unbound) access list is correctly configured after creation/update/delete
     */
    public function test_crud() {
        # Create a parent access list to test with
        $access_list = new DNSResolverAccessList(
            data: [
                "name" => "test",
                "action" => "allow snoop",
                "networks" => [
                    ["network" => "1.2.3.4", "mask" => 32]
                ]
            ],
            async: false
        );
        $access_list->create(apply: true);

        # Create an access list network to test with
        $access_list_network = new DNSResolverAccessListNetwork(
            parent_id: $access_list->id,
            data: ["network" => "4.3.2.1", "mask" => 24],
            async: false
        );
        $access_list_network->create(apply: true);

        # Ensure the access list exists in /var/unbound/access_lists.conf
        $unbound_acls = file_get_contents("/var/unbound/access_lists.conf");
        $this->assert_str_contains(
            $unbound_acls,
            "#test\naccess-control: 1.2.3.4/32 allow_snoop\naccess-control: 4.3.2.1/24 allow_snoop"
        );

        # Update the access list network with new values
        $access_list_network->from_representation(["network" => "127.0.0.5", "mask" => 29]);
        $access_list_network->update(apply: true);

        # Ensure the new access list exists and the previous one doesn't
        $unbound_acls = file_get_contents("/var/unbound/access_lists.conf");
        $this->assert_str_does_not_contain(
            $unbound_acls,
            "#test\naccess-control: 1.2.3.4/32 allow_snoop\naccess-control: 4.3.2.1/24 allow_snoop"
        );
        $this->assert_str_contains(
            $unbound_acls,
            "#test\naccess-control: 1.2.3.4/32 allow_snoop\naccess-control: 127.0.0.5/29 allow_snoop"
        );

        # Delete the access list and ensure it no longer exists
        $access_list_network->delete(apply: true);
        $unbound_acls = file_get_contents("/var/unbound/access_lists.conf");
        $this->assert_str_does_not_contain(
            $unbound_acls,
            "#test\naccess-control: 1.2.3.4/32 allow_snoop\naccess-control: 127.0.0.5/29 allow_snoop"
        );

        # Delete the parent access list
        $access_list->delete(apply: true);
    }

    /**
     * Checks that IPv4 network entries cannot use a `mask` greater than `32`
     */
    public function test_validate_mask() {
        # Try to create an IPv4 access list network entry with a mask greater than 32
        $access_list_network = new DNSResolverAccessListNetwork(
            data: ["network" => "4.3.2.1", "mask" => 33],
            async: false
        );
        $this->assert_throws_response(
            response_id: "DNS_RESOLVER_ACCESS_LIST_NETWORK_MASK_EXCEEDS_MAXIMUM_IPV4",
            code: 400,
            callable: function () use ($access_list_network) {
                $access_list_network->validate(skip_parent: true);
            }
        );
    }
}
