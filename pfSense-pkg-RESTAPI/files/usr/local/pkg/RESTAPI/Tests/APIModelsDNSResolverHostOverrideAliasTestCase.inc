<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSResolverHostOverride;
use RESTAPI\Models\DNSResolverHostOverrideAlias;

class APIModelsDNSResolverHostOverrideAliasTestCase extends TestCase
{
    /**
     * Ensures that host override aliases are correctly configured in Unbound after creation, modification, and deletion.
     */
    public function test_create_update_and_delete()
    {
        # Create a parent host override to test with
        $host = new DNSResolverHostOverride(
            data: ['host' => 'test', 'domain' => 'example.com', 'ip' => ['127.0.0.53']],
            async: false,
        );
        $host->create();

        # Add an alias to the parent host override
        $alias = new DNSResolverHostOverrideAlias(
            parent_id: $host->id,
            data: ['host' => 'alias', 'domain' => 'example.com'],
            async: false,
        );
        $alias->create(apply: true);

        # Read the Unbound host entries config
        $unbound_config = file_get_contents('/var/unbound/host_entries.conf');

        # Ensure an A record exists for the alias that matches the parent host override's IP
        $this->assert_str_contains($unbound_config, 'local-data: "alias.example.com. A 127.0.0.53');

        # Update the host override with new values
        $alias->host->value = 'updated-alias';
        $alias->update(apply: true);

        # Re-read the Unbound host entries config
        $unbound_config = file_get_contents('/var/unbound/host_entries.conf');

        # Ensure the old values are no longer present
        $this->assert_str_does_not_contain($unbound_config, 'local-data: "alias.example.com. A 127.0.0.53');

        # Ensure the new values are present
        $this->assert_str_contains($unbound_config, 'local-data: "updated-alias.example.com. A 127.0.0.53');

        # Delete the host override and re-read the Unbound config
        $alias->delete(apply: true);
        $unbound_config = file_get_contents('/var/unbound/host_entries.conf');

        # Ensure the deleted entry is no longer present
        $this->assert_str_does_not_contain($unbound_config, 'local-data: "updated-alias.example.com. A 127.0.0.53');

        # Delete the parent host override
        $host->delete(apply: true);
    }
}
