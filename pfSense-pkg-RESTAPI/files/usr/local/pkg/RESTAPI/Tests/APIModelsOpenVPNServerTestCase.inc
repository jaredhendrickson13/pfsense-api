<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\NetworkInterface;
use RESTAPI\Models\OpenVPNServer;

class APIModelsOpenVPNServerTestCase extends TestCase {
    private string $ca_refid;
    private string $cert_refid;
    private string $tls_key;
    private array $base_data;

    /**
     * Setup the test environment before starting tests.
     */
    public function setup(): void {
        # Create a CA we can test with
        $ca = new CertificateAuthority(
            descr: 'test_ca',
            crt: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt'),
            prv: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key'),
        );
        $ca->create();
        $this->ca_refid = $ca->refid->value;

        # Create a new certificate we can test with
        $cert = new Certificate(
            descr: 'test_cert',
            crt: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt'),
            prv: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key'),
        );
        $cert->create();
        $this->cert_refid = $cert->refid->value;

        # Obtain an OpenVPN key to test with
        $this->tls_key = file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_openvpn_tls.key');

        # Set the base data we can use to quickly create a OpenVPNServer model object
        $this->base_data = [
            'mode' => 'p2p_tls',
            'dev_mode' => 'tun',
            'protocol' => 'UDP4',
            'interface' => 'wan',
            'tls' => $this->tls_key,
            'tls_type' => 'auth',
            'dh_length' => '2048',
            'ecdh_curve' => 'none',
            'data_ciphers' => ['AES-256-GCM'],
            'data_ciphers_fallback' => 'AES-256-GCM',
            'digest' => 'SHA256',
            'caref' => $this->ca_refid,
            'certref' => $this->cert_refid,
        ];
    }

    /**
     * Remove the CA and cert used for testing after tests complete.
     */
    public function teardown(): void {
        $ca = CertificateAuthority::query(refid: $this->ca_refid)->first();
        $ca->delete();
        $cert = Certificate::query(refid: $this->cert_refid)->first();
        $cert->delete();
    }

    /**
     * Checks that the sum of all `authmode` item's characters must be less than 254 characters
     */
    public function test_authmode_total_char_limit(): void {
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_AUTHMODE_CONTAINS_TOO_MANY_CHARS_FOR_AUTHSCRIPT',
            code: 400,
            callable: function () {
                $rand_string = substr(str_shuffle(str_repeat('abc123', 256)), 0, 256);
                $ovpns = new OpenVPNServer(
                    authmode: ['the', 'sum', 'of', 'these', 'items', 'contains', 'too', 'many', 'chars', $rand_string],
                );
                $ovpns->validate_extra();
            },
        );
    }

    /**
     * Checks that the sum of all `data_ciphers` chars must be less than 254
     */
    public function test_data_ciphers_total_char_limit(): void {
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_DATA_CIPHERS_CONTAINS_TOO_MANY_CHARS',
            code: 400,
            callable: function () {
                $rand_string = substr(str_shuffle(str_repeat('abc123', 256)), 0, 256);
                $ovpns = new OpenVPNServer(
                    data_ciphers: ['sum', 'of', 'these', 'items', 'contains', 'too', 'many', 'chars', $rand_string],
                );
                $ovpns->validate_extra();
            },
        );
    }

    /**
     * Checks that a `tunnel_network`, `tunnel_networkv6` or `serverbridge_interface` value is required when `dev_mode`
     * set to `tap`.
     */
    public function test_server_needs_tunnel_network_or_bridge_if_ip_with_tap_dev_mode(): void {
        # Construct an OpenVPNServer object to test with
        $ovpns = new OpenVPNServer(data: $this->base_data);

        # Ensure an error is thrown when tap mode is used without a tunnel network or bridge interface assigned
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_TAP_MODE_REQUIRES_TUNNEL_NETWORK_OR_BRIDGE_INTERFACE',
            code: 400,
            callable: function () use ($ovpns) {
                $ovpns->dev_mode->value = 'tap';
                $ovpns->tunnel_network->value = null;
                $ovpns->tunnel_networkv6->value = null;
                $ovpns->serverbridge_interface->value = null;
                $ovpns->validate();
            },
        );

        # Ensure no error is thrown if we have a tunnel network or bridge interface assigned
        $this->assert_does_not_throw(
            callable: function () use ($ovpns) {
                $ovpns->dev_mode->value = 'tap';
                $ovpns->tunnel_network->value = '1.2.3.4/24';
                $ovpns->tunnel_networkv6->value = null;
                $ovpns->serverbridge_interface->value = null;
                $ovpns->validate();

                $ovpns->dev_mode->value = 'tap';
                $ovpns->tunnel_network->value = null;
                $ovpns->tunnel_networkv6->value = '1234::1/24';
                $ovpns->serverbridge_interface->value = null;
                $ovpns->validate();

                $ovpns->dev_mode->value = 'tap';
                $ovpns->tunnel_network->value = null;
                $ovpns->tunnel_networkv6->value = null;
                $ovpns->serverbridge_dhcp->value = true;
                $ovpns->serverbridge_interface->value = 'lan';
                $ovpns->serverbridge_dhcp_start->value = '10.1.2.3';
                $ovpns->serverbridge_dhcp_end->value = '10.1.2.5';
                $ovpns->validate();
            },
        );
    }

    /**
     * Checks that a OpenVPNServer object cannot be disabled or deleted while it's in use.
     */
    public function test_cannot_disable_or_delete_while_in_use(): void {
        # Create a OpenVPNServer to test with
        $ovpns = new OpenVPNServer(data: $this->base_data);
        $ovpns->create();

        # Create a NetworkInterface that uses this OpenVPN server interface
        $if = new NetworkInterface(
            if: 'ovpns' . $ovpns->vpnid->value,
            descr: 'OVPNTEST',
            typev4: 'none',
            typev6: 'none',
        );
        $if->create();

        # Ensure an error is thrown if we try to disable or delete the OpenVPNServer while its assigned to an interface
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_CANNOT_BE_DISABLED_WHILE_IN_USE',
            code: 409,
            callable: function () use ($ovpns) {
                $ovpns->disable->value = true;
                $ovpns->validate();
            },
        );
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_CANNOT_BE_DELETED_WHILE_IN_USE',
            code: 409,
            callable: function () use ($ovpns) {
                $ovpns->delete();
            },
        );

        # Delete the interface assignment
        $if->delete();

        # Ensure the OpenVPNServer can be disabled and deleted while not assigned
        $this->assert_does_not_throw(
            callable: function () use ($ovpns) {
                $ovpns->disable->value = true;
                $ovpns->validate();

                $ovpns->delete();
            },
        );
    }

    /**
     * Checks that an IPv4 OpenVPNServer cannot be created on an interface without a IPv4 address.
     */
    public function test_ipv4_server_must_use_ipv4_interface(): void {
        # Create a new interface without an IPv4 address
        $if = new NetworkInterface(if: $this->env['PFREST_OPT1_IF'], descr: 'OVPNTEST', typev4: 'none', typev6: 'none');
        $if->create();

        # Ensure an IPv4 only OpenVPNServer cannot be created on the interface without an IPv4 address
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_INTERFACE_HAS_NO_IPV4_ASSIGNMENT',
            code: 400,
            callable: function () use ($if) {
                $ovpns = new OpenVPNServer(data: $this->base_data, protocol: 'UDP4', interface: $if->id);
                $ovpns->validate();
            },
        );

        # Delete the interface
        $if->delete();
    }

    /**
     * Checks that an IPv6 OpenVPNServer cannot be created on an interface without a IPv6 address.
     */
    public function test_ipv6_server_must_use_ipv6_interface(): void {
        # Create a new interface without an IPv6 address
        $if = new NetworkInterface(if: $this->env['PFREST_OPT1_IF'], descr: 'OVPNTEST', typev4: 'none', typev6: 'none');
        $if->create();

        # Ensure an IPv4 only OpenVPNServer cannot be created on the interface without an IPv4 address
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_INTERFACE_HAS_NO_IPV6_ASSIGNMENT',
            code: 400,
            callable: function () use ($if) {
                $ovpns = new OpenVPNServer(data: $this->base_data, protocol: 'UDP6', interface: $if->id);
                $ovpns->validate();
            },
        );

        # Delete the interface
        $if->delete();
    }

    /**
     * Checks that the `tls` field must contain a valid OpenVPN TLS key
     */
    public function test_validate_tls(): void {
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_TLS_INVALID_KEY',
            code: 400,
            callable: function () {
                $ovpns = new OpenVPNServer(data: $this->base_data);
                $ovpns->tls->value = 'not an openvpn tls key!';
                $ovpns->validate();
            },
        );
    }

    /**
     * Checks that the `tunnel_network` and `tunnel_networkv6` values are converted to network
     * address CIDRs during validation
     */
    public function test_tunnel_networks_convert_to_network_address_cidr(): void {
        $ovpns = new OpenVPNServer(
            data: $this->base_data,
            tunnel_network: '10.1.2.3/24',
            tunnel_networkv6: '1234::5/64',
        );
        $ovpns->validate();
        $this->assert_equals($ovpns->tunnel_network->value, '10.1.2.0/24');
        $this->assert_equals($ovpns->tunnel_networkv6->value, '1234::/64');
    }

    /**
     * Checks that the `serverbridge_dhcp` and `tunnel_network` values cannot be set at the same time.
     */
    public function test_no_serverbridge_dhcp_with_tunnel_network(): void {
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_BRIDGE_DHCP_CANNOT_BE_ENABLED_WITH_TUNNEL_NETWORK',
            code: 409,
            callable: function () {
                $ovpns = new OpenVPNServer(
                    data: $this->base_data,
                    dev_mode: 'tap',
                    serverbridge_dhcp: true,
                    serverbridge_interface: 'lan',
                    serverbridge_dhcp_start: '10.1.2.3',
                    serverbridge_dhcp_end: '10.1.2.5',
                    tunnel_network: '10.2.3.0/24',
                );
                $ovpns->validate();
            },
        );
    }

    /**
     * Checks that the `serverbridge_dhcp_start` value must precede the `serverbridge_dhcp_end` value.
     */
    public function test_serverbridge_dhcp_start_must_precede_serverbridge_dhcp_end(): void {
        $this->assert_throws_response(
            response_id: 'OPENVPN_SERVER_BRIDGE_DHCP_END_MUST_BE_IP_GREATER_THAN_DHCP_START',
            code: 400,
            callable: function () {
                $ovpns = new OpenVPNServer(
                    data: $this->base_data,
                    dev_mode: 'tap',
                    serverbridge_dhcp: true,
                    serverbridge_interface: 'lan',
                    serverbridge_dhcp_start: '10.1.2.5',
                    serverbridge_dhcp_end: '10.1.2.4',
                );
                $ovpns->validate();
            },
        );
    }

    /**
     * Checks that the get_next_vpn_id() method correctly obtains the next available OpenVPN ID
     */
    public function test_get_next_vpn_ip(): void {
        $ovpns = new OpenVPNServer();
        $this->assert_equals($ovpns->get_next_vpn_id(), 1);
    }

    /**
     * Checks that the get_dh_length_choices() correctly obtains the available `dh_length` choices on this system.
     */
    public function test_get_dh_length_choices(): void {
        $ovpns = new OpenVPNServer();
        $this->assert_is_not_empty($ovpns->get_dh_length_choices());
    }

    /**
     * Checks that the get_ecdh_curve_choices() correctly obtains the available `ecdh_curve` choices on this system.
     */
    public function test_get_ecdh_curve_choices(): void {
        $ovpns = new OpenVPNServer();
        $this->assert_is_not_empty($ovpns->get_ecdh_curve_choices());
    }

    /**
     * Checks that the get_available_cipher_choices() correctly obtains the available cipher choices on this system.
     */
    public function test_get_available_cipher_choices(): void {
        $ovpns = new OpenVPNServer();
        $this->assert_is_not_empty($ovpns->get_available_cipher_choices());
    }

    /**
     * Checks that the get_available_digest_choices() correctly obtains the available `digest` choices on this system.
     */
    public function get_available_digest_choices(): void {
        $ovpns = new OpenVPNServer();
        $this->assert_is_not_empty($ovpns->get_available_digest_choices());
    }

    /**
     * Checks that OpenVPN servers are correctly created, updated and deleted.
     */
    public function test_crud(): void {
        # Create a OpenVPN server
        $ovpns = new OpenVPNServer(data: $this->base_data);
        $ovpns->create();

        # Ensure the OpenVPN is correctly configured on the backend
        $ovpn_config = file_get_contents("/var/etc/openvpn/server{$ovpns->vpnid->value}/config.ovpn");
        $ifconfig = new Command("ifconfig ovpns{$ovpns->vpnid->value}");
        $this->assert_equals($ifconfig->result_code, 0);
        $this->assert_str_contains($ovpn_config, "dev-type {$ovpns->dev_mode->value}");
        $lc_proto = strtolower($ovpns->protocol->value);
        $this->assert_str_contains($ovpn_config, "proto $lc_proto");
        $if_ip = $ovpns->interface->get_related_model()->get_current_ipv4();
        $this->assert_str_contains($ovpn_config, "local $if_ip");
        $this->assert_str_contains($ovpn_config, "lport {$ovpns->local_port->value}");
        $this->assert_str_contains($ovpn_config, "auth {$ovpns->digest->value}");
        $this->assert_str_contains($ovpn_config, "dh /etc/dh-parameters.{$ovpns->dh_length->value}");
        $ciphers = implode(':', $ovpns->data_ciphers->value);
        $this->assert_str_contains($ovpn_config, "data-ciphers $ciphers");
        $this->assert_str_contains($ovpn_config, "data-ciphers-fallback {$ovpns->data_ciphers_fallback->value}");
        $this->assert_str_contains($ovpn_config, "allow-compression {$ovpns->allow_compression->value}");

        # Update the server with new values and ensure it is correctly reconfigured on the backend
        $ovpns->from_representation(
            dev_mode: 'tap',
            protocol: 'TCP4',
            interface: 'lan',
            local_port: '1195',
            dh_length: '8192',
            digest: 'SHA512',
            data_ciphers: ['AES-256-GCM', 'AES-128-GCM'],
            data_ciphers_fallback: 'CHACHA20-POLY1305',
            tunnel_network: '10.1.2.0/24',
            allow_compression: 'yes',
        );
        $ovpns->update();
        $ovpn_config = file_get_contents("/var/etc/openvpn/server{$ovpns->vpnid->value}/config.ovpn");
        $this->assert_str_contains($ovpn_config, "dev-type {$ovpns->dev_mode->value}");
        $lc_proto = strtolower($ovpns->protocol->value);
        $this->assert_str_contains($ovpn_config, "proto $lc_proto");
        $if_ip = $ovpns->interface->get_related_model()->get_current_ipv4();
        $this->assert_str_contains($ovpn_config, "local $if_ip");
        $this->assert_str_contains($ovpn_config, "lport {$ovpns->local_port->value}");
        $this->assert_str_contains($ovpn_config, "auth {$ovpns->digest->value}");
        $this->assert_str_contains($ovpn_config, "dh /etc/dh-parameters.{$ovpns->dh_length->value}");
        $ciphers = implode(':', $ovpns->data_ciphers->value) . ":{$ovpns->data_ciphers_fallback->value}";
        $this->assert_str_contains($ovpn_config, "data-ciphers $ciphers");
        $this->assert_str_contains($ovpn_config, "data-ciphers-fallback {$ovpns->data_ciphers_fallback->value}");
        $this->assert_str_contains($ovpn_config, "allow-compression {$ovpns->allow_compression->value}");

        # Delete the OpenVPNServer and ensure it is no longer found on the backend
        $ovpns->delete();
        $ifconfig = new Command("ifconfig ovpns{$ovpns->vpnid->value}");
        $this->assert_equals($ifconfig->result_code, 1);
    }

    /**
     * Checks that the `from_internal_vpnif()` method correctly obtains the VPN interface name.
     */
    public function test_from_internal_vpnif(): void {
        $ovpns = new OpenVPNServer();
        $ovpns->vpnid->value = $ovpns->get_next_vpn_id();
        $this->assert_equals($ovpns->from_internal_vpnif(), $ovpns::INTERFACE_PREFIX . $ovpns->vpnid->value);
    }

    /**
     * Checks that we can set the `authmode` field to 'Local Database' without an AuthServer created
     */
    public function test_authmode_local_database(): void {
        $this->assert_does_not_throw(
            callable: function () {
                $ovpns = new OpenVPNServer(data: $this->base_data);
                $ovpns->mode->value = 'server_user';
                $ovpns->authmode->value = ['Local Database'];
                $ovpns->validate();
            },
        );
    }

    /**
     * Checks that the 'generate_tls_key()' method correctly generates a new OpenVPN TLS key
     */
    public function test_generate_tls_key(): void {
        $this->assert_str_contains(
            OpenVPNServer::generate_tls_key(),
            '-----BEGIN OpenVPN Static key V1-----',
        );
    }
}
