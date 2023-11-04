<?php

namespace API\Tests;

use API\Core\Model;
use API\Core\TestCase;
use API\Models\DHCPServer;
use API\Models\NetworkInterface;
use API\Models\VirtualIP;

class APIModelsDHCPServerTestCase extends TestCase
{
    /**
     * Ensures that a DHCP server cannot be enabled on an interface that does not have a static IPv4 address.
     */
    public function test_cannot_enable_dhcp_server_on_non_static_interface() {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CANNOT_ENABLE_WITHOUT_STATIC_IPV4",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");
                
                # Temporarily change the `lan` interface to use dhcp IPv4
                Model::set_config("interfaces/lan/ipaddr", "dhcp");
                
                # Try to enable the DHCP server
                $dhcp_server->validate_enable(enable: true);
            }
        );
    }

    /**
     * Ensures that a DHCP server cannot be enabled on an interface is already running a DHCP relay.
     */
    public function test_cannot_enable_dhcp_server_with_relay_running() {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CANNOT_BE_ENABLED_WITH_DHCP_RELAY",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");

                # Temporarily change the `lan` interface to use dhcp IPv4
                Model::set_config("dhcrelay/0/interface", "lan");

                # Try to enable the DHCP server
                $dhcp_server->validate_enable(enable: true);
            }
        );
    }

    /**
     * Ensures that a DHCP server's `range_from` field cannot be the interface's network address.
     */
    public function test_range_from_cannot_network_address() {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_RANGE_FROM_CANNOT_BE_NETWORK_ADDRESS",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");
                $interface = new NetworkInterface(id: "lan");

                # Try to enable the DHCP server
                $dhcp_server->validate_range_from(range_from: $interface->get_network_ipv4());
            }
        );
    }

    /**
     * Ensures that a DHCP server's `range_from` field cannot be greater than `range_to`
     */
    public function test_range_from_cannot_be_greater_than_range_to() {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_RANGE_FROM_CANNOT_BE_GREATER_THAN_RANGE_TO",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");
                $dhcp_server->range_to->value = "192.168.1.1";
                $dhcp_server->validate_range_from(range_from: "192.168.1.2");
            }
        );
    }

    /**
     * Ensures that a DHCP server's `range_from` cannot be outside the interface's subnet
     */
    public function test_range_from_cannot_be_outside_interface_subnet() {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_RANGE_FROM_OUTSIDE_OF_SUBNET",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");
                $dhcp_server->validate_range_from(range_from: "1.2.3.4");
            }
        );
    }

    /**
     * Ensures that a DHCP server's `range_to` field cannot be the interface's broadcast address.
     */
    public function test_range_to_cannot_broadcast_address() {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_RANGE_FROM_CANNOT_BE_BROADCAST_ADDRESS",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");
                $interface = new NetworkInterface(id: "lan");

                # Try to enable the DHCP server
                $dhcp_server->validate_range_to(range_to: $interface->get_broadcast_ipv4());
            }
        );
    }

    /**
     * Ensures that a DHCP server's `range_to` cannot be outside the interface's subnet
     */
    public function test_range_to_cannot_be_outside_interface_subnet() {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_RANGE_TO_OUTSIDE_OF_SUBNET",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");
                $dhcp_server->validate_range_to(range_to: "1.2.3.4");
            }
        );
    }

    /**
     * Ensures that a DHCP server's primary DHCP pool cannot conflict with an existing virtual IP.
     */
    public function test_primary_dhcp_pool_cannot_conflict_with_vip() {
        # Create a virtual IP to use for testing
        $vip = new VirtualIP(
            representation_data: [
                "interface" => "lan", 
                "mode" => "ipalias",
                "subnet" => "192.168.1.2", 
                "subnet_bits" => 24
            ]
        );
        $vip->create();
        
        # Ensure an error is thrown when we try to changed the DHCP server pool to conflict with the vip
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_POOL_OVERLAPS_VIP",
            code: 400,
            callable: function() {
                # Use the `lan` DHCP server for this test
                $dhcp_server = new DHCPServer(id: "lan");
                $dhcp_server->range_from->value = "192.168.1.2";
                $dhcp_server->validate_range_to(range_to: "192.168.1.3");
            }
        );
        
        # Delete the virtual IP
        $vip->delete();
    }
}
