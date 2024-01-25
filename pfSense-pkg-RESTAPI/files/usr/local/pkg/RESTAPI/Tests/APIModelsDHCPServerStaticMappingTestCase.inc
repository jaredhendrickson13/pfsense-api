<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServer;
use RESTAPI\Models\DHCPServerAddressPool;
use RESTAPI\Models\DHCPServerStaticMapping;
use RESTAPI\Models\VirtualIP;

class APIModelsDHCPServerStaticMappingTestCase extends TestCase
{
    /**
     * Checks that the `ipaddr` field cannot be set to an IP address that is in use by an existing virtual IP.
     */
    public function test_ipaddr_cannot_overlap_vip(): void {
        # Create a virtual IP to test with
        $vip = new VirtualIP(interface: "lan", mode: "ipalias", subnet: "192.168.1.205", subnet_bits: 24);
        $vip->create();

        # Ensure an error is thrown if we try to create a DHCPServerStaticMapping with the IP assigned to the virtual IP
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_STATIC_MAPPING_IPADDR_OVERLAPS_EXISTING_OBJECT",
            code: 409,
            callable: function () use ($vip) {
                $static_mapping = new DHCPServerStaticMapping(
                    parent_id: "lan",
                    mac: "11:22:33:44:55:66",
                    ipaddr: $vip->subnet->value
                );
                $static_mapping->validate();
            }
        );

        # Delete the vip
        $vip->delete();
    }

    /**
     * Checks that the `ipaddr` field cannot be set to an IP address that is in use by an existing address pool.
     */
    public function test_ipaddr_cannot_overlap_address_pool(): void {
        # Create an address pool to test with
        $pool = new DHCPServerAddressPool(parent_id: "lan", range_from: "192.168.1.210", range_to: "192.168.1.215");
        $pool->create();

        # Ensure an error is thrown if we try to create a DHCPServerStaticMapping with the IP within the address pool
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_STATIC_MAPPING_IPADDR_OVERLAPS_EXISTING_OBJECT",
            code: 409,
            callable: function () use ($pool) {
                $static_mapping = new DHCPServerStaticMapping(
                    parent_id: "lan",
                    mac: "11:22:33:44:55:66",
                    ipaddr: $pool->range_from->value
                );
                $static_mapping->validate();
            }
        );

        # Delete the pool
        $pool->delete();
    }

    /**
     * Checks that the `ipaddr` field cannot be set to an IP address that overlaps the DHCP server's primary pool
     */
    public function test_ipaddr_cannot_overlap_primary_pool(): void {
        # Update the DHCP server primary pool
        $dhcp_server = new DHCPServer(id: "lan", range_from: "192.168.1.10", range_to: "192.168.1.15");
        $dhcp_server->update();

        # Ensure an error is thrown if we try to create a DHCPServerStaticMapping within the primary pool
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_STATIC_MAPPING_IPADDR_OVERLAPS_EXISTING_OBJECT",
            code: 409,
            callable: function () use ($dhcp_server) {
                $static_mapping = new DHCPServerStaticMapping(
                    parent_id: "lan",
                    mac: "11:22:33:44:55:66",
                    ipaddr: $dhcp_server->range_from->value
                );
                $static_mapping->validate();
            }
        );
    }

    /**
     * Checks that the `maxleasetime` field cannot be assigned a value less than `defaultleasetime`.
     */
    public function test_maxleasetime_cannot_be_less_than_defaultleasetime(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_STATIC_MAPPING_MAX_LEASE_TIME_LESS_THAN_DEFAULT",
            code: 400,
            callable: function () {
                $static_mapping = new DHCPServerStaticMapping(
                    parent_id: "lan",
                    mac: "11:22:33:44:55:66",
                    ipaddr: "192.168.1.205",
                    defaultleasetime: 3600,
                    maxleasetime: 3000
                );
                $static_mapping->validate();
            }
        );
    }

    /**
     * Checks that the `gateway` field must be assigned a value within the parent DHCP server's interface subnet.
     */
    public function test_gateway_must_be_within_interface_subnet(): void {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_STATIC_MAPPING_GATEWAY_NOT_WITHIN_SUBNET",
            code: 400,
            callable: function () {
                $static_mapping = new DHCPServerStaticMapping(
                    parent_id: "lan",
                    mac: "11:22:33:44:55:66",
                    ipaddr: "192.168.1.205",
                    gateway: "1.1.1.1"
                );
                $static_mapping->validate();
            }
        );
    }
}