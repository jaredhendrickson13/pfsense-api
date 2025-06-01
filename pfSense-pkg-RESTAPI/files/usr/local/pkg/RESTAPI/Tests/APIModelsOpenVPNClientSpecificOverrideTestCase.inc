<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\OpenVPNClientSpecificOverride;
use RESTAPI\Models\OpenVPNServer;

class APIModelsOpenVPNClientSpecificOverrideTestCase extends TestCase {
    private CertificateAuthority $ca;
    private Certificate $cert;
    private OpenVPNServer $ovpns;

    /**
     * Setup the test environment before starting tests.
     */
    public function setup(): void {
        # Create a CA we can test with
        $this->ca = new CertificateAuthority(
            descr: 'test_ca',
            crt: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt'),
            prv: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key'),
        );
        $this->ca->create();

        # Create a new certificate we can test with
        $this->cert = new Certificate(
            descr: 'test_cert',
            crt: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt'),
            prv: file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key'),
        );
        $this->cert->create();

        # Obtain an OpenVPN key to test with
        $tls_key = file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_openvpn_tls.key');

        # Set the base data we can use to quickly create a OpenVPNServer model object
        $this->ovpns = new OpenVPNServer(
            data: [
                'mode' => 'p2p_tls',
                'dev_mode' => 'tun',
                'protocol' => 'UDP4',
                'interface' => 'wan',
                'tls' => $tls_key,
                'tls_type' => 'auth',
                'dh_length' => '2048',
                'ecdh_curve' => 'none',
                'data_ciphers' => ['AES-256-GCM'],
                'data_ciphers_fallback' => 'AES-256-GCM',
                'digest' => 'SHA256',
                'caref' => $this->ca->refid->value,
                'certref' => $this->cert->refid->value,
            ],
        );
        $this->ovpns->create();
    }

    /**
     * Remove the OpenVPN server, CA and cert used for testing after tests complete.
     */
    public function teardown(): void {
        $this->ovpns->delete();
        $this->ca->delete();
        $this->cert->delete();
    }

    /**
     * Checks that the OpenVPN client specific override is correctly represented on the backend after creation,
     * update and deletion.
     */
    public function test_crud(): void {
        # Create a new CSO and ensure it is correctly represented on the backend
        $cso = new OpenVPNClientSpecificOverride(
            common_name: 'test.example.com',
            disable: false,
            block: true,
            server_list: [$this->ovpns->vpnif->value],
            tunnel_network: '10.1.2.0/24',
            tunnel_networkv6: '1234::/64',
            local_network: ['10.1.2.0/24'],
            local_networkv6: ['1234::/64'],
            remote_network: ['10.1.2.0/24'],
            remote_networkv6: ['1234::/64'],
            gwredir: true,
            push_reset: false,
            remove_options: ['remove_route'],
            dns_domain: 'example.com',
            dns_server1: '127.0.0.1',
            dns_server2: '127.0.0.2',
            dns_server3: '127.0.0.3',
            dns_server4: '127.0.0.4',
            ntp_server1: '127.0.0.1',
            ntp_server2: '127.0.0.2',
            netbios_enable: true,
            netbios_ntype: 1,
            netbios_scope: 'test_scope',
            wins_server1: '127.0.0.1',
            wins_server2: '127.0.0.2',
            custom_options: ['test_custom_option example1', 'test_custom_option example2'],
        );
        $cso->create();
        $cso_conf = file_get_contents(
            "/var/etc/openvpn/server{$this->ovpns->vpnid->value}/csc/{$cso->common_name->value}",
        );
        $this->assert_str_contains($cso_conf, 'disable'); // For `block` field
        $this->assert_str_does_not_contain($cso_conf, 'push-reset');
        $this->assert_str_contains($cso_conf, 'push-remove route');
        $this->assert_str_contains($cso_conf, 'push "route 10.1.2.0 255.255.255.0"');
        $this->assert_str_contains($cso_conf, 'push "route-ipv6 1234::/64"');
        $this->assert_str_contains($cso_conf, 'iroute 10.1.2.0 255.255.255.0');
        $this->assert_str_contains($cso_conf, 'iroute-ipv6 1234::/64');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DOMAIN example.com"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.0.1"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.0.2"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.0.3"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.0.4"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option NTP 127.0.0.1"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option NTP 127.0.0.2"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option WINS 127.0.0.1"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option WINS 127.0.0.2"');
        $this->assert_str_contains($cso_conf, 'push "redirect-gateway def1"');
        $this->assert_str_contains($cso_conf, 'test_custom_option example1');
        $this->assert_str_contains($cso_conf, 'test_custom_option example2');

        # Update the CSO and ensure the new values are represented on the backend
        $cso->from_representation(
            common_name: 'updatedtest.example.com',
            disable: false,
            block: false,
            server_list: [$this->ovpns->vpnif->value],
            tunnel_network: '10.2.3.0/24',
            tunnel_networkv6: '4321::/64',
            local_network: ['10.2.3.0/24'],
            local_networkv6: ['4321::/64'],
            remote_network: ['10.2.3.0/24'],
            remote_networkv6: ['4321::/64'],
            gwredir: false,
            push_reset: true,
            remove_options: [],
            dns_domain: 'updated.example.com',
            dns_server1: '127.0.1.1',
            dns_server2: '127.0.1.2',
            dns_server3: '127.0.1.3',
            dns_server4: '127.0.1.4',
            ntp_server1: '127.0.1.1',
            ntp_server2: '127.0.1.2',
            netbios_enable: true,
            netbios_ntype: 2,
            netbios_scope: 'updated_test_scope',
            wins_server1: '127.0.1.1',
            wins_server2: '127.0.1.2',
            custom_options: ['updated_test_custom_option example1', 'updated_test_custom_option example2'],
        );
        $cso->update();
        $cso_conf = file_get_contents(
            "/var/etc/openvpn/server{$this->ovpns->vpnid->value}/csc/{$cso->common_name->value}",
        );
        $this->assert_str_does_not_contain($cso_conf, 'disable'); // For `block` field
        $this->assert_str_contains($cso_conf, 'push-reset');
        $this->assert_str_does_not_contain($cso_conf, 'push-remove route');
        $this->assert_str_contains($cso_conf, 'push "route 10.2.3.0 255.255.255.0"');
        $this->assert_str_contains($cso_conf, 'push "route-ipv6 4321::/64"');
        $this->assert_str_contains($cso_conf, 'iroute 10.2.3.0 255.255.255.0');
        $this->assert_str_contains($cso_conf, 'iroute-ipv6 4321::/64');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DOMAIN updated.example.com"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.1.1"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.1.2"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.1.3"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option DNS 127.0.1.4"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option NTP 127.0.1.1"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option NTP 127.0.1.2"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option WINS 127.0.1.1"');
        $this->assert_str_contains($cso_conf, 'push "dhcp-option WINS 127.0.1.2"');
        $this->assert_str_does_not_contain($cso_conf, 'push "redirect-gateway def1"');
        $this->assert_str_contains($cso_conf, 'updated_test_custom_option example1');
        $this->assert_str_contains($cso_conf, 'updated_test_custom_option example2');

        # Delete the CSO and ensure it is no longer present
        $cso->delete();
        $this->assert_is_false(
            is_file("/var/etc/openvpn/server{$this->ovpns->vpnid->value}/csc/{$cso->common_name->value}"),
        );
    }
}
