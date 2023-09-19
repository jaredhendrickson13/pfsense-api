<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Validators\IPAddressValidator;
use API\Validators\NumericRangeValidator;
use API\Validators\SubnetValidator;

class APIValidatorsSubnetValidatorTestCase extends TestCase {
    /**
     * Checks that IPv4 subnets are correctly validated
     */
    public function test_ipv4_subnet_validation() {
        # Ensure an error is thrown when the IPv4 subnet is invalid
        $this->assert_throws_response(
            response_id: "SUBNET_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new SubnetValidator(allow_ipv4: true, allow_ipv6: false);
                $test_validator->validate("not an IPv4!");
            }
        );

        # Ensure an error is thrown when the IPv4 subnet is actually and IPv6 subnet
        $this->assert_throws_response(
            response_id: "SUBNET_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new SubnetValidator(allow_ipv4: true, allow_ipv6: false);
                $test_validator->validate("1234::/128");
            }
        );

        # Ensure an error is not thrown when the IPv4 subnet is valid
        $this->assert_does_not_throw(
            callable: function () {
                $test_validator = new SubnetValidator(allow_ipv4: true, allow_ipv6: false);
                $test_validator->validate("127.0.0.1/24");
            }
        );
    }

    /**
     * Checks that IPv6 subnets are correctly validated
     */
    public function test_ipv6_subnet_validation() {
        # Ensure an error is thrown when the IPv4 subnet is invalid
        $this->assert_throws_response(
            response_id: "SUBNET_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new SubnetValidator(allow_ipv4: false, allow_ipv6: true);
                $test_validator->validate("not an IPv6!");
            }
        );

        # Ensure an error is thrown when the IPv6 subnet is actually and IPv4 subnet
        $this->assert_throws_response(
            response_id: "SUBNET_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $test_validator = new SubnetValidator(allow_ipv4: false, allow_ipv6: true);
                $test_validator->validate("127.0.0.1/24");
            }
        );

        # Ensure an error is not thrown when the IPv6 subnet is valid
        $this->assert_does_not_throw(
            callable: function () {
                $test_validator = new SubnetValidator(allow_ipv4: false, allow_ipv6: true);
                $test_validator->validate("1234::/128");
            }
        );
    }

    /**
     * Check that the SubnetValidator does not throw any errors for values that are subnet CIDRs.
     */
    public function test_subnet_validator() {
        $this->assert_does_not_throw(
            function () {
                $test_validator = new SubnetValidator(allow_ipv4: true, allow_ipv6: true, allow_keywords: ["test"]);
                $test_validator->validate("1.2.3.4/24");
                $test_validator->validate("0::/128");
                $test_validator->validate("test");
            }
        );
    }
}
