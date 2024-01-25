<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServerBackend;
use RESTAPI\Models\Service;

class APIModelsDHCPServerBackendTestCase extends TestCase
{
    /**
     * Checks that the backend DHCP server is successful changed after being updated.
     */
    public function test_switch_backend(): void {
        # Switch the backend to Kea DHCP
        $dhcp_backend = new DHCPServerBackend(dhcpbackend: "kea", async: false);
        $dhcp_backend->update();
        
        # Ensure ISCP DHCP (dhcpd) is no longer running and Kea DHCP (kea-dhcp4) is now running
        $this->assert_is_false(Service::query(name: "dhcpd")->exists());
        $this->assert_is_true(Service::query(name: "kea-dhcp4", status: true)->exists());
        
        # Switch the backend back to ISCP DHCP and ensure Kea is no longer running, but ISC DHCP is.
        $dhcp_backend->dhcpbackend->value = "isc";
        $dhcp_backend->update();
        $this->assert_is_false(Service::query(name: "kea-dhcp4")->exists());
        $this->assert_is_true(Service::query(name: "dhcpd", status: true)->exists());
    }
}
