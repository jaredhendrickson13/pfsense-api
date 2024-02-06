<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\HexValidator;

class APIValidatorsHexValidatorTestCase extends TestCase {
    /**
     * Checks that the $expected_length property is always twice the $bytes length.
     */
    public function test_hex_value_must_match_bytes_length() {
        # Ensure hex values matching this bytes length are allowed
        $this->assert_does_not_throw(function () {
            # Create a HexValidator to test with
            $hex_validator = new HexValidator(16);
            $good_hex_value = bin2hex(openssl_random_pseudo_bytes(16));
            $hex_validator->validate($good_hex_value);
        });

        # Ensure hex values that do not match this bytes length are not allowed
        $this->assert_throws_response(
            response_id: 'HEX_VALIDATOR_VALUE_IS_NOT_EXPECTED_LENGTH',
            code: 400,
            callable: function () {
                # Create a HexValidator to test with
                $hex_validator = new HexValidator(16);
                $bad_hex_value = bin2hex(openssl_random_pseudo_bytes(12));
                $hex_validator->validate($bad_hex_value);
            },
        );
    }

    /**
     * Checks that the value must only contain hex characters.
     */
    public function test_hex_value_must_only_contain_hex_chars() {
        # Ensure hex values are allowed
        $this->assert_does_not_throw(function () {
            # Create a HexValidator to test with
            $hex_validator = new HexValidator(16);
            $good_hex_value = bin2hex(openssl_random_pseudo_bytes(16));
            $hex_validator->validate($good_hex_value);
        });

        # Ensure values that do not contain all hex characters are not allowed
        $this->assert_throws_response(
            response_id: 'HEX_VALIDATOR_VALUE_CONTAINS_NOT_HEX_CHARS',
            code: 400,
            callable: function () {
                # Create a HexValidator to test with
                $hex_validator = new HexValidator(16);
                $bad_hex_value = 'not a hex string!';
                $hex_validator->validate($bad_hex_value);
            },
        );

        # Ensure the hex prefix is allowed if enabled
        $this->assert_does_not_throw(function () {
            # Create a HexValidator to test with
            $hex_validator = new HexValidator(bytes: 16, allow_hex_prefix: true);
            $good_hex_value = HexValidator::HEX_PREFIX . bin2hex(openssl_random_pseudo_bytes(16));
            $hex_validator->validate($good_hex_value);
        });
        # Ensure the hex prefix is not allowed if disabled
        $this->assert_throws_response(
            response_id: 'HEX_VALIDATOR_VALUE_CONTAINS_NOT_HEX_CHARS',
            code: 400,
            callable: function () {
                # Create a HexValidator to test with
                $hex_validator = new HexValidator(bytes: 16, allow_hex_prefix: false);
                $bad_hex_value = HexValidator::HEX_PREFIX . bin2hex(openssl_random_pseudo_bytes(16));
                $hex_validator->validate($bad_hex_value);
            },
        );
    }
}
