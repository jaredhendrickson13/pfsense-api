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
     * Check that the IPAddressValidator does not throw any errors for values that are IP addresses.
     */
    public function test_ip_address_validator() {
        $this->assert_does_not_throw(
            function () {
                $test_validator = new IPAddressValidator(allow_fqdn: true);
                $test_validator->validate("1.2.3.4");
                $test_validator->validate("0::");
                $test_validator->validate("www.example.com");
            }
        );
    }

}
