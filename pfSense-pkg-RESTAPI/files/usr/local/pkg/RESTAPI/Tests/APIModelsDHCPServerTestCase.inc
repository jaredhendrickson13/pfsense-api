<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\Model;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServer;
use RESTAPI\Models\DHCPServerStaticMapping;
use RESTAPI\Models\NetworkInterface;
use RESTAPI\Models\VirtualIP;

class APIModelsDHCPServerTestCase extends TestCase {
    /**
     * Ensures that a DHCP server cannot be enabled on an interface that does not have a static IPv4 address.
     */
    public function test_cannot_enable_dhcp_server_on_non_static_interface(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_CANNOT_ENABLE_WITHOUT_STATIC_IPV4',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);

                # Temporarily change the `lan` interface to use dhcp IPv4
                Model::set_config('interfaces/lan/ipaddr', 'dhcp');

                # Try to enable the DHCP server
                $dhcp_server->validate_enable(enable: true);
            },
        );
    }

    /**
     * Ensures that a DHCP server cannot be enabled on an interface is already running a DHCP relay.
     */
    public function test_cannot_enable_dhcp_server_with_relay_running(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_CANNOT_BE_ENABLED_WITH_DHCP_RELAY',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);

                # Temporarily change the `lan` interface to use dhcp IPv4
                Model::set_config('dhcrelay/enable', '');

                # Try to enable the DHCP server
                $dhcp_server->validate_enable(enable: true);
            },
        );
    }

    /**
     * Ensures that a DHCP server's `range_from` field cannot be the interface's network address.
     */
    public function test_range_from_cannot_network_address(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_RANGE_FROM_CANNOT_BE_NETWORK_ADDRESS',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $interface = new NetworkInterface(id: 'lan');

                # Try to enable the DHCP server
                $dhcp_server->validate_range_from(range_from: $interface->get_network_ipv4());
            },
        );
    }

    /**
     * Ensures that a DHCP server's `range_from` field cannot be greater than `range_to`
     */
    public function test_range_from_cannot_be_greater_than_range_to(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_RANGE_FROM_CANNOT_BE_GREATER_THAN_RANGE_TO',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->range_to->value = '192.168.1.1';
                $dhcp_server->validate_range_from(range_from: '192.168.1.2');
            },
        );
    }

    /**
     * Ensures that a DHCP server's `range_from` cannot be outside the interface's subnet
     */
    public function test_range_from_cannot_be_outside_interface_subnet(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_RANGE_FROM_OUTSIDE_OF_SUBNET',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->validate_range_from(range_from: '1.2.3.4');
            },
        );
    }

    /**
     * Ensures that a DHCP server's `range_to` field cannot be the interface's broadcast address.
     */
    public function test_range_to_cannot_broadcast_address(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_RANGE_FROM_CANNOT_BE_BROADCAST_ADDRESS',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $interface = new NetworkInterface(id: 'lan');

                # Try to enable the DHCP server
                $dhcp_server->validate_range_to(range_to: $interface->get_broadcast_ipv4());
            },
        );
    }

    /**
     * Ensures that a DHCP server's `range_to` cannot be outside the interface's subnet
     */
    public function test_range_to_cannot_be_outside_interface_subnet(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_RANGE_TO_OUTSIDE_OF_SUBNET',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->validate_range_to(range_to: '1.2.3.4');
            },
        );
    }

    /**
     * Ensures that a DHCP server's primary DHCP pool cannot conflict with an existing virtual IP.
     */
    public function test_primary_dhcp_pool_cannot_conflict_with_vip(): void {
        # Create a virtual IP to use for testing
        $vip = new VirtualIP(interface: 'lan', mode: 'ipalias', subnet: '192.168.1.2', subnet_bits: 24);
        $vip->create(apply: true);

        # Ensure an error is thrown when we try to change the DHCP server pool to conflict with the vip
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_PRIMARY_POOL_OVERLAPS_EXISTING_OBJECT',
            code: 409,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->range_from->value = '192.168.1.2';
                $dhcp_server->range_to->value = '192.168.1.3';
                $dhcp_server->validate_extra();
            },
        );

        # Delete the virtual IP
        $vip->delete(apply: true);
    }

    /**
     * Ensures that the primary DHCP pool cannot conflict with an existing DHCP static mapping.
     */
    public function test_dhcp_pool_cannot_conflict_with_static_mapping(): void {
        # Add a DHCP static mapping
        $static_map = new DHCPServerStaticMapping(
            parent_id: 'lan',
            mac: '11:22:33:44:55:66',
            ipaddr: '192.168.1.2',
            async: false,
        );
        $static_map->create(apply: true);

        # Ensure an error is thrown when we try to change the DHCP server pool to conflict with the static mapping
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_PRIMARY_POOL_OVERLAPS_EXISTING_OBJECT',
            code: 409,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->range_from->value = '192.168.1.2';
                $dhcp_server->range_to->value = '192.168.1.3';
                $dhcp_server->validate_extra();
            },
        );

        # Delete the static map
        $static_map->delete(apply: true);
    }

    /**
     * Ensures that the `maxleasetime` cannot be set to a value lower than the `defaultleasttime`.
     */
    public function test_maxleasetime_cannot_be_lower_than_defaultleasetime(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_MAX_LEASE_TIME_LESS_THAN_DEFAULT',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->defaultleasetime->value = 25000;
                $dhcp_server->validate_maxleasetime(maxleasetime: 24000);
            },
        );
    }

    /**
     * Ensures the DHCP server `gateway` value is a value within the interface's subnet.
     */
    public function test_dhcp_server_gateway_in_interface_subnet(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_GATEWAY_NOT_WITHIN_SUBNET',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->validate_gateway(gateway: '1.2.3.4');
            },
        );
    }

    /**
     * Ensures `nonak` cannot be enabled when a `failover_peerip` is set
     */
    public function test_no_nonak_with_failover_peerip(): void {
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_NONAK_WITH_FAILOVER_PEERIP_NOT_ALLOWED',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->failover_peerip->value = '192.168.1.2';
                $dhcp_server->validate_nonak(nonak: true);
            },
        );
    }

    /**
     * Ensures `staticarp` cannot be enabled if there are any static mappings without IPs.
     */
    public function test_no_staticarp_with_no_ip_static_mappings(): void {
        # Add a DHCP static mapping
        $static_mapping = new DHCPServerStaticMapping(parent_id: 'lan', mac: '00:00:00:00:00:00', async: false);
        $static_mapping->create(apply: true);

        # Ensure an error is thrown when we try to change the DHCP server pool to conflict with the static mapping
        $this->assert_throws_response(
            response_id: 'DHCP_SERVER_STATICARP_WITH_NO_IP_STATIC_MAPPINGS',
            code: 400,
            callable: function (): void {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: 'lan', async: false);
                $dhcp_server->validate_staticarp(staticarp: true);
            },
        );

        # Delete the static mapping
        $static_mapping->delete(apply: true);
    }

    /**
     * Ensures the DHCP server process is killed for the assigned interface when `enable` is set to `false` and
     * ensures the DHCP server process is respawned when it is re-enabled.
     */
    public function test_enable_controls_dhcpd_process(): void {
        # Disable the DHCP server and ensure the `dhcpd` process is no longer running
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->enable->value = false;
        $dhcp_server->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('ps aux'))->output,
            "-pf /var/run/dhcpd.pid {$this->env['PFREST_LAN_IF']}",
        );

        # Enable the DHCP server and ensure the `dhcpd` process is now running
        $dhcp_server->enable->value = true;
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('ps aux'))->output,
            "-pf /var/run/dhcpd.pid {$this->env['PFREST_LAN_IF']}",
        );
    }

    /**
     * Ensures the `ignorebootp` field correctly sets the dhcpd `ignore bootp` setting.
     */
    public function test_ignorebootp_set_dhcpd_ignore_bootp(): void {
        # Enable `ignorebootp` and ensure it is set in the dhcpd.conf
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->ignorebootp->value = true;
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'ignore bootp;');

        # Disable `ignorebootp` and ensure it is removed from the dhcpd.conf
        $dhcp_server->ignorebootp->value = false;
        $dhcp_server->update(apply: true);
        $this->assert_str_does_not_contain((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'ignore bootp;');
    }

    /**
     * Ensures the `denyunknown` field correctly sets the `deny` setting in dhcpd.conf
     */
    public function test_denyunknown_sets_dhcpd_deny(): void {
        # Set `denyunknown` to `enabled` and ensure dhcpd.conf denies all unknown clients
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->denyunknown->value = 'enabled';
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'deny unknown-clients;');
        $this->assert_str_does_not_contain(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'allow members of "s_lan";',
        );

        # Set `denyunknown` to `class` and ensure dhcpd.conf only allows clients known to this interface
        $dhcp_server->denyunknown->value = 'class';
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'allow members of "s_lan";');
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'deny unknown-clients;');

        # Unset `denyunknown` and ensure no 'deny' settings are configured in dhcpd.conf
        $dhcp_server->denyunknown->value = null;
        $dhcp_server->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'allow members of "s_lan";',
        );
        $this->assert_str_does_not_contain(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'deny unknown-clients;',
        );
    }

    /**
     * Ensures the `ignoreclientuids` field correctly sets the dhcpd `ignore-client-uid` setting.
     */
    public function test_ignoreclientuids_sets_dhcpd_ignore_client_uids(): void {
        # Enable `ignoreclientuids` and ensure it is set in the dhcpd.conf
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->ignoreclientuids->value = true;
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'ignore-client-uids true;');

        # Disable `ignoreclientuids` and ensure it is removed from the dhcpd.conf
        $dhcp_server->ignoreclientuids->value = false;
        $dhcp_server->update(apply: true);
        $this->assert_str_does_not_contain(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'ignore-client-uids true;',
        );
    }

    /**
     * Ensures the `disablepingcheck` field correctly sets the dhcpd `ping-check` setting.
     */
    public function test_disablepingcheck_sets_dhcpd_ping_check(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->disablepingcheck->value = true;
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'ping-check false;');

        $dhcp_server->disablepingcheck->value = false;
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'ping-check true;');
    }

    /**
     * Ensures the `range_from` and `range_to` field correctly configures the DHCP range in dhcpd.conf
     */
    public function test_range_from_and_range_to_sets_dhcpd_range(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->range_from->value = '192.168.1.105';
        $dhcp_server->range_to->value = '192.168.1.108';
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'range 192.168.1.105 192.168.1.108;',
        );
    }

    /**
     * Ensures the `winsserver` field correctly sets the WINS servers configured in dhcpd.conf
     */
    public function test_winsserver_sets_dhcpd_netbios_nameservers(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->winsserver->value = ['192.168.1.100', '192.168.1.101'];
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'option netbios-name-servers 192.168.1.100,192.168.1.101;',
        );
    }

    /**
     * Ensures the `dnsserver` field correctly sets the DNS servers configured in dhcpd.conf
     */
    public function test_dnsserver_sets_dhcpd_dns_servers(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->dnsserver->value = ['192.168.1.100', '192.168.1.101'];
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'option domain-name-servers 192.168.1.100,192.168.1.101;',
        );
    }

    /**
     * Ensures the `ntpserver` field correctly sets the NTP servers configured in dhcpd.conf
     */
    public function test_ntpserver_sets_dhcpd_ntp_servers(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->ntpserver->value = ['192.168.1.100', '192.168.1.101'];
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'option ntp-servers 192.168.1.100,192.168.1.101;',
        );
    }

    /**
     * Ensures the `mac_allow` field sets the proper dhcpd settings
     */
    public function test_mac_allow_sets_dhcpd_allow_members(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->mac_allow->value = ['11:22:33:aa:bb:cc'];
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'match if substring (hardware, 1, 6) = 11:22:33:aa:bb:cc;',
        );
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'allow members of "112233aabbcc";',
        );
    }

    /**
     * Ensures the `mac_deny` field sets the proper dhcpd settings
     */
    public function test_mac_deny_sets_dhcpd_deny_members(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->mac_deny->value = ['11:22:33:aa:bb:cc'];
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'match if substring (hardware, 1, 6) = 11:22:33:aa:bb:cc;',
        );
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'deny members of "112233aabbcc";',
        );
    }

    /**
     * Ensures the `gateway` field correctly sets the router configured in dhcpd.conf
     */
    public function test_gateway_sets_dhcpd_router(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->gateway->value = '192.168.1.2';
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'option routers 192.168.1.2;',
        );
    }

    /**
     * Ensures the `domain` field correctly sets the domain name configured in dhcpd.conf
     */
    public function test_domain_sets_dhcpd_domain_name(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->domain->value = 'example.com';
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'option domain-name "example.com";',
        );
    }

    /**
     * Ensures the `domainsearchlist` field correctly sets the domain search configured in dhcpd.conf
     */
    public function test_domainsearchlist_sets_dhcpd_domain_search(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->domainsearchlist->value = ['example.com'];
        $dhcp_server->update(apply: true);
        $this->assert_str_contains(
            (new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output,
            'option domain-search "example.com";',
        );
    }

    /**
     * Ensures the `defaultleasetime` field correctly sets the default-lease-time configured in dhcpd.conf
     */
    public function test_defaultleasetime_sets_dhcpd_default_lease_time(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->defaultleasetime->value = 7201;
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'default-lease-time 7201;');
    }

    /**
     * Ensures the `maxleasetime` field correctly sets the max-lease-time configured in dhcpd.conf
     */
    public function test_maxleasetime_sets_dhcpd_max_lease_time(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->maxleasetime->value = 86401;
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'max-lease-time 86401;');
    }

    /**
     * Ensures the `failover_peerip` field correctly sets the failover peer address configured in dhcpd.conf
     */
    public function test_failover_peerip_sets_dhcpd_peer_address(): void {
        $dhcp_server = new DHCPServer(id: 'lan', async: false);
        $dhcp_server->failover_peerip->value = '192.168.1.2';
        $dhcp_server->update(apply: true);
        $this->assert_str_contains((new Command('cat /var/dhcpd/etc/dhcpd.conf'))->output, 'peer address 192.168.1.2;');
    }

    /**
     * Ensures non-static interfaces are exempt from DHCP server initialization. This is a regression test for #781
     */
    public function test_non_static_interfaces_exempt_from_dhcp_server_initialization(): void {
        # Ensure no config entry exists for the `opt1` DHCP server
        Model::del_config('dhcpd/opt1');

        # Temporarily add a mock interface using pppoe for IPv4
        Model::set_config('interfaces/opt1/ipaddr', 'pppoe');

        # Init a DHCP server model. This should trigger initialization logic.
        new DHCPServer();

        # Ensure a config entry was not initialized for the non-static interface
        $this->assert_is_false(array_key_exists('opt1', Model::get_config('dhcpd')));
    }
}
