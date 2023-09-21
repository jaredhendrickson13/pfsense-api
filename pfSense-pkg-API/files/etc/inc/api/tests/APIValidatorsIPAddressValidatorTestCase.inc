<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Validators\IPAddressValidator;
use API\Validators\NumericRangeValidator;

class APIValidatorsIPAddressValidatorTestCase extends TestCase {
    /**
     * Checks that validation fails if a value is not a valid IP address.
     */
    public function test_throw_response_for_non_ip_value() {
        $this->assert_throws_response(
            response_id: "IP_ADDRESS_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new IPAddressValidator();
                $test_validator->validate("not an IP!");
            }
        );
    }

    /**
     * Checks that validation fails if a value is not a valid IP address.
     */
    public function test_throw_response_for_non_ip_or_fqdn_value() {
        $this->assert_throws_response(
            response_id: "IP_ADDRESS_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new IPAddressValidator(allow_fqdn: true);
                $test_validator->validate("not an IP or FQDN!");
            }
        );
    }

    /**
     * Checks that IPv4 address validation works when allowed and fails when not allowed
     */
    public function test_ipv4_validation_value() {
        # Ensure IPv4 address validation works when allowed
        $this->assert_does_not_throw(
            callable: function () {
                $test_validator = new IPAddressValidator(allow_ipv4: true, allow_ipv6: false, allow_fqdn: false);
                $test_validator->validate("1.2.3.4");
            }
        );

        # Ensure IPv4 address validation fails when not allowed
        $this->assert_throws_response(
            response_id: "IP_ADDRESS_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new IPAddressValidator(allow_ipv4: false);
                $test_validator->validate("1.2.3.4");
            }
        );
    }

    /**
     * Checks that IPv6 address validation works when allowed and fails when not allowed.
     */
    public function test_ipv6_validation_value() {
        # Ensure IPv6 address validation works when allowed
        $this->assert_does_not_throw(
            callable: function () {
                $test_validator = new IPAddressValidator(allow_ipv4: false, allow_ipv6: true, allow_fqdn: false);
                $test_validator->validate("1234::1");
            }
        );

        # Ensure IPv6 address validation fails when not allowed
        $this->assert_throws_response(
            response_id: "IP_ADDRESS_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new IPAddressValidator(allow_ipv6: false);
                $test_validator->validate("1234::1");
            }
        );
    }

    /**
     * Checks that IPv6 link-local address validation works when allowed and fails when not allowed.
     */
    public function test_ipv6_link_local_validation_value() {
        # Ensure IPv6 link-local address validation works when allowed
        $this->assert_does_not_throw(
            callable: function () {
                $test_validator = new IPAddressValidator(allow_ipv6: true, allow_ipv6_link_local: true);
                $test_validator->validate("fe80::1");
            }
        );

        # Ensure IPv6 link-local address validation fails when not allowed
        $this->assert_throws_response(
            response_id: "IP_ADDRESS_VALIDATOR_IPV6_LINK_LOCAL_NOT_ALLOWED",
            code: 400,
            callable: function () {
                $test_validator = new IPAddressValidator(allow_ipv6: true, allow_ipv6_link_local: false);
                $test_validator->validate("fe80::1");
            }
        );
    }

    /**
     * Checks that values in `allow_keywords` are always considered valid
     */
    public function test_allow_keywords_validation() {
        # Ensure keyword validation works when allowed
        $this->assert_does_not_throw(
            callable: function () {
                $test_validator = new IPAddressValidator(
                    allow_ipv4: false,
                    allow_ipv6: false,
                    allow_fqdn: false,
                    allow_keywords: ["test"]
                );
                $test_validator->validate("test");
            }
        );

        # Ensure keyword validation fails when not allowed
        $this->assert_throws_response(
            response_id: "IP_ADDRESS_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new IPAddressValidator(
                    allow_ipv4: false,
                    allow_ipv6: false,
                    allow_fqdn: false,
                    allow_keywords: []
                );
                $test_validator->validate("test");
            }
        );
    }

    /**
     * Check that the IPAddressValidator does not throw any errors for values that are IP addresses.
     */
    public function test_ip_address_validator() {
        $this->assert_does_not_throw(
            function () {
                $test_validator = new IPAddressValidator(allow_ipv4: true, allow_ipv6:true, allow_fqdn: true);
                $test_validator->validate("1.2.3.4");
                $test_validator->validate("0::");
                $test_validator->validate("www.example.com");
            }
        );
    }

}
