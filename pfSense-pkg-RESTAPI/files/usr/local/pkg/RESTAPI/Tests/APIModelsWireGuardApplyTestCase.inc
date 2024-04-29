<?php

namespace RESTAPI\Tests;


use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSResolverApply;
use RESTAPI\Models\DNSResolverHostOverride;
use RESTAPI\Models\Package;
use RESTAPI\Models\WireGuardPeer;
use RESTAPI\Models\WireGuardSettings;

class APIModelsWireGuardApplyTestCase extends TestCase
{
    /**
     * Ensure the WireGuard package is installed and enabled before running tests.
     */
    public function setup(): void {
        # Install the package
        $pkg = new Package(name: 'pfSense-pkg-WireGuard');
        $pkg->create();

        # Enable WireGuard
        $wg_settings = new WireGuardSettings(enable: true, async: false);
        $wg_settings->update(apply: true);
    }

    /**
     * Ensure the WireGuard package is removed after running tests
     */
    public function teardown(): void {
        $pkg = Package::query(name: 'pfSense-pkg-WireGuard')->first();
        $pkg->delete();
    }
    
    /**
     * Ensure the Model is correctly able to apply changes to the WireGuard configuration and determine if there
     * are any pending changes.
     */
    public function test_apply(): void {

        # Make a change to the WireGuard configuration and ensure we can see that there are pending changes
        $peers = new WireGuardPeer();
        $peers->replace_all(data: []);
        $wg_apply = new DNSResolverApply(async: false);
        $this->assert_is_false($wg_apply->applied->value);
        
        # Apply the change and ensure that the applied status is now true
        $wg_apply->apply();
        $wg_apply = new DNSResolverApply(async: false);
        $this->assert_is_true($wg_apply->applied->value);
    }
}
