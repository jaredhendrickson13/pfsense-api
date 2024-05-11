<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSResolverApply;
use RESTAPI\Models\DNSResolverHostOverride;
use RESTAPI\Models\Package;
use RESTAPI\Models\WireGuardApply;
use RESTAPI\Models\WireGuardPeer;
use RESTAPI\Models\WireGuardSettings;

class APIModelsWireGuardApplyTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-WireGuard'];

    /**
     * Ensures WireGuard is enabled before testing
     */
    public function setup(): void {
        $wg = new WireGuardSettings(enable: true, async: false);
        $wg->update(apply: true);
    }

    /**
     * Ensure the Model is correctly able to apply changes to the WireGuard configuration and determine if there
     * are any pending changes.
     */
    public function test_apply(): void {
        # Make a change to the WireGuard configuration and ensure we can see that there are pending changes
        $peers = new WireGuardPeer();
        $peers->replace_all(data: []);
        $wg_apply = new WireGuardApply(async: false);
        $this->assert_is_false($wg_apply->applied->value);

        # Apply the change and ensure that the applied status is now true
        $wg_apply->apply();
        $wg_apply = new WireGuardApply(async: true);
        $this->assert_is_true($wg_apply->applied->value);
    }
}
