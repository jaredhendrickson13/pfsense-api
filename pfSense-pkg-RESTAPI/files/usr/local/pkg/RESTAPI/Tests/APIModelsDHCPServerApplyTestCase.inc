<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServer;
use RESTAPI\Models\DHCPServerApply;

class APIModelsDHCPServerApplyTestCase extends TestCase {
    /**
     * Ensures that the Model correctly displays when the DHCP server has pending changes, and ensures it is cleared
     * once the changes are applied.
     */
    public function test_apply_dhcp_server_changes(): void {
        # Make a small change to the DHCP server configuration to ensure the subsystem is marked as dirty
        $dhcp_server = new DHCPServer(id: 'lan');
        $dhcp_server->update();

        # Ensure the DHCP server shows changes are pending
        $dhcp_server_apply = new DHCPServerApply(async: false);
        $this->assert_is_false($dhcp_server_apply->applied->value);

        # Apply the changes and ensure the DHCP server no longer shows changes are pending
        $dhcp_server_apply->create();
        $dhcp_server_apply = new DHCPServerApply(async: false);
        $this->assert_is_true($dhcp_server_apply->applied->value);
    }
}
