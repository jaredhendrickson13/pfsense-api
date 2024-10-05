<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\BINDSyncRemoteHost;

class APIModelsBINDSyncRemoteHostTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-bind'];

    /**
     * Checks that we can update the BIND sync remote host settings.
     */
    public function test_crud(): void {
        # Create a new remote host
        $remote_host = new BINDSyncRemoteHost(
            syncdestinenable: false,
            syncprotocol: 'https',
            ipaddress: '127.0.0.1',
            syncport: '443',
            username: 'admin',
            password: 'pfsense',
        );
        $remote_host->create();

        # Update the remote host
        $remote_host->syncdestinenable->value = false;
        $remote_host->syncprotocol->value = 'http';
        $remote_host->ipaddress->value = '127.0.0.2';
        $remote_host->syncport->value = '80';
        $remote_host->username->value = 'admin2';
        $remote_host->password->value = 'pfsense2';
        $remote_host->update();

        # Delete the remote host
        $remote_host->delete();
    }
}
