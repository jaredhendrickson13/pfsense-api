<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Models\Service;
use RESTAPI\Models\WireGuardSettings;

class APIModelsWireGuardSettingsTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-WireGuard'];

    /**
     * Checks that the `enable` field correctly enables the WireGuard service when `true` and stops the service
     * when `false`
     */
    public function test_enable_controls_service(): void {
        # Enable WireGuard and ensure the service is running
        $wg = new WireGuardSettings(enable: true, async: false);
        $wg->update(apply: true);
        $this->assert_is_true(Service::query(name: 'wireguard', status: true)->exists());

        # Disable WireGuard and ensure the service is stopped
        $wg->enable->value = false;
        $wg->update(apply: true);
        $this->assert_is_true(Service::query(name: 'wireguard', status: false)->exists());
    }

    /**
     * Checks that WireGuard cannot be disabled when one or more pfSense interfaces are assigned a WireGuard tunnel
     * interface.
     */
    public function test_cannot_disable_while_tunnels_in_use_by_if(): void {
        # Define a WireGuardSettings object to test with
        $wg = new WireGuardSettings(async: false);

        # Ensure WireGuard can be enabled and disabled while tunnels are not assigned to interfaces
        $this->assert_does_not_throw(
            callable: function () use ($wg) {
                $wg->enable->value = true;
                $wg->update(apply: true);
                $wg->enable->value = false;
                $wg->update(apply: true);
            },
        );

        # Enable WireGuard
        $wg->enable->value = true;
        $wg->update(apply: true);

        # Mock an interface assigned a WireGuard tunnel
        # TODO: Replace this with a WireGuardTunnel model call when developed
        $wg->set_config('interfaces/opt1', ['if' => 'tun_wg0']);

        # Ensure WireGuard cannot be disabled while a tunnel is assigned to a pfSense interface
        $this->assert_throws_response(
            response_id: 'WIREGUARD_SETTINGS_CANNOT_DISABLE_WITH_ACTIVE_TUNNELS',
            code: 409,
            callable: function () use ($wg) {
                # Try to disable WireGuard and ensure an error is thrown
                $wg->enable->value = false;
                $wg->validate();
            },
        );

        # Unset the interface assignment and disable WireGuard
        $wg->del_config('interfaces/opt1');
        $wg->enable->value = false;
        $wg->update(apply: true);
    }
}
