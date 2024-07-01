<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\VirtualIPApply;

class APIModelsVirtualIPApplyTestCase extends TestCase {
    /**
     * Ensure the Model is correctly able to apply changes to the IPsec configuration and determine if there
     * are any pending changes.
     */
    public function test_apply(): void {
        # Simulate serialized changes and ensure applied is seen as false
        file_put_contents('/tmp/.firewall_virtual_ip.apply', '');
        $vip_apply = new VirtualIPApply(async: false);
        $this->assert_is_false($vip_apply->applied->value);

        # Apply the change and ensure that the applied status is now true
        $vip_apply->apply();
        $vip_apply = new VirtualIPApply(async: false);
        $this->assert_is_true($vip_apply->applied->value);
    }
}
