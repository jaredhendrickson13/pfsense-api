<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\MACAddressValidator;

class APIValidatorsMACAddressValidatorTestCase extends TestCase {
    /**
     * Checks that validation fails if a value is not a valid MAC address.
     */
    public function test_throw_response_for_non_mac_value() {
        $this->assert_throws_response(
            response_id: 'MAC_ADDRESS_VALIDATOR_FAILED',
            code: 400,
            callable: function () {
                $test_validator = new MACAddressValidator();
                $test_validator->validate('not a MAC!');
            },
        );
    }

    /**
     * Check that the MACAddressValidator does not throw any errors for values that are MAC addresses.
     */
    public function test_mac_address_validator() {
        $this->assert_does_not_throw(function () {
            $test_validator = new MACAddressValidator();
            $test_validator->validate('00:13:10:00:00:00');
            $test_validator->validate('00:13:10:00:00:0a');
            $test_validator->validate('00:13:10:00:00:0A');
        });
    }
}
