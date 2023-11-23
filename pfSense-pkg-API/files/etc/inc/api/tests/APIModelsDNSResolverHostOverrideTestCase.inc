<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\DNSResolverHostOverride;

class APIModelsDNSResolverHostOverrideTestCase extends TestCase
{
    /**
     * Ensures that host overrides are correctly configured in Unbound after creation, modification, and deletion.
     */
    public function test_create_update_and_delete() {
        # Create a host override to test with
        $host = new DNSResolverHostOverride(
            data: ["host" => "test", "domain" => "example.com", "ip" => ["127.0.0.53", "127.0.0.54"]],
            async: false
        );
        $host->create(apply: true);
        # Read the Unbound host entries config
        $unbound_config = file_get_contents("/var/unbound/host_entries.conf");

        # Ensure the first IP has correct A and PTR records
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data: "test.example.com. A 127.0.0.53'
            )
        );
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.53 test.example.com"'
            )
        );

        # Ensure the second IP has correct A and PTR records
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data: "test.example.com. A 127.0.0.54'
            )
        );
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.54 test.example.com"'
            )
        );

        # Update the host override with new values
        $host->from_representation(["host" => "update", "domain" => "example.com", "ip" => ["127.0.0.5", "127.0.0.6"]]);
        $host->update(apply: true);

        # Re-read the Unbound host entries config
        $unbound_config = file_get_contents("/var/unbound/host_entries.conf");

        # Ensure the old values are no longer present
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data: "test.example.com. A 127.0.0.53'
            )
        );
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.53 test.example.com"'
            )
        );
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data: "test.example.com. A 127.0.0.54'
            )
        );
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.54 test.example.com"'
            )
        );

        # Ensure the new values are present
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data: "update.example.com. A 127.0.0.5'
            )
        );
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.5 update.example.com"'
            )
        );
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data: "update.example.com. A 127.0.0.6'
            )
        );
        $this->assert_is_true(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.6 update.example.com"'
            )
        );

        # Delete the host override and re-read the Unbound config
        $host->delete(apply: true);
        $unbound_config = file_get_contents("/var/unbound/host_entries.conf");

        # Ensure the deleted entries are no longer present
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data: "update.example.com. A 127.0.0.5'
            )
        );
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.5 update.example.com"'
            )
        );
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data: "update.example.com. A 127.0.0.6'
            )
        );
        $this->assert_is_false(
            str_contains(
                $unbound_config,
                'local-data-ptr: "127.0.0.6 update.example.com"'
            )
        );
    }
}