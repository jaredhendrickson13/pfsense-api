<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecPhase1;
use RESTAPI\Models\IPsecPhase2;

class APIModelsIPsecPhase2TestCase extends TestCase {
    const BASE_P1_DATA = [
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
    const BASE_P2_DATA = [
        'ikeid' => 1,
        'mode' => 'tunnel',
        'localid_type' => 'network',
        'localid_address' => '192.168.1.1',
        'localid_netbits' => 24,
        'remoteid_type' => 'network',
        'remoteid_address' => '1.2.3.4',
        'remoteid_netbits' => 24,
        'protocol' => 'esp',
        'encryption_algorithm_option' => [['name' => 'aes', 'keylen' => 128]],
        'hash_algorithm_option' => ['hmac_sha256'],
        'pfsgroup' => 14,
    ];

    /**
     * @var $p1 IPsecPhase1 The IPsecPhase1 object created before running tests.
     */
    private IPsecPhase1 $p1;

    /**
     * Before running tests, create an IPsecPhase1 entry we can use during tests.
     */
    public function setup(): void {
        $this->p1 = new IPsecPhase1(data: self::BASE_P1_DATA, async: false);
        $this->p1->create(apply: true);
    }

    /**
     * After running tests, destroy the IPsecPhase1 entry we created before running tests.
     */
    public function teardown(): void {
        $this->p1->delete(apply: true);
    }

    /**
     * Checks that the `localid_type` cannot be set to an interface if `mode` is set to `vti`
     */
    public function test_localid_type_cannot_be_interface_while_mode_is_vti(): void {
        # Ensure interfaces are not accepted when mode is vti
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_LOCALID_TYPE_CANNOT_BE_INTERFACE_WITH_VTI_MODE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'vti';
                $p2->validate_localid_type('lan');
            },
        );

        # Ensure interfaces are accepted when mode is not vti
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel';
                $p2->validate_localid_type('lan');
            },
        );
    }

    /**
     * Checks that `localid_address` must be an IPv4 address when `mode` is set to `tunnel`
     */
    public function test_localid_address_must_be_ipv4_with_tunnel_mode(): void {
        # Ensure an error is thrown if mode is tunnel but localid_address is an IPv6
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_LOCALID_ADDRESS_MUST_BE_IPV4_WITH_TUNNEL_MODE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel';
                $p2->localid_address->value = '1234::1';
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure no error is thrown if mode is tunnel and localid_address is an IPv4
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel';
                $p2->localid_address->value = '1.2.3.4';
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that `localid_address` must be an IPv6 address when `mode` is set to `tunnel6`
     */
    public function test_localid_address_must_be_ipv6_with_tunnel6_mode(): void {
        # Ensure an error is thrown if mode is tunnel6 but localid_address is an IPv6
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_LOCALID_ADDRESS_MUST_BE_IPV6_WITH_TUNNEL6_MODE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '1.2.3.4';
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure no error is thrown if mode is tunnel6 and localid_address is an IPv6
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '1234::1';
                $p2->remoteid_address->value = '4321::1';
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that the `localid_netbits` field cannot be greater than 32 if `localid_address` is an IPv4 address
     */
    public function test_localid_netbits_cannot_be_greater_than_32_with_ipv4(): void {
        # Ensure an error is thrown if a localid_netbits value greater than 32 is assigned to an IPv4 address
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_LOCALID_NETBITS_EXCEEDS_IPV4_MAXIMUM',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->localid_address->value = '1.2.3.4';
                $p2->localid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure an error is not thrown if a localid_netbits value less than or equal to 32 is assigned to an IPv4 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->localid_address->value = '1.2.3.4';
                $p2->localid_netbits->value = 32;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure an error is not thrown if a localid_netbits value greater than 32 is assigned to an IPv6 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '1234::1';
                $p2->remoteid_address->value = '4321::1';
                $p2->localid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that the `localid_type` and `natlocalid_type` cannot be mismatched.
     */
    public function test_natlocalid_type_must_match_localid_type(): void {
        # Ensure an error is thrown if a natlocalid_type is address and localid_type is network.
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_NATLOCALID_TYPE_MUST_MATCH_LOCALID_TYPE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->localid_type->value = 'network';
                $p2->validate_natlocalid_type('address');
            },
        );

        # Ensure an error is thrown if a natlocalid_type is network and localid_type is address.
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_NATLOCALID_TYPE_MUST_MATCH_LOCALID_TYPE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->localid_type->value = 'address';
                $p2->validate_natlocalid_type('network');
            },
        );

        # Ensure no error is thrown if the values match
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->localid_type->value = 'address';
                $p2->validate_natlocalid_type('address');
                $p2->localid_type->value = 'network';
                $p2->validate_natlocalid_type('network');
            },
        );
    }

    /**
     * Checks that `natlocalid_address` must be an IPv4 address when `mode` is set to `tunnel`
     */
    public function test_natlocalid_address_must_be_ipv4_with_tunnel_mode(): void {
        # Ensure an error is thrown if mode is tunnel but natlocalid_address is an IPv6
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_NATLOCALID_ADDRESS_MUST_BE_IPV4_WITH_TUNNEL_MODE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel';
                $p2->natlocalid_type->value = 'network';
                $p2->natlocalid_address->value = '1234::1';
                $p2->natlocalid_netbits->value = 24;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure no error is thrown if mode is tunnel and natlocalid_address is an IPv4
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel';
                $p2->natlocalid_type->value = 'network';
                $p2->natlocalid_address->value = '1.2.3.4';
                $p2->natlocalid_netbits->value = 24;
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that `natlocalid_address` must be an IPv6 address when `mode` is set to `tunnel6`
     */
    public function test_natlocalid_address_must_be_ipv6_with_tunnel6_mode(): void {
        # Ensure an error is thrown if mode is tunnel6 but natlocalid_address is an IPv6
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_NATLOCALID_ADDRESS_MUST_BE_IPV6_WITH_TUNNEL6_MODE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '1111::1';
                $p2->natlocalid_type->value = 'network';
                $p2->natlocalid_address->value = '1.2.3.4';
                $p2->natlocalid_netbits->value = 24;
                $p2->remoteid_address->value = '4321::1';
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure no error is thrown if mode is tunnel6 and natlocalid_address is an IPv6
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '1111::1';
                $p2->natlocalid_type->value = 'network';
                $p2->natlocalid_address->value = '1234::1';
                $p2->natlocalid_netbits->value = 24;
                $p2->remoteid_address->value = '4321::1';
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that the `natlocalid_netbits` field cannot be greater than 32 if `natlocalid_address` is an IPv4 address
     */
    public function test_natlocalid_netbits_cannot_be_greater_than_32_with_ipv4(): void {
        # Ensure an error is thrown if a natlocalid_netbits value greater than 32 is assigned to an IPv4 address
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_NATLOCALID_NETBITS_EXCEEDS_IPV4_MAXIMUM',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->natlocalid_type->value = 'network';
                $p2->natlocalid_address->value = '1.2.3.4';
                $p2->natlocalid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure an error is not thrown if a natlocalid_netbits value less than or equal to 32 is assigned to an IPv4 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->natlocalid_type->value = 'network';
                $p2->natlocalid_address->value = '1.2.3.4';
                $p2->natlocalid_netbits->value = 32;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure an error is not thrown if a natlocalid_netbits value greater than 32 is assigned to an IPv6 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '1111::1';
                $p2->natlocalid_type->value = 'network';
                $p2->natlocalid_address->value = '1234::1';
                $p2->remoteid_address->value = '4321::1';
                $p2->natlocalid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that `remoteid_address` must be an IPv4 address when `mode` is set to `tunnel`
     */
    public function test_remoteid_address_must_be_ipv4_with_tunnel_mode(): void {
        # Ensure an error is thrown if mode is tunnel but remoteid_address is an IPv6
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_REMOTEID_ADDRESS_MUST_BE_IPV4_WITH_TUNNEL_MODE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel';
                $p2->remoteid_type->value = 'network';
                $p2->remoteid_address->value = '1234::1';
                $p2->remoteid_netbits->value = 24;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure no error is thrown if mode is tunnel and remoteid_address is an IPv4
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel';
                $p2->remoteid_type->value = 'network';
                $p2->remoteid_address->value = '1.2.3.4';
                $p2->remoteid_netbits->value = 24;
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that `remoteid_address` must be an IPv6 address when `mode` is set to `tunnel6`
     */
    public function test_remoteid_address_must_be_ipv6_with_tunnel6_mode(): void {
        # Ensure an error is thrown if mode is tunnel6 but remoteid_address is an IPv6
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_REMOTEID_ADDRESS_MUST_BE_IPV6_WITH_TUNNEL6_MODE',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '4321::1';
                $p2->remoteid_type->value = 'network';
                $p2->remoteid_address->value = '1.2.3.4';
                $p2->remoteid_netbits->value = 24;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure no error is thrown if mode is tunnel6 and remoteid_address is an IPv6
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '4321::1';
                $p2->remoteid_type->value = 'network';
                $p2->remoteid_address->value = '1234::1';
                $p2->remoteid_netbits->value = 24;
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that the `remoteid_netbits` field cannot be greater than 32 if `remoteid_address` is an IPv4 address
     */
    public function test_remoteid_netbits_cannot_be_greater_than_32_with_ipv4(): void {
        # Ensure an error is thrown if a remoteid_netbits value greater than 32 is assigned to an IPv4 address
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_REMOTEID_NETBITS_EXCEEDS_IPV4_MAXIMUM',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->remoteid_type->value = 'network';
                $p2->remoteid_address->value = '1.2.3.4';
                $p2->remoteid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure an error is not thrown if a remoteid_netbits value less than or equal to 32 is assigned to an IPv4 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->remoteid_type->value = 'network';
                $p2->remoteid_address->value = '1.2.3.4';
                $p2->remoteid_netbits->value = 32;
                $p2->validate(skip_parent: true);
            },
        );

        # Ensure an error is not thrown if a remoteid_netbits value greater than 32 is assigned to an IPv6 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = 'tunnel6';
                $p2->localid_address->value = '4321::1';
                $p2->remoteid_type->value = 'network';
                $p2->remoteid_address->value = '1234::1';
                $p2->remoteid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            },
        );
    }

    /**
     * Checks that the `rekey_time` must be less than the `lifetime` value.
     */
    public function test_rekey_time_must_be_less_than_lifetime(): void {
        # Ensure an error is thrown when `rekey_time` is greater than `lifetime`
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_REKEY_TIME_GREATER_THAN_LIFETIME',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->lifetime->value = 3600;
                $p2->validate_rekey_time(3601);
            },
        );

        # Ensure an error is thrown when `rekey_time` is equal to `lifetime`
        $this->assert_throws_response(
            response_id: 'IPSEC_PHASE_2_REKEY_TIME_GREATER_THAN_LIFETIME',
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->lifetime->value = 3600;
                $p2->validate_rekey_time($p2->lifetime->value);
            },
        );

        # Ensure no error is thrown when rekey_time is less than lifetime
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->lifetime->value = 3600;
                $p2->validate_rekey_time(3599);
            },
        );
    }

    /**
     * Check that the `mode` field is correctly represented in swanctl.conf after applying.
     */
    public function test_mode_found_in_swanctl(): void {
        # Create a tunnel mode p2 to test with
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->mode->value = 'tunnel';
        $p2->create(apply: true);

        # Ensure the p2 mode is shown as `tunnel` in swanctl.conf
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'mode = tunnel');

        # Update the mode to tunnel6 and ensure it is still represented as tunnel in swanctl.conf
        $p2->mode->value = 'tunnel6';
        $p2->localid_address->value = '1234::1';
        $p2->remoteid_address->value = '4321::1';
        $p2->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'mode = tunnel');

        # Update the mode to transport and ensure it is represented as transport in swanctl.conf
        $p2->mode->value = 'transport';
        $p2->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'mode = transport');

        # Update the mode to vi and ensure the mode is no longer present and policies is set to no in swanctl.conf
        $p2->from_representation(self::BASE_P2_DATA);
        $p2->mode->value = 'vti';
        $p2->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'policies = no');

        # Delete the p1 and p2 entries.
        $p2->delete();
    }

    /**
     * Checks that the `localid_*` fields are correctly represented in swanctl.conf
     */
    public function test_localid_found_in_swanctl(): void {
        # Create a p2 to test with and interface as the localid
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->mode->value = 'tunnel';
        $p2->localid_type->value = 'wan';
        $p2->create(apply: true);

        # Ensure the lan interface's subnet is shown as the local_ts item in swanctl.conf
        $lan_addr = $p2->localid_type->get_related_model()->get_network_ipv4();
        $lan_subnet = $p2->localid_type->get_related_model()->get_current_subnetv4();
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "local_ts = $lan_addr/$lan_subnet");

        # Change the localid to an address and ensure it is represented correctly in swanctl
        $p2->localid_type->value = 'address';
        $p2->localid_address->value = '4.3.2.1';
        $p2->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, 'local_ts = 4.3.2.1');

        # Change the localid to an address and ensure it is represented correctly in swanctl
        $p2->localid_type->value = 'network';
        $p2->localid_address->value = '4.3.2.0';
        $p2->localid_netbits->value = 25;
        $p2->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "local_ts = {$p2->localid_address->value}/{$p2->localid_netbits->value}");

        # Delete the p2
        $p2->delete(apply: true);
    }

    /**
     * Checks that the `natlocalid_*` fields are correctly represented in swanctl.conf
     */
    public function test_natlocalid_found_in_swanctl(): void {
        # Create a p2 to test with
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->mode->value = 'tunnel';
        $p2->localid_type->value = 'address';
        $p2->localid_address->value = '1.2.3.4';
        $p2->natlocalid_type->value = 'address';
        $p2->natlocalid_address->value = '4.3.2.1';
        $p2->create(apply: true);

        # Ensure the natlocalid fields are correctly represented in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains(
            $swanctl,
            "local_ts = {$p2->natlocalid_address->value}|{$p2->localid_address->value}",
        );

        # Delete the p2
        $p2->delete(apply: true);
    }

    /**
     * Checks that the `remote_id*` fields are correctly represented in swanctl.conf
     */
    public function test_remoteid_found_in_swanctl(): void {
        # Create a p2 to test with
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->remoteid_type->value = 'address';
        $p2->remoteid_address->value = '4.3.2.1';
        $p2->create(apply: true);

        # Ensure the remoteid fields are correctly represented in swanctl for 'address' remoteid_type
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "remote_ts = {$p2->remoteid_address->value}");

        # Ensure the remoteid fields are correctly represented in swanctl for 'network' remoteid_type
        $p2->remoteid_type->value = 'network';
        $p2->remoteid_address->value = '5.5.5.5';
        $p2->remoteid_netbits->value = 26;
        $p2->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains(
            $swanctl,
            "remote_ts = {$p2->remoteid_address->value}/{$p2->remoteid_netbits->value}",
        );

        # Delete the p2
        $p2->delete(apply: true);
    }

    /**
     * Checks that the `protocol` field is correctly represented in swanctl.conf
     */
    public function test_protocol_found_in_swanctl(): void {
        # Create a p2 to test with
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->protocol->value = 'ah';
        $p2->hash_algorithm_option->value = ['hmac_sha512', 'hmac_sha256'];
        $p2->pfsgroup->value = 20;
        $p2->create(apply: true);

        # Ensure the `ah` protocol value is correctly represented in swanctl
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, $this->get_expected_proposals($p2));

        # Ensure the `esp` protocol value is correctly represented in swanctl
        $p2->protocol->value = 'esp';
        $p2->encryption_algorithm_option->value = [
            ['name' => 'aes', 'keylen' => 256],
            ['name' => 'aes', 'keylen' => 128],
        ];
        $p2->update(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, $this->get_expected_proposals($p2));

        # Delete the p2
        $p2->delete(apply: true);
    }

    /**
     * Checks that the `lifetime` field is correctly represented in swanctl.conf
     */
    public function test_lifetime_found_in_swanctl(): void {
        # Create a p2 to test with
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->lifetime->value = 4000;
        $p2->create(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "life_time = {$p2->lifetime->value}s");

        # Delete the p2
        $p2->delete(apply: true);
    }

    /**
     * Checks that the `rekey_time` field is correctly represented in swanctl.conf
     */
    public function test_rekey_time_found_in_swanctl(): void {
        # Create a p2 to test with
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->rekey_time->value = 2000;
        $p2->create(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "rekey_time = {$p2->rekey_time->value}s");

        # Delete the p2
        $p2->delete(apply: true);
    }

    /**
     * Checks that the `rand_time` field is correctly represented in swanctl.conf
     */
    public function test_rand_time_found_in_swanctl(): void {
        # Create a p2 to test with
        $p2 = new IPsecPhase2(data: self::BASE_P2_DATA, async: false);
        $p2->ikeid->value = $this->p1->ikeid->value;
        $p2->rand_time->value = 300;
        $p2->create(apply: true);
        $swanctl = file_get_contents('/var/etc/ipsec/swanctl.conf');
        $this->assert_str_contains($swanctl, "rand_time = {$p2->rand_time->value}s");

        # Delete the p2
        $p2->delete(apply: true);
    }

    /**
     * Generates the ESP or AH proposals line expected to be found in swanctl.conf for a given IPsecPhase2 object.
     */
    public function get_expected_proposals(IPsecPhase2 $p2): string {
        # Variables
        $proposals = [];
        $pfs = ipsec_convert_to_modp($p2->pfsgroup->value);
        $protocol = $p2->protocol->value;

        # Obtain the expected proposals for the `ah` protocol
        if ($protocol === 'ah') {
            # Reverse the order of the hash options to match how they're represented in swanctl
            $hashes = array_reverse($p2->hash_algorithm_option->value);

            # Add a proposal for each hash algorithm defined
            foreach ($hashes as $hash) {
                $hash = str_replace('hmac_', '', $hash);
                $proposals[] = "$hash-$pfs";
            }
        } elseif ($protocol === 'esp') {
            # Loop through and assign a proposal for each encryption and hash combination
            foreach ($p2->encryption_algorithm_option->value as $algo) {
                foreach ($p2->hash_algorithm_option->value as $hash) {
                    $hash = str_replace('hmac_', '', $hash);
                    $name = $algo['name'];
                    $keylen = $algo['keylen'];
                    $proposals[] = str_contains($name, 'gcm') ? "$name$keylen-$pfs" : "$name$keylen-$hash-$pfs";
                }
            }
        }

        return $protocol . '_proposals = ' . implode(',', $proposals);
    }
}
