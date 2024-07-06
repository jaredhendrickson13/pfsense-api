<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\IPsecPhase1;

class APIModelsIPsecPhase1TestCase extends TestCase {
    const BASE_DATA = [
        'iketype' => 'ikev2',
        'protocol' => 'inet',
        'interface' => 'wan',
        'remote_gateway' => '1.2.3.4',
        'authentication_method' => 'pre_shared_key',
        'myid_type' => 'myaddress',
        'peerid_type' => 'peeraddress',
        'pre_shared_key' => 'testkey',
        'encryption' => [
            [
                'encryption_algorithm_name' => 'aes',
                'encryption_algorithm_keylen' => 256,
                'hash_algorithm' => 'sha256',
                'dhgroup' => 14,
            ],
        ],
    ];

    /**
     * Checks that `remote_gateway` cannot be set to an IPv6 address if `protocol` is set to `inet`
     */
    public function test_remote_gateway_cannot_be_ipv6_with_inet_protocol(): void {
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_IPV6_REMOTE_GATEWAY_NOT_ALLOWED',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1(data: self::BASE_DATA);
                $p1->protocol->value = 'inet';
                $p1->remote_gateway->value = '1234::1';
                $p1->validate();
            },
        );
    }

    /**
     * Checks that `remote_gateway` cannot be set to an IPv4 address if `protocol` is set to `inet6`
     */
    public function test_remote_gateway_cannot_be_ipv4_with_inet6_protocol(): void {
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_IPV4_REMOTE_GATEWAY_NOT_ALLOWED',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1(data: self::BASE_DATA);
                $p1->protocol->value = 'inet6';
                $p1->remote_gateway->value = '1.2.3.4';
                $p1->validate();
            },
        );
    }

    /**
     * Checks that the `remote_gateway` value must be unique unless all duplicate gateways have `gw_duplicates` enabled
     */
    public function test_remote_gateway_unique_without_dup_gw(): void {
        # Create an IPsec phase 1 entry to test with
        $p1 = new IPsecPhase1(data: self::BASE_DATA);
        $p1->create();

        # Try to create another phase 1 entry with the same gateway and ensure it throws an error
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_ILLEGAL_DUPLICATE_REMOTE_GATEWAY',
            code: 400,
            callable: function () {
                $dup_p1 = new IPsecPhase1(data: self::BASE_DATA);
                $dup_p1->create();
            },
        );

        # Ensure enabling `gw_duplicates` allows multiple phase 1 entries to use the same `remote_gateway`
        $p1->gw_duplicates->value = true;
        $p1->update();
        $this->assert_does_not_throw(
            callable: function () {
                $dup_p1 = new IPsecPhase1(data: self::BASE_DATA);
                $dup_p1->gw_duplicates->value = true;
                $dup_p1->create();
                $dup_p1->delete();
            },
        );

        # Delete the IPsec phase 1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `remote_gateway` field can be shared with other phase 1 entries without `gw_duplicates` if it is
     * an INADRR_ANY address.
     */
    public function test_remote_gateway_inaddr_any_without_gw_duplicates(): void {
        # Create an IPsec phase 1 entry to test with and use an INADDR_ANY remote gateway
        $p1 = new IPsecPhase1(data: self::BASE_DATA);
        $p1->remote_gateway->value = '0.0.0.0';
        $p1->peerid_type->value = 'any';
        $p1->startaction->value = 'none';
        $p1->create();

        # Ensure multiple phase 1 entries can use this IP even if gw_duplicates is disabled
        $p1->gw_duplicates->value = true;
        $p1->update();
        $this->assert_does_not_throw(
            callable: function () {
                $dup_p1 = new IPsecPhase1(data: self::BASE_DATA);
                $dup_p1->remote_gateway->value = '0.0.0.0';
                $dup_p1->peerid_type->value = 'any';
                $dup_p1->startaction->value = 'none';
                $dup_p1->create();
                $dup_p1->delete();
            },
        );

        # Delete the IPsec phase 1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `myid_data` field correctly enforces input validation depending on the `myid_type`.
     */
    public function test_validate_myid_data(): void {
        # Ensure `myid_type` value `address` must be IP address
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_MYID_DATA_MUST_BE_IP_ADDRESS',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'address';
                $p1->validate_myid_data('not an IP address!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'address';
                $p1->validate_myid_data('1.2.3.4');
            },
        );

        # Ensure `myid_type` value `fqdn` must be an FQDN
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_MYID_DATA_MUST_BE_FQDN',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'fqdn';
                $p1->validate_myid_data('not an FQDN!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'fqdn';
                $p1->validate_myid_data('example.com');
            },
        );

        # Ensure `myid_type` value `user_fqdn` must be an email address
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_MYID_DATA_MUST_BE_EMAIL_ADDRESS',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'user_fqdn';
                $p1->validate_myid_data('not an email!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'user_fqdn';
                $p1->validate_myid_data('test@example.com');
            },
        );

        # Ensure `myid_type` value `dyn_dns` must be an FQDN
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_MYID_DATA_MUST_BE_FQDN',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'dyn_dns';
                $p1->validate_myid_data('not an FQDN!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = 'dyn_dns';
                $p1->validate_myid_data('example.com');
            },
        );
    }

    /**
     * Checks that the `peerid_type` cannot be set to `peeraddress` when the `remote_gateway` is an INADDR_ANY IP
     */
    public function test_peerid_type_cannot_be_peeraddress_with_inaddr_any_remote_gateway(): void {
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_NO_INADDR_ANY_WITH_PEERADDRESS',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->remote_gateway->value = '0.0.0.0';
                $p1->validate_peerid_type('peeraddress');
            },
        );
    }

    /**
     * Checks that the `peerid_data` field correctly enforces input validation depending on the `peerid_type`.
     */
    public function test_validate_peerid_data(): void {
        # Ensure `peerid_type` value `address` must be IP address
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_PEERID_DATA_MUST_BE_IP_ADDRESS',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'address';
                $p1->validate_peerid_data('not an IP address!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'address';
                $p1->validate_peerid_data('1.2.3.4');
            },
        );

        # Ensure `peerid_type` value `fqdn` must be an FQDN
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_PEERID_DATA_MUST_BE_FQDN',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'fqdn';
                $p1->validate_peerid_data('not an FQDN!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'fqdn';
                $p1->validate_peerid_data('example.com');
            },
        );

        # Ensure `peerid_type` value `user_fqdn` must be an email address
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_PEERID_DATA_MUST_BE_EMAIL_ADDRESS',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'user_fqdn';
                $p1->validate_peerid_data('not an email!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'user_fqdn';
                $p1->validate_peerid_data('test@example.com');
            },
        );

        # Ensure `peerid_type` value `dyn_dns` must be an FQDN
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_PEERID_DATA_MUST_BE_FQDN',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'dyn_dns';
                $p1->validate_peerid_data('not an FQDN!');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = 'dyn_dns';
                $p1->validate_peerid_data('example.com');
            },
        );
    }

    /**
     * Checks that the `rekey_time` cannot be set to a value higher than `lifetime`
     */
    public function test_rekey_time_cannot_be_greater_than_lifetime(): void {
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_REKEY_TIME_GREATER_THAN_LIFETIME',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->lifetime->value = 10000;
                $p1->validate_rekey_time(10001);
            },
        );
    }

    /**
     * Checks that the `reauth_time` cannot be set to a value higher than `lifetime`
     */
    public function test_reauth_time_cannot_be_greater_than_lifetime(): void {
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_REAUTH_TIME_GREATER_THAN_LIFETIME',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->lifetime->value = 10000;
                $p1->validate_reauth_time(10001);
            },
        );
    }

    /**
     * Checks that the `startaction` must be set to `none` when `remote_gateway` is an INADDR_ANY IP
     */
    public function test_startaction_must_be_none_with_inaddr_any_remote_gateway(): void {
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_NO_STARTACTION_WITH_INADDR_ANY_REMOTE_GATEWAY',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->remote_gateway->value = '0.0.0.0';
                $p1->validate_startaction('start');
            },
        );
    }

    /**
     * Checks that the `nattport` cannot use the same port as `ikeport`
     */
    public function test_nattport_cannot_match_ikeport(): void {
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_1_NATTPORT_CANNOT_MATCH_IKEPORT',
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->ikeport->value = '500';
                $p1->validate_nattport('500');
            },
        );
    }

    /**
     * Checks that the `iketype` is correctly represented on the backend after applying.
     */
    public function test_iketype_found_in_swanctl(): void {
        # Create a new IPsec phase 1 using IKEv2
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->iketype->value = 'ikev2';
        $p1->create(apply: true);

        # Ensure the `iketype` is correctly shown in swanctl
        $swanctl_conf = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl_conf, 'version = 2');

        # Update the p1 entry to IKEv1 and ensure it is correctly shown
        $p1->iketype->value = 'ikev1';
        $p1->mode->value = 'main';
        $p1->update(apply: true);
        $swanctl_conf = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl_conf, 'version = 1');

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the IKEv1 `mode` setting is correctly represented on the backend after applying.
     */
    public function test_ikev1_mode_found_in_swanctl(): void {
        # Create a new IPsec phase 1 in main mode
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->iketype->value = 'ikev1';
        $p1->mode->value = 'main';
        $p1->create(apply: true);

        # Ensure aggressive mode is not enabled in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'aggressive = no');

        # Enable aggressive move and ensure it is enabled in swanctl
        $p1->mode->value = 'aggressive';
        $p1->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'aggressive = yes');

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `interface` field is correctly represented on the backend after apply
     */
    public function test_interface_found_in_swanctl(): void {
        # Create a new IPsec phase 1 using the wan interface
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->interface->value = 'wan';
        $p1->create(apply: true);

        # Ensure the WAN interface's IP is set as the local_addrs in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $wan_ip = $p1->interface->get_related_model()->get_current_ipv4();
        $this->assert_str_contains($swanctl, "local_addrs = $wan_ip");

        # Update the p1 to use the LAN interface and ensure the local_addrs IP changes
        $p1->interface->value = 'lan';
        $p1->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $lan_ip = $p1->interface->get_related_model()->get_current_ipv4();
        $this->assert_str_does_not_contain($swanctl, "local_addrs = $wan_ip");
        $this->assert_str_contains($swanctl, "local_addrs = $lan_ip");

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `remote_gateway` value is correctly represented on the backend after applying
     */
    public function test_remote_gateway_found_in_swanctl(): void {
        # Create a new IPsec phase 1 using the wan interface
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->remote_gateway->value = '1.2.3.4';
        $p1->create(apply: true);

        # Ensure the remote_gateway is correctly set in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'remote_addrs = 1.2.3.4');

        # Update the remote gateway and ensure it changes in swanctl
        $p1->remote_gateway->value = '4.3.2.1';
        $p1->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_does_not_contain($swanctl, 'remote_addrs = 1.2.3.4');
        $this->assert_str_contains($swanctl, 'remote_addrs = 4.3.2.1');

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `authentication_method` is correctly represented on the backend after applying.
     */
    public function test_authentication_method_found_in_swanctl(): void {
        # Create a new IPsec phase 1 using PSK auth
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->authentication_method->value = 'pre_shared_key';
        $p1->pre_shared_key->value = 'test_key';
        $p1->create(apply: true);

        # Ensure psk auth is used by swanctl and that the psk is correctly shown
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'auth = psk');
        $this->assert_str_contains($swanctl, 'secret = 0s' . base64_encode($p1->pre_shared_key->value));

        # Create a cert and CA to use for `cert` authentication testing
        $test_cert_x509 = file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt');
        $test_key_x509 = file_get_contents('/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key');
        $cert = new Certificate(data: ['descr' => 'testcert', 'crt' => $test_cert_x509, 'prv' => $test_key_x509]);
        $ca = new CertificateAuthority(data: ['descr' => 'testca', 'crt' => $test_cert_x509, 'prv' => $test_key_x509]);
        $cert->create();
        $ca->create();

        # Update the p1 to use cert auth and ensure it is correctly shown in swanctl
        $p1->authentication_method->value = 'cert';
        $p1->certref->value = $cert->refid->value;
        $p1->caref->value = $ca->refid->value;
        $p1->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'send_cert = always');
        $this->assert_str_contains($swanctl, 'auth = pubkey');

        # Delete the p1 entry, cert and ca
        $p1->delete(apply: true);
        $cert->delete();
        $ca->delete();
    }

    /**
     * Checks that the `lifetime` is correctly represented on the backend after applying.
     */
    public function test_lifetime_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific lifetime and rekey_time values
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->lifetime->value = 30000;
        $p1->rekey_time->value = 25000;
        $p1->create(apply: true);

        # Ensure the `over_time` swanctl value is equal to lifetime minus the rekey_time
        $over_time = $p1->lifetime->value - $p1->rekey_time->value;
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "over_time = $over_time");

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `rekey_time` is correctly represented on the backend after applying.
     */
    public function test_rekey_time_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific rekey_time value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->rekey_time->value = 25000;
        $p1->create(apply: true);

        # Ensure the rekey_time is correctly shown in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "rekey_time = {$p1->rekey_time->value}");

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `reauth_time` is correctly represented on the backend after applying.
     */
    public function test_reauth_time_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific reauth_time value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->reauth_time->value = 25000;
        $p1->create(apply: true);

        # Ensure the reauth_time is correctly shown in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "reauth_time = {$p1->reauth_time->value}");

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `rand_time` is correctly represented on the backend after applying.
     */
    public function test_rand_time_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific rand_time value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->rand_time->value = 3000;
        $p1->create(apply: true);

        # Ensure the rand_time is correctly shown in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "rand_time = {$p1->rand_time->value}");

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `nat_traversal` is correctly represented on the backend after applying.
     */
    public function test_nat_traversal_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific nat_traversal value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->nat_traversal->value = 'force';
        $p1->create(apply: true);

        # Ensure the nat_traversal is correctly shown in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'encap = yes');

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `mobike` is correctly represented on the backend after applying.
     */
    public function test_mobike_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific mobike value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->mobike->value = true;
        $p1->create(apply: true);

        # Ensure the mobike is correctly shown in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'mobike = yes');

        # Update the p1 to disable mobike and recheck swanctl
        $p1->mobike->value = false;
        $p1->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'mobike = no');

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `ikeport` is correctly represented on the backend after applying.
     */
    public function test_ikeport_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific ikeport value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->ikeport->value = '501';
        $p1->create(apply: true);

        # Ensure the ikeport is correctly shown in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'remote_port = ' . $p1->ikeport->value);

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `dpd_delay` is correctly represented on the backend after applying.
     */
    public function test_dpd_delay_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific dpd_delay value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->dpd_delay->value = 15;
        $p1->create(apply: true);

        # Ensure the dpd_delay is correctly shown in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'dpd_delay = ' . $p1->dpd_delay->value);

        # Delete the p1 entry
        $p1->delete(apply: true);
    }

    /**
     * Checks that the `dpd_maxfail` is correctly represented on the backend after applying.
     */
    public function test_dpd_maxfail_found_in_swanctl(): void {
        # Create a new IPsec phase 1 with a specific dpd_maxfail value
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->iketype->value = 'ikev1';
        $p1->mode->value = 'main';
        $p1->dpd_delay->value = 15;
        $p1->dpd_maxfail->value = 10;
        $p1->create(apply: true);

        # Ensure the dpd_maxfail is correctly shown in swanctl
        $dpd_timeout = $p1->dpd_delay->value * ($p1->dpd_maxfail->value + 1);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "dpd_timeout = $dpd_timeout");

        # Delete the p1 entry
        $p1->delete(apply: true);
    }
}
