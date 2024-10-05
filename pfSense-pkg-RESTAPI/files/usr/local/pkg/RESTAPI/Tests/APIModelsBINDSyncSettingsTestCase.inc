<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDSyncSettings;

class APIModelsBINDSyncSettingsTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-bind'];

    /**
     * Checks that we can update the BIND sync settings.
     */
    public function test_bind_sync_update(): void {
        $this->assert_does_not_throw(
            callable: function () {
                $sync = new BINDSyncSettings(synconchanges: 'manual', synctimeout: 60, masterip: '1.2.3.4');
                $sync->update();
            },
        );
    }
}
