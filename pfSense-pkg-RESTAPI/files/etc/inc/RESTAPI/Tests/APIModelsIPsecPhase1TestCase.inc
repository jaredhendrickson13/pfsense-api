<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\IPsecPhase1;

class APIModelsIPsecPhase1TestCase extends TestCase
{
    const BASE_DATA = [
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
    
    /**
     * Checks that `remote_gateway` cannot be set to an IPv6 address if `protocol` is set to `inet`
     */
    public function test_remote_gateway_cannot_be_ipv6_with_inet_protocol(): void {
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_IPV6_REMOTE_GATEWAY_NOT_ALLOWED",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1(data: self::BASE_DATA);
                $p1->protocol->value = "inet";
                $p1->remote_gateway->value = "1234::1";
                $p1->validate();
            }
        );
    }

    /**
     * Checks that `remote_gateway` cannot be set to an IPv4 address if `protocol` is set to `inet6`
     */
    public function test_remote_gateway_cannot_be_ipv4_with_inet6_protocol(): void {
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_IPV4_REMOTE_GATEWAY_NOT_ALLOWED",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1(data: self::BASE_DATA);
                $p1->protocol->value = "inet6";
                $p1->remote_gateway->value = "1.2.3.4";
                $p1->validate();
            }
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
            response_id: "IPSEC_PHASE_1_ILLEGAL_DUPLICATE_REMOTE_GATEWAY",
            code: 400,
            callable: function () {
                $dup_p1 = new IPsecPhase1(data: self::BASE_DATA);
                $dup_p1->create();
            }
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
            }
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
        $p1->remote_gateway->value = "0.0.0.0";
        $p1->create();
        
        # Ensure multiple phase 1 entries can use this IP even if gw_duplicates is disabled
        $p1->gw_duplicates->value = true;
        $p1->update();
        $this->assert_does_not_throw(
            callable: function () {
                $dup_p1 = new IPsecPhase1(data: self::BASE_DATA);
                $dup_p1->remote_gateway->value = "0.0.0.0";
                $dup_p1->create();
                $dup_p1->delete();
            }
        );

        # Delete the IPsec phase 1 entry
        $p1->delete(apply: true);
    }
}
