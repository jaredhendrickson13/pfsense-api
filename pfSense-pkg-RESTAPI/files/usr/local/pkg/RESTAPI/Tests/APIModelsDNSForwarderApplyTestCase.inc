<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSForwarderApply;
use RESTAPI\Models\DNSForwarderHostOverride;
use RESTAPI\Models\DNSResolverApply;
use RESTAPI\Models\DNSResolverHostOverride;

class APIModelsDNSForwarderApplyTestCase extends TestCase {
    /**
     * Ensure the Model is correctly able to apply changes to the DNS Forwarder configuration and determine if there
     * are any pending changes.
     */
    public function test_apply(): void {
        # Make a change to the DNS Forwarder configuration and ensure we can see that there are pending changes
        $host_override = new DNSForwarderHostOverride();
        $host_override->replace_all(data: []);
        $dns_apply = new DNSForwarderApply(async: false);
        $this->assert_is_false($dns_apply->applied->value);

        # Apply the change and ensure that the applied status is now true
        $dns_apply->apply();
        $dns_apply = new DNSForwarderApply(async: false);
        $this->assert_is_true($dns_apply->applied->value);
    }
}
