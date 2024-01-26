<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServer;
use RESTAPI\Models\DHCPServerAddressPool;
use RESTAPI\Models\DHCPServerBackend;
use RESTAPI\Models\DHCPServerStaticMapping;
use RESTAPI\Models\NetworkInterface;
use RESTAPI\Models\VirtualIP;

class APIModelsDHCPServerAddressPoolTestCase extends TestCase
{
    /**
     * Checks that the DHCPServerAddressPool cannot overlap with the DHCPServer's primary address pool.
     */
    public function test_address_pool_cannot_overlap_primary_address_pool(): void {
        # Define the parent DHCPServer's address pool
        $dhcp_server = new DHCPServer(id: "lan", range_from: "192.168.1.15", range_to: "192.168.1.30", async: false);
        $dhcp_server->update();

        # Ensure an error is thrown if we try to create a DHCPServerAddressPool that overlaps with the primary pool
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_OVERLAPS_EXISTING_OBJECT",
            code: 409,
            callable: function () use ($dhcp_server) {
                $pool = new DHCPServerAddressPool(
                    parent_id: $dhcp_server->id,
                    range_from: "192.168.1.29",
                    range_to: "192.168.1.35"
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that the DHCPServerAddressPool cannot overlap with another DHCPServerAddressPool.
     */
    public function test_address_pool_cannot_overlap_other_address_pool(): void {
        # Create a new DHCPServerAddress pool
        $pool = new DHCPServerAddressPool(
            parent_id: "lan",
            range_from: "192.168.1.200",
            range_to: "192.168.1.210",
            async: false
        );
        $pool->create();

        # Ensure an error is thrown if we try to create a DHCPServerAddressPool that overlaps with another address pool
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_OVERLAPS_EXISTING_OBJECT",
            code: 409,
            callable: function () use ($pool) {
                $new_pool = new DHCPServerAddressPool(
                    parent_id: $pool->parent_id,
                    range_from: "192.168.1.205",
                    range_to: "192.168.1.230"
                );
                $new_pool->validate();
            }
        );

        # Delete the address pool
        $pool->delete();
    }

    /**
     * Checks that the DHCPServerAddressPool cannot overlap with a DHCPServerStaticMapping's IP address.
     */
    public function test_address_pool_cannot_overlap_static_mapping(): void {
        # Create a new static mapping to test with
        $static_mapping = new DHCPServerStaticMapping(
            parent_id: "lan",
            mac: "11:22:33:44:55:66",
            ipaddr: "192.168.1.205",
            async: false
        );
        $static_mapping->create();

        # Ensure an error is thrown if we try to create a DHCPServerAddressPool that overlaps with the static mapping
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_OVERLAPS_EXISTING_OBJECT",
            code: 409,
            callable: function () use ($static_mapping) {
                $address_pool = new DHCPServerAddressPool(
                    parent_id: $static_mapping->parent_id,
                    range_from: "192.168.1.205",
                    range_to: "192.168.1.230"
                );
                $address_pool->validate();
            }
        );

        # Delete the static mapping
        $static_mapping->delete();
    }

    /**
     * Checks that the DHCPServerAddressPool cannot overlap with a VirtualIP.
     */
    public function test_address_pool_cannot_overlap_vip(): void {
        # Create a new virtual ip to test with
        $vip = new VirtualIP(
            interface: "lan",
            mode: "ipalias",
            subnet: "192.168.1.205",
            subnet_bits: 24,
            async: false
        );
        $vip->create();

        # Ensure an error is thrown if we try to create a DHCPServerAddressPool that overlaps with the static mapping
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_OVERLAPS_EXISTING_OBJECT",
            code: 409,
            callable: function () use ($vip) {
                $vip = new DHCPServerAddressPool(
                    parent_id: "lan",
                    range_from: "192.168.1.205",
                    range_to: "192.168.1.230"
                );
                $vip->validate();
            }
        );

        # Delete the virtual IP
        $vip->delete();
    }

    /**
     * Checks that the `range_from` field cannot be the interface's network IP
     */
    public function test_range_from_cannot_be_interface_network_address(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_RANGE_FROM_CANNOT_BE_NETWORK_ADDRESS",
            code: 400,
            callable: function () {
                $if = new NetworkInterface(id: "lan");
                $pool = new DHCPServerAddressPool(
                    parent_id: $if->id,
                    range_from: $if->get_network_ipv4(),
                    range_to: "192.168.1.200"
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that the `range_from` field cannot be an IP greater than `range_to`
     */
    public function test_range_from_cannot_be_greater_than_range_to(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_RANGE_FROM_CANNOT_BE_GREATER_THAN_RANGE_TO",
            code: 400,
            callable: function () {
                $pool = new DHCPServerAddressPool(
                    parent_id: "lan",
                    range_from: "192.168.1.201",
                    range_to: "192.168.1.200"
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that the `range_from` must be an IP address within the parent interface's subnet
     */
    public function test_range_from_must_be_ip_within_interfaces_subnet(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_RANGE_FROM_OUTSIDE_OF_SUBNET",
            code: 400,
            callable: function () {
                $pool = new DHCPServerAddressPool(
                    parent_id: "lan",
                    range_from: "1.1.1.1",
                    range_to: "1.1.1.2"
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that the `range_to` field cannot be the parent interface's broadcast address
     */
    public function test_range_to_cannot_be_interface_broadcast_address(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_RANGE_TO_CANNOT_BE_BROADCAST_ADDRESS",
            code: 400,
            callable: function () {
                $if = new NetworkInterface(id: "lan");
                $pool = new DHCPServerAddressPool(
                    parent_id: "lan",
                    range_from: "192.168.1.200",
                    range_to: $if->get_broadcast_ipv4()
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that the `range_to` must be an IP address within the parent interface's subnet
     */
    public function test_range_to_must_be_ip_within_interfaces_subnet(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_RANGE_TO_OUTSIDE_OF_SUBNET",
            code: 400,
            callable: function () {
                $pool = new DHCPServerAddressPool(
                    parent_id: "lan",
                    range_from: "192.168.1.200",
                    range_to: "192.168.2.200"
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that the `maxleasetime` field cannot be less than the `defaultleastime` field
     */
    public function test_maxleasetime_cannot_be_less_than_defaultleasetime(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_MAX_LEASE_TIME_LESS_THAN_DEFAULT",
            code: 400,
            callable: function () {
                $pool = new DHCPServerAddressPool(
                    parent_id: "lan",
                    range_from: "192.168.1.200",
                    range_to: "192.168.1.205",
                    defaultleasetime: 15000,
                    maxleasetime: 14000
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that the `gateway` field must be an IP within the parent interface's subnet.
     */
    public function test_gateway_must_be_ip_within_parent_interface(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_ADDRESS_POOL_GATEWAY_NOT_WITHIN_SUBNET",
            code: 400,
            callable: function () {
                $pool = new DHCPServerAddressPool(
                    parent_id: "lan",
                    range_from: "192.168.1.200",
                    range_to: "192.168.1.205",
                    gateway: "1.1.1.1"
                );
                $pool->validate();
            }
        );
    }

    /**
     * Checks that address pools are correctly represented in dhcpd.conf after creation, modification and deletion.
     */
    public function test_crud_dhcpd(): void {
        # Switch the DHCP server backend to ISC DHCP (dhcpd)
        $backend = new DHCPServerBackend(dhcpbackend: "isc", async: false);
        $backend->update();

        # Create an address pool to test with
        $pool = new DHCPServerAddressPool(
            parent_id: "lan",
            range_from: "192.168.1.200",
            range_to: "192.168.1.205",
            domain: "example.com",
            mac_allow: ["00:11:22:33:44:55"],
            mac_deny: ["55:44:33:22:11:00"],
            domainsearchlist: ["example.com"],
            defaultleasetime: 7201,
            maxleasetime: 86401,
            gateway: "192.168.1.2",
            dnsserver: ["127.0.0.53"],
            winsserver: ["127.0.0.43"],
            ntpserver: ["127.0.0.123"],
            ignorebootp: true,
            ignoreclientuids: true,
            denyunknown: "enabled",
            async: false
        );
        $pool->create();

        # Ensure the pool is found in dhcpd.conf
        $dhcpd_conf = file_get_contents("/var/dhcpd/etc/dhcpd.conf");
        $this->assert_str_contains($dhcpd_conf, "range 192.168.1.200 192.168.1.205;");
        $this->assert_str_contains($dhcpd_conf, 'allow members of "001122334455";');
        $this->assert_str_contains($dhcpd_conf, 'deny members of "554433221100"');
        $this->assert_str_contains($dhcpd_conf, 'deny unknown-clients;');
        $this->assert_str_contains($dhcpd_conf, 'option routers 192.168.1.2;');
        $this->assert_str_contains($dhcpd_conf, 'option domain-name "example.com";');
        $this->assert_str_contains($dhcpd_conf, 'option domain-search "example.com";');
        $this->assert_str_contains($dhcpd_conf, 'default-lease-time 7201;');
        $this->assert_str_contains($dhcpd_conf, 'max-lease-time 86401;');
        $this->assert_str_contains($dhcpd_conf, 'ignore bootp;');
        $this->assert_str_contains($dhcpd_conf, 'ignore-client-uids true;');
        $this->assert_str_contains($dhcpd_conf, 'option netbios-name-servers 127.0.0.43;');
        $this->assert_str_contains($dhcpd_conf, 'option netbios-node-type 8;');
        $this->assert_str_contains($dhcpd_conf, 'option ntp-servers 127.0.0.123;');
        $this->assert_str_contains($dhcpd_conf, 'option domain-name-servers 127.0.0.53;');

        # Update the pool with new values
        $pool->from_representation(
            range_from: "192.168.1.205",
            range_to: "192.168.1.210",
            domain: "new.example.com",
            mac_allow: ["55:44:33:22:11:00"],
            mac_deny: ["00:11:22:33:44:55"],
            domainsearchlist: ["new.example.com"],
            defaultleasetime: 7205,
            maxleasetime: 86405,
            gateway: "192.168.1.5",
            dnsserver: ["127.0.1.53"],
            winsserver: ["127.0.1.43"],
            ntpserver: ["127.0.1.123"],
            ignorebootp: false,
            ignoreclientuids: false,
            denyunknown: "class",
            async: false
        );
        $pool->update();

        # Ensure the updated pool is found in dhcpd.conf
        $dhcpd_conf = file_get_contents("/var/dhcpd/etc/dhcpd.conf");
        $this->assert_str_contains($dhcpd_conf, "range 192.168.1.205 192.168.1.210;");
        $this->assert_str_contains($dhcpd_conf, 'allow members of "554433221100";');
        $this->assert_str_contains($dhcpd_conf, 'deny members of "001122334455"');
        $this->assert_str_contains($dhcpd_conf, 'deny unknown-clients;');
        $this->assert_str_contains($dhcpd_conf, 'option routers 192.168.1.5;');
        $this->assert_str_contains($dhcpd_conf, 'option domain-name "new.example.com";');
        $this->assert_str_contains($dhcpd_conf, 'option domain-search "new.example.com";');
        $this->assert_str_contains($dhcpd_conf, 'default-lease-time 7205;');
        $this->assert_str_contains($dhcpd_conf, 'max-lease-time 86405;');
        $this->assert_str_does_not_contain($dhcpd_conf, 'ignore bootp;');
        $this->assert_str_does_not_contain($dhcpd_conf, 'ignore-client-uids true;');
        $this->assert_str_contains($dhcpd_conf, 'option netbios-name-servers 127.0.1.43;');
        $this->assert_str_contains($dhcpd_conf, 'option ntp-servers 127.0.1.123;');
        $this->assert_str_contains($dhcpd_conf, 'option domain-name-servers 127.0.1.53;');


        # Delete the pool and ensure it's no longer found in dhcpd.conf
        $pool->delete();
        $dhcpd_conf = file_get_contents("/var/dhcpd/etc/dhcpd.conf");
        $this->assert_str_does_not_contain($dhcpd_conf, "range 192.168.1.205 192.168.1.210;");

    }
}