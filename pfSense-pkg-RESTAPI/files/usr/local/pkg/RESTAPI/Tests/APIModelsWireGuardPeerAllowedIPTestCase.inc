<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\Package;
use RESTAPI\Models\WireGuardPeer;
use RESTAPI\Models\WireGuardPeerAllowedIP;
use RESTAPI\Models\WireGuardSettings;
use RESTAPI\Models\WireGuardTunnel;

class APIModelsWireGuardPeerAllowedIPTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-WireGuard'];

    /**
     * Ensures WireGuard is enabled before testing
     */
    public function setup(): void {
        $wg = new WireGuardSettings(enable: true, async: false);
        $wg->update(apply: true);
    }

    /**
     * Checks that the WireGuard is correctly represented on the backend after creating, updating and deleting.
     */
    public function test_crud(): void {
        # Create WireGuardTunnel and WireGuardPeer object
        $tunnel = new WireGuardTunnel(privatekey: 'GNdQw+ujEIVgys4B2dDCXcBpiiQsNd2bAq5hnTp+smg=', async: false);
        $tunnel->create(apply: true);
        $peer = new WireGuardPeer(
            enabled: true,
            tun: $tunnel->name->value,
            persistentkeepalive: 200,
            publickey: 'KG0BA4UyPilHH5qnXCfr6Lw8ynecOPor88tljLy3AHk=',
            presharedkey: 'zppaKHpHl5tvjbwuYh8uTc8lfyzfLYg3IBrwn9yCFsc=',
            async: false,
        );
        $peer->create(apply: true);

        # Create a WireGuardPeerAllowedIP and ensure it is shown in 'wg showconf'
        $allowed_ip = new WireGuardPeerAllowedIP(parent_id: $peer->id, address: '1.2.3.0', mask: 24, async: false);
        $allowed_ip->create(apply: true);
        $wg_show = new Command('wg showconf ' . $peer->tun->value);
        $this->assert_str_contains($wg_show->output, 'AllowedIPs = 1.2.3.0/24');

        # Update the allowed IP and ensure the updated value is shown in 'wg showconf'
        $allowed_ip->from_representation(address: '3.2.1.0', mask: 25);
        $allowed_ip->update(apply: true);
        $wg_show = new Command('wg showconf ' . $peer->tun->value);
        $this->assert_str_contains($wg_show->output, 'AllowedIPs = 3.2.1.0/25');

        # Delete the allowed IP and ensure it is no longer present in 'wg showconf'
        $allowed_ip->delete(apply: true);
        $wg_show = new Command('wg showconf ' . $peer->tun->value);
        $this->assert_str_does_not_contain($wg_show->output, 'AllowedIPs');
    }
}
