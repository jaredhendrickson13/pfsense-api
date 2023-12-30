<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
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
        $p1->peerid_type->value = "any";
        $p1->startaction->value = "none";
        $p1->create();
        
        # Ensure multiple phase 1 entries can use this IP even if gw_duplicates is disabled
        $p1->gw_duplicates->value = true;
        $p1->update();
        $this->assert_does_not_throw(
            callable: function () {
                $dup_p1 = new IPsecPhase1(data: self::BASE_DATA);
                $dup_p1->remote_gateway->value = "0.0.0.0";
                $dup_p1->peerid_type->value = "any";
                $dup_p1->startaction->value = "none";
                $dup_p1->create();
                $dup_p1->delete();
            }
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
            response_id: "IPSEC_PHASE_1_MYID_DATA_MUST_BE_IP_ADDRESS",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "address";
                $p1->validate_myid_data("not an IP address!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "address";
                $p1->validate_myid_data("1.2.3.4");
            }
        );

        # Ensure `myid_type` value `fqdn` must be an FQDN
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_MYID_DATA_MUST_BE_FQDN",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "fqdn";
                $p1->validate_myid_data("not an FQDN!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "fqdn";
                $p1->validate_myid_data("example.com");
            }
        );

        # Ensure `myid_type` value `user_fqdn` must be an email address
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_MYID_DATA_MUST_BE_EMAIL_ADDRESS",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "user_fqdn";
                $p1->validate_myid_data("not an email!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "user_fqdn";
                $p1->validate_myid_data("test@example.com");
            }
        );

        # Ensure `myid_type` value `dyn_dns` must be an FQDN
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_MYID_DATA_MUST_BE_FQDN",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "dyn_dns";
                $p1->validate_myid_data("not an FQDN!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->myid_type->value = "dyn_dns";
                $p1->validate_myid_data("example.com");
            }
        );
    }

    /**
     * Checks that the `peerid_type` cannot be set to `peeraddress` when the `remote_gateway` is an INADDR_ANY IP
     */
    public function test_peerid_type_cannot_be_peeraddress_with_inaddr_any_remote_gateway(): void {
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_NO_INADDR_ANY_WITH_PEERADDRESS",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->remote_gateway->value = "0.0.0.0";
                $p1->validate_peerid_type("peeraddress");
            }
        );
    }

    /**
     * Checks that the `peerid_data` field correctly enforces input validation depending on the `peerid_type`.
     */
    public function test_validate_peerid_data(): void {
        # Ensure `peerid_type` value `address` must be IP address
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_PEERID_DATA_MUST_BE_IP_ADDRESS",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "address";
                $p1->validate_peerid_data("not an IP address!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "address";
                $p1->validate_peerid_data("1.2.3.4");
            }
        );

        # Ensure `peerid_type` value `fqdn` must be an FQDN
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_PEERID_DATA_MUST_BE_FQDN",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "fqdn";
                $p1->validate_peerid_data("not an FQDN!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "fqdn";
                $p1->validate_peerid_data("example.com");
            }
        );

        # Ensure `peerid_type` value `user_fqdn` must be an email address
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_PEERID_DATA_MUST_BE_EMAIL_ADDRESS",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "user_fqdn";
                $p1->validate_peerid_data("not an email!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "user_fqdn";
                $p1->validate_peerid_data("test@example.com");
            }
        );

        # Ensure `peerid_type` value `dyn_dns` must be an FQDN
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_PEERID_DATA_MUST_BE_FQDN",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "dyn_dns";
                $p1->validate_peerid_data("not an FQDN!");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->peerid_type->value = "dyn_dns";
                $p1->validate_peerid_data("example.com");
            }
        );
    }

    /**
     * Checks that the `rekey_time` cannot be set to a value higher than `lifetime`
     */
    public function test_rekey_time_cannot_be_greater_than_lifetime(): void {
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_REKEY_TIME_GREATER_THAN_LIFETIME",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->lifetime->value = 10000;
                $p1->validate_rekey_time(10001);
            }
        );
    }

    /**
     * Checks that the `reauth_time` cannot be set to a value higher than `lifetime`
     */
    public function test_reauth_time_cannot_be_greater_than_lifetime(): void {
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_REAUTH_TIME_GREATER_THAN_LIFETIME",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->lifetime->value = 10000;
                $p1->validate_reauth_time(10001);
            }
        );
    }

    /**
     * Checks that the `startaction` must be set to `none` when `remote_gateway` is an INADDR_ANY IP
     */
    public function test_startaction_must_be_none_with_inaddr_any_remote_gateway(): void {
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_NO_STARTACTION_WITH_INADDR_ANY_REMOTE_GATEWAY",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->remote_gateway->value = "0.0.0.0";
                $p1->validate_startaction("start");
            }
        );
    }

    /**
     * Checks that the `nattport` cannot use the same port as `ikeport`
     */
    public function test_nattport_cannot_match_ikeport(): void {
        $this->assert_throws_response(
            response_id: "IPSEC_PHASE_1_NATTPORT_CANNOT_MATCH_IKEPORT",
            code: 400,
            callable: function () {
                $p1 = new IPsecPhase1();
                $p1->ikeport->value = "500";
                $p1->validate_nattport("500");
            }
        );
    }

    /**
     * Checks that the `iketype` is correctly represented on the backend after applying.
     */
    public function test_iketype_found_in_swanctl(): void {
        # Create a new IPsec phase 1 and ensure
        $p1 = new IPsecPhase1(data: self::BASE_DATA, async: false);
        $p1->iketype->value = "ikev2";
        $p1->create(apply: true);

        # Ensure the `iketype` is correctly shown in swanctl
        $swanctl_conf = file_get_contents("/var/etc/ipsec/swanctl.conf");
        $this->assert_str_contains(
            $swanctl_conf,
            "version = 2"
        );

        # Update the p1 entry to IKEv1 and ensure it is correctly shown
        $p1->iketype->value = "ikev1";
        $p1->mode->value = "main";
        $p1->update(apply: true);
        $swanctl_conf = file_get_contents("/var/etc/ipsec/swanctl.conf");
        $this->assert_str_contains(
            $swanctl_conf,
            "version = 1"
        );

        # Delete the p1 entry
        $p1->delete(apply: true);
    }
}
