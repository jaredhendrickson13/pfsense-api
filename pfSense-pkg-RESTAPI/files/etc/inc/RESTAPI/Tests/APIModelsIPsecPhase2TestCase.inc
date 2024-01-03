<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecPhase1;
use RESTAPI\Models\IPsecPhase2;

class APIModelsIPsecPhase2TestCase extends TestCase
{
    const BASE_P1_DATA = [
        "iketype" => "ikev2",
        "protocol" => "inet",
        "interface" => "wan",
        "remote_gateway" => "1.2.3.4",
        "authentication_method" => "pre_shared_key",
        "myid_type" => "myaddress",
        "peerid_type" => "peeraddress",
        "pre_shared_key" => "testkey",
        "encryption" => [
            [
                "encryption_algorithm_name" => "aes",
                "encryption_algorithm_keylen" => 256,
                "hash_algorithm" => "sha256",
                "dhgroup" => 14
            ]
        ]
    ];
    const BASE_P2_DATA = [
        "ikeid" => 1,
        "mode" => "tunnel",
        "localid_type" => "network",
        "Localid_address" => "192.168.1.1",
        "localid_netbits" => 24,
        "remoteid_type" => "network",
        "remoteid_address" => "1.2.3.4",
        "remoteid_netbits" => 24,
        "protocol" => "esp",
        "encryption_algorithm_option" => [["name" => "aes", "keylen" => 128]],
        "hash_algorithm_option" => ["hmac_sha256"],
        "pfsgroup" => 14
    ];

    /**
     * Checks that the `localid_type` cannot be set to an interface if `mode` is set to `vti`
     */
    public function test_localid_type_cannot_be_interface_while_mode_is_vti(): void {
        # Ensure interfaces are not accepted when mode is vti
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_2_LOCALID_TYPE_CANNOT_BE_INTERFACE_WITH_VTI_MODE",
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = "vti";
                $p2->validate_localid_type("lan");
            }
        );

        # Ensure interfaces are accepted when mode is not vti
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = "tunnel";
                $p2->validate_localid_type("lan");
            }
        );
    }

    /**
     * Checks that `localid_address` must be an IPv4 address when `mode` is set to `tunnel`
     */
    public function test_localid_address_must_be_ipv4_with_tunnel_mode(): void {
        # Ensure an error is thrown if mode is tunnel but localid_address is an IPv6
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_2_LOCALID_ADDRESS_MUST_BE_IPV4_WITH_TUNNEL_MODE",
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = "tunnel";
                $p2->localid_address->value = "1234::1";
                $p2->validate(skip_parent: true);
            }
        );

        # Ensure no error is thrown if mode is tunnel and localid_address is an IPv4
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = "tunnel";
                $p2->localid_address->value = "1.2.3.4";
                $p2->validate(skip_parent: true);
            }
        );
    }

    /**
     * Checks that `localid_address` must be an IPv6 address when `mode` is set to `tunnel6`
     */
    public function test_localid_address_must_be_ipv6_with_tunnel6_mode(): void {
        # Ensure an error is thrown if mode is tunnel6 but localid_address is an IPv6
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_2_LOCALID_ADDRESS_MUST_BE_IPV6_WITH_TUNNEL6_MODE",
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = "tunnel6";
                $p2->localid_address->value = "1.2.3.4";
                $p2->validate(skip_parent: true);
            }
        );

        # Ensure no error is thrown if mode is tunnel6 and localid_address is an IPv6
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = "tunnel6";
                $p2->localid_address->value = "1234::1";
                $p2->remoteid_address->value = "4321::1";
                $p2->validate(skip_parent: true);
            }
        );
    }

    /**
     * Checks that the `localid_netbits` field cannot be greater than 32 if `localid_address` is an IPv4 address
     */
    public function test_localid_netbits_cannot_be_greater_than_32_with_ipv4(): void {
        # Ensure an error is thrown if a localid_netbits value greater than 32 is assigned to an IPv4 address
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_2_LOCALID_NETBITS_EXCEEDS_IPV4_MAXIMUM",
            code: 400,
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->localid_address->value = "1.2.3.4";
                $p2->localid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            }
        );

        # Ensure an error is not thrown if a localid_netbits value less than or equal to 32 is assigned to an IPv4 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->localid_address->value = "1.2.3.4";
                $p2->localid_netbits->value = 32;
                $p2->validate(skip_parent: true);
            }
        );

        # Ensure an error is not thrown if a localid_netbits value greater than 32 is assigned to an IPv6 address
        $this->assert_does_not_throw(
            callable: function () {
                $p2 = new IPsecPhase2(data: self::BASE_P2_DATA);
                $p2->mode->value = "tunnel6";
                $p2->localid_address->value = "1234::1";
                $p2->remoteid_address->value = "4321::1";
                $p2->localid_netbits->value = 33;
                $p2->validate(skip_parent: true);
            }
        );
    }
}