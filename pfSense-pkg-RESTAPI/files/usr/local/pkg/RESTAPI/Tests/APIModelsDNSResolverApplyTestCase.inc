<?php

namespace RESTAPI\Tests;


use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSResolverApply;
use RESTAPI\Models\DNSResolverHostOverride;

class APIModelsDNSResolverApplyTestCase extends TestCase
{
    /**
     * Ensure the Model is correctly able to apply changes to the DNS Resolver configuration and determine if there
     * are any pending changes.
     */
    public function test_apply(): void {

        # Make a change to the DNS Resolver configuration and ensure we can see that there are pending changes
        $host_override = new DNSResolverHostOverride();
        $host_override->replace_all(data: []);
        $dns_apply = new DNSResolverApply(async: false);
        $this->assert_is_false($dns_apply->applied->value);
        
        # Apply the change and ensure that the applied status is now true
        $dns_apply->apply();
        $dns_apply = new DNSResolverApply(async: false);
        $this->assert_is_true($dns_apply->applied->value);
    }
}
