<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServer;
use RESTAPI\Models\DHCPServerAddressPool;
use RESTAPI\Models\DHCPServerBackend;
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
        $pool = new DHCPServerAddressPool(
            parent_id: "lan",
            range_from: "192.168.1.210",
            range_to: "192.168.1.215",
            async: false
        );
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
    
    /**
     * Checks that static mappings are correctly represented in dhcpd.conf after creating, updating and deleting.
     */
    public function test_crud_dhcpd(): void {
        # Switch our DHCP backend to ISC DHCP
        $dhcp_backend = new DHCPServerBackend(dhcpbackend: "isc", async: false);
        $dhcp_backend->update();
        
        # Create a new static mapping to test with
        $static_mapping = new DHCPServerStaticMapping(
            parent_id: "lan",
            mac: "11:22:33:44:55:66",
            ipaddr: "192.168.1.215",
            cid: "test_cid",
            hostname: "test-hostname",
            domain: "example.com",
            domainsearchlist: ["example.com"],
            defaultleasetime: 7200,
            maxleasetime: 86400,
            gateway: "192.168.1.2",
            dnsserver: ["127.0.0.53"],
            winsserver: ["127.0.0.42"],
            ntpserver: ["127.0.0.123"],
            arp_table_static_entry: true,
            async: false
        );
        $static_mapping->create();
        
        # Ensure each field is correctly represented in dhcpd.conf
        $dhcp_conf = file_get_contents('/var/dhcpd/etc/dhcpd.conf');
        $this->assert_str_contains($dhcp_conf, "host s_{$static_mapping->parent_id}_{$static_mapping->id} {");
        $this->assert_str_contains($dhcp_conf, 'hardware ethernet 11:22:33:44:55:66;');
        $this->assert_str_contains($dhcp_conf, 'option dhcp-client-identifier "test_cid";');
        $this->assert_str_contains($dhcp_conf, 'fixed-address 192.168.1.215;');
        $this->assert_str_contains($dhcp_conf, 'option host-name "test-hostname";');
        $this->assert_str_contains($dhcp_conf, 'option routers 192.168.1.2;');
        $this->assert_str_contains($dhcp_conf, 'option domain-name "example.com";');
        $this->assert_str_contains($dhcp_conf, 'option domain-search "example.com";');
        $this->assert_str_contains($dhcp_conf, 'option domain-name-servers 127.0.0.53;');
        $this->assert_str_contains($dhcp_conf, 'default-lease-time 7200;');
        $this->assert_str_contains($dhcp_conf, 'max-lease-time 86400;');
        $this->assert_str_contains($dhcp_conf, 'option netbios-name-servers 127.0.0.42;');
        $this->assert_str_contains($dhcp_conf, 'option ntp-servers 127.0.0.123;');
        
        # Update the static mapping and ensure it's updated values are now represented in dhcpd.conf
        $static_mapping->from_representation(
            parent_id: "lan",
            mac: "66:55:44:33:22:11",
            ipaddr: "192.168.1.216",
            cid: "new_test_cid",
            hostname: "new-hostname",
            domain: "new.example.com",
            domainsearchlist: ["new.example.com"],
            defaultleasetime: 8000,
            maxleasetime: 90000,
            gateway: "192.168.1.5",
            dnsserver: ["127.0.1.53"],
            winsserver: ["127.0.1.42"],
            ntpserver: ["127.0.1.123"],
        );
        $static_mapping->update();
        
        # Ensure each field is correctly represented in dhcpd.conf
        $dhcp_conf = file_get_contents('/var/dhcpd/etc/dhcpd.conf');
        $this->assert_str_contains($dhcp_conf, 'hardware ethernet 66:55:44:33:22:11;');
        $this->assert_str_contains($dhcp_conf, 'option dhcp-client-identifier "new_test_cid";');
        $this->assert_str_contains($dhcp_conf, 'fixed-address 192.168.1.216;');
        $this->assert_str_contains($dhcp_conf, 'option host-name "new-hostname";');
        $this->assert_str_contains($dhcp_conf, 'option routers 192.168.1.5;');
        $this->assert_str_contains($dhcp_conf, 'option domain-name "new.example.com";');
        $this->assert_str_contains($dhcp_conf, 'option domain-search "new.example.com";');
        $this->assert_str_contains($dhcp_conf, 'option domain-name-servers 127.0.1.53;');
        $this->assert_str_contains($dhcp_conf, 'default-lease-time 8000;');
        $this->assert_str_contains($dhcp_conf, 'max-lease-time 90000;');
        $this->assert_str_contains($dhcp_conf, 'option netbios-name-servers 127.0.1.42;');
        $this->assert_str_contains($dhcp_conf, 'option ntp-servers 127.0.1.123;');
        
        # Delete the static mapping and ensure it no longer exists
        $static_mapping->delete();
        $dhcp_conf = file_get_contents('/var/dhcpd/etc/dhcpd.conf');
        $this->assert_str_does_not_contain($dhcp_conf, "host s_$static_mapping->parent_id\_$static_mapping->id {");
    }
    
    /**
     * Checks that static mappings are correctly represented in kea-dhcpv4.conf after creating, updating and deleting.
     */
    public function test_crud_kea(): void {
        # Switch our DHCP backend to Kea DHCP
        $dhcp_backend = new DHCPServerBackend(dhcpbackend: "kea", async: false);
        $dhcp_backend->update();

        # Create a new static mapping to test with
        $static_mapping = new DHCPServerStaticMapping(
            parent_id: "lan",
            mac: "11:22:33:44:55:66",
            ipaddr: "192.168.1.215",
            cid: "test_cid",
            hostname: "test-hostname",
            domain: "example.com",
            domainsearchlist: ["example.com"],
            defaultleasetime: 7200,
            maxleasetime: 86400,
            gateway: "192.168.1.2",
            dnsserver: ["127.0.0.53"],
            winsserver: ["127.0.0.42"],
            ntpserver: ["127.0.0.123"],
            arp_table_static_entry: true,
            async: false
        );
        $static_mapping->create();
        
        # Read the kea-dhcp4.conf and ensure each field is correctly represented in kea-dhcp4.conf reservations
        $kea_conf = json_decode(file_get_contents("/usr/local/etc/kea/kea-dhcp4.conf"), associative: true);
        $kea_reserv = $kea_conf["Dhcp4"]["subnet4"][0]["reservations"][0];
        $kea_options = $kea_reserv["option-data"];
        $this->assert_equals($kea_reserv["hw-address"], $static_mapping->mac->value);
        $this->assert_equals($kea_reserv["ip-address"], $static_mapping->ipaddr->value);
        $this->assert_equals($kea_reserv["hostname"], $static_mapping->hostname->value);
        $this->assert_equals($kea_reserv["hostname"], $static_mapping->hostname->value);
        $this->assert_equals(
            $this->get_kea_option_by_name("routers", $kea_options), 
            $static_mapping->gateway->value
        );
        $this->assert_equals(
            $this->get_kea_option_by_name("domain-name", $kea_options),
            $static_mapping->domain->value
        );
        $this->assert_equals(
            $this->get_kea_option_by_name("domain-search", $kea_options),
            $static_mapping->domainsearchlist->value[0]
        );
        $this->assert_equals(
            $this->get_kea_option_by_name("domain-name-servers", $kea_options),
            $static_mapping->dnsserver->value[0]
        );
        $this->assert_equals(
            $this->get_kea_option_by_name("netbios-name-servers", $kea_options),
            $static_mapping->winsserver->value[0]
        );
        $this->assert_equals(
            $this->get_kea_option_by_name("ntp-servers", $kea_options),
            $static_mapping->ntpserver->value[0]
        );
        
        # Delete the static mapping and ensure there are no more Kea reservations registered
        $static_mapping->delete();
        $kea_conf = json_decode(file_get_contents("/usr/local/etc/kea/kea-dhcp4.conf"), associative: true);
        $this->assert_is_empty($kea_conf["Dhcp4"]["subnet4"][0]["reservations"]);
    }
    
    /**
     * Obtains the 'data' value of a Kea reservations options-data item with a provided name.
     * @param string $name The name of the reservations option-data item to obtain
     * @param array $option_data The entire reservation's `option-data` array.
     * @return string The 'data' value of the $options_data item with the given $name
     */
    public function get_kea_option_by_name(string $name, array $option_data): string {
        foreach ($option_data as $option) {
            if ($option["name"] === $name) {
                return $option["data"];
            }
        }
        
        return "";
    }
}
