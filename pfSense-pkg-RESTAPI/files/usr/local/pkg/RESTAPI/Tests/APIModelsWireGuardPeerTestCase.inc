<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\WireGuardPeer;
use RESTAPI\Models\WireGuardSettings;
use RESTAPI\Models\WireGuardTunnel;

class APIModelsWireGuardPeerTestCase extends TestCase {
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
     * Checks that the `presharedkey` field must be assigned a valid PSK if set.
     */
    public function test_validate_presharedkey(): void {
        # Ensure bad PSK throws an error
        $this->assert_throws_response(
            response_id: 'WIREGUARD_PEER_PRESHAREDKEY_INVALID',
            code: 400,
            callable: function () {
                $peer = new WireGuardPeer(
                    publickey: 'KG0BA4UyPilHH5qnXCfr6Lw8ynecOPor88tljLy3AHk=',
                    presharedkey: 'not a valid key!',
                );
                $peer->validate();
            },
        );

        # Ensure good PSK does not throw an error
        $this->assert_does_not_throw(
            callable: function () {
                $peer = new WireGuardPeer(
                    publickey: 'KG0BA4UyPilHH5qnXCfr6Lw8ynecOPor88tljLy3AHk=',
                    presharedkey: 'zppaKHpHl5tvjbwuYh8uTc8lfyzfLYg3IBrwn9yCFsc=',
                );
                $peer->validate();
            },
        );
    }

    /**
     * Checks that the WireGuard is correctly represented on the backend after creating, updating and deleting.
     */
    public function test_crud(): void {
        # Create a WireGuardTunnel object
        $tunnel = new WireGuardTunnel(privatekey: 'GNdQw+ujEIVgys4B2dDCXcBpiiQsNd2bAq5hnTp+smg=', async: false);
        $tunnel->create(apply: true);

        # Create a WireGuardPeer and ensure it is correctly shown using 'wg showconf'
        $peer = new WireGuardPeer(
            enabled: true,
            tun: $tunnel->name->value,
            persistentkeepalive: 200,
            publickey: 'KG0BA4UyPilHH5qnXCfr6Lw8ynecOPor88tljLy3AHk=',
            presharedkey: 'zppaKHpHl5tvjbwuYh8uTc8lfyzfLYg3IBrwn9yCFsc=',
            async: false,
        );
        $peer->create(apply: true);
        $wg_show = new Command('wg showconf ' . $peer->tun->value);
        $this->assert_str_contains($wg_show->output, 'PublicKey = ' . $peer->publickey->value);
        $this->assert_str_contains($wg_show->output, 'PresharedKey = ' . $peer->presharedkey->value);
        $this->assert_str_contains($wg_show->output, 'PersistentKeepalive = ' . $peer->persistentkeepalive->value);

        # Update the peer and ensure the updated values are shown in 'wg showconf'
        $peer->from_representation(
            persistentkeepalive: 80,
            publickey: 'KJoyFoK5PB5bua5ipRmzijghBSw37bffuuZ2wUN3/Dw=',
            presharedkey: '8O+HvB+YG8uPA9E/NJpuBUpGgHU1lHBses6Wzb37Ziw=',
            endpoint: '1.2.3.4',
            port: '12345',
        );
        $peer->update(apply: true);
        $wg_show = new Command('wg showconf ' . $peer->tun->value);
        $this->assert_str_contains($wg_show->output, 'PublicKey = ' . $peer->publickey->value);
        $this->assert_str_contains($wg_show->output, 'PresharedKey = ' . $peer->presharedkey->value);
        $this->assert_str_contains($wg_show->output, 'PersistentKeepalive = ' . $peer->persistentkeepalive->value);
        $this->assert_str_contains($wg_show->output, 'Endpoint = ' . $peer->endpoint->value . ':' . $peer->port->value);

        # Delete the peer and ensure it is no longer shown in 'wg showconf'
        $peer->delete(apply: true);
        $wg_show = new Command('wg showconf ' . $peer->tun->value);
        $this->assert_str_does_not_contain($wg_show->output, '[Peer]');
    }
}
