<?php

namespace RESTAPI\Tests;

use RESTAPI\Auth\BasicAuth;
use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPRelay;
use RESTAPI\Models\DHCPServer;

class APIModelsDHCPRelayTestCase extends TestCase {
    /**
     * Disables all running DHCPServers.
     */
    public function disable_dhcp_servers(bool $apply = false): void {
        foreach (DHCPServer::query(enable: true)->model_objects as $dhcp_server) {
            $dhcp_server->enable->value = false;
            $dhcp_server->update();
        }
        if ($apply) {
            (new DHCPServer(async: false))->apply();
        }
    }

    /**
     * Checks that the DHCPRelay cannot be enabled if any DHCPServer is already enabled.
     */
    public function test_dhcp_relay_cannot_enable_if_dhcp_server_enabled(): void {
        # Disable all running DHCPServers
        $this->disable_dhcp_servers();

        # Ensure we can enable the DHCPRelay
        $this->assert_does_not_throw(
            callable: function () {
                $dhcp_relay = new DHCPRelay(enable: true, server: ['1.2.3.4']);
                $dhcp_relay->update();
            },
        );

        # Disable the DHCPRelay
        $dhcp_relay = new DHCPRelay(enable: false);
        $dhcp_relay->update();

        # Enable a DHCPServer
        $dhcp_server = new DHCPServer(id: 'lan', enable: true);
        $dhcp_server->update();

        # Ensure we cannot enable the DHCPRelay
        $this->assert_throws_response(
            response_id: 'DHCP_RELAY_CANNOT_BE_ENABLED_WHILE_DHCP_SERVER_IS_ENABLED',
            code: 409,
            callable: function () use ($dhcp_relay) {
                $dhcp_relay = new DHCPRelay(enable: true);
                $dhcp_relay->update();
            },
        );
    }

    /**
     * Checks that the to_internal() method correctly appends the '_vip' prefix to the 'carpstatusvip' field.
     */
    public function test_dhcp_relay_to_internal_carpstatusvip(): void {
        $dhcp_relay = new DHCPRelay(carpstatusvip: 'examplecarpvip');
        $this->assert_equals($dhcp_relay->to_internal()['carpstatusvip'], '_vipexamplecarpvip');
    }

    /**
     * Checks that the from_internal_carpstatusvip() method correctly removes the '_vip' prefix from the 'carpstatusvip'
     * field when the value is loaded from the internal data.
     */
    public function test_dhcp_relay_from_internal_carpstatusvip(): void {
        $dhcp_relay = new DHCPRelay();
        $this->assert_equals($dhcp_relay->from_internal_carpstatusvip('_vipexamplecarpvip'), 'examplecarpvip');
    }

    /**
     * Checks that the DHCP relay service is running after enabling and disabled after disabling the DHCPRelay.
     */
    public function test_dhcp_relay_service_running(): void {
        # Disable all running DHCPServers
        $this->disable_dhcp_servers(apply: true);

        # Ensure the DHCPRelay is enabled
        $dhcp_relay = new DHCPRelay(enable: true, interface: ["lan"], server: ['1.2.3.4']);
        $dhcp_relay->update();

        # Ensure the DHCP relay service is running with the correct arguments
        $dhcrelay_process = new Command('ps aux | grep dhcrelay');
        $lan_if = $this->env['PFREST_LAN_IF'];
        $wan_if = $this->env['PFREST_WAN_IF'];
        $this->assert_str_contains(
            $dhcrelay_process->output,
            "/usr/local/sbin/dhcrelay -id $lan_if -iu $wan_if 1.2.3.4",
        );

        # Disable the DHCPRelay
        $dhcp_relay = new DHCPRelay(enable: false);
        $dhcp_relay->update();

        # Ensure the DHCP relay service is not running
        $dhcrelay_pid = new Command('pgrep dhcrelay');
        $this->assert_equals($dhcrelay_pid->result_code, 1);

        # Re-enable the DHCPServer on LAN
        $dhcp_server = new DHCPServer(id: 'lan', enable: true);
        $dhcp_server->update(apply: true);
    }
}
