<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\Package;
use RESTAPI\Models\WireGuardSettings;
use RESTAPI\Models\WireGuardTunnel;
use RESTAPI\Models\WireGuardTunnelAddress;

class APIModelsWireGuardTunnelAddressTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-WireGuard'];

    /**
     * Ensures WireGuard is enabled before testing
     */
    public function setup(): void {
        # Only enable WireGuard if it's not already enabled
        $wg = new WireGuardSettings(async: false);
        if (!$wg->enable->value) {
            $wg->enable->value = true;
            $wg->update(apply: true);
        }
    }

    /**
     * Checks that a WireGuardTunnelAddress cannot be created with an address and mask that conflict with an existing
     * address assignment on the system.
     */
    public function test_address_and_mask_cannot_conflict_with_existing_address(): void {
        # Create a new tunnel
        $tunnel = new WireGuardTunnel(privatekey: 'KG0BA4UyPilHH5qnXCfr6Lw8ynecOPor88tljLy3AHk=', listenport: '55000');
        $tunnel->create();

        # Assign an address to the tunnel that conflicts with the LAN's subnet and ensure it throws an error
        $this->assert_throws_response(
            response_id: 'WIREGUARD_TUNNEL_ADDRESS_AND_MASK_ALREADY_IN_USE',
            code: 409,
            callable: function () use ($tunnel) {
                $address = new WireGuardTunnelAddress(parent_id: $tunnel->id, address: '192.168.1.1', mask: 24);
                $address->validate();
            },
        );

        # Delete the tunnel
        $tunnel->delete();
    }

    /**
     * Checks that the WireGuardTunnelAddress is correctly represented on the backend after creating,
     * updating and deleting.
     */
    public function test_crud(): void {
        # Create a new tunnel
        $tunnel = new WireGuardTunnel(privatekey: 'KG0BA4UyPilHH5qnXCfr6Lw8ynecOPor88tljLy3AHk=', async: false);
        $tunnel->create();

        # Create a new address and ensure it's found on the backend
        $address = new WireGuardTunnelAddress(parent_id: $tunnel->id, address: '10.168.1.1', mask: 24, async: false);
        $address->create(apply: true);
        $ifconfig = new Command('ifconfig ' . $address->parent_model->name->value);
        $this->assert_str_contains($ifconfig->output, 'inet 10.168.1.1 netmask 0xffffff00');

        # Update the address with new values and ensure it's found on the backend
        $address->from_representation(address: '10.170.1.1', mask: 32);
        $address->update(apply: true);
        $ifconfig = new Command('ifconfig ' . $address->parent_model->name->value);
        $this->assert_str_contains($ifconfig->output, 'inet 10.170.1.1 netmask 0xffffffff');

        # Delete the address and ensure it's no longer found on the backend
        $address->delete(apply: true);
        $ifconfig = new Command('ifconfig ' . $address->parent_model->name->value);
        $this->assert_str_does_not_contain($ifconfig->output, 'inet 10.170.1.1 netmask 0xffffffff');

        # Delete the tunnel
        $tunnel->delete(apply: true);
    }
}
