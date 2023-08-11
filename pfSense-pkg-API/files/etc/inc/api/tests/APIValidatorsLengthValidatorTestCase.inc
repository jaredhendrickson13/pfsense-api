<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Validators\LengthValidator;

class APIValidatorsLengthValidatorTestCase extends TestCase {
    /**
     * Check that MaximumLengthValidator throws a server error if a non-string or non-array value is passed to the
     * `validate()` method.
     */
    public function test_throws_server_error_for_unsupported_type() {
        $this->assert_throws_response(
            response_id: "LENGTH_VALIDATOR_UNSUPPORTED_TYPE",
            code: 500,
            callable: function () {
                $test_validator = new LengthValidator(minimum: 0, maximum: 120);
                $test_validator->validate(0);
            }
        );
    }

    /**
     * Checks that the LengthValidator throws a validation error if a string has less chars than the minimum required.
     */
    public function test_string_minimum_constraint() {
        $this->assert_throws_response(
            response_id: "LENGTH_VALIDATOR_MINIMUM_CONSTRAINT",
            code: 400,
            callable: function () {
                $test_validator = new LengthValidator(minimum: 50, maximum: 120);
                $test_validator->validate("Not enough characters!");
            }
        );
    }

    /**
     * Checks that the LengthValidator throws a validation error if a string has more chars than the maximum allowed.
     */
    public function test_string_maximum_constraint() {
        $this->assert_throws_response(
            response_id: "LENGTH_VALIDATOR_MAXIMUM_CONSTRAINT",
            code: 400,
            callable: function () {
                $test_validator = new LengthValidator(minimum: 0, maximum: 5);
                $test_validator->validate("Too many characters");
            }
        );
    }

    /**
     * Checks that the LengthValidator throws a validation error if an array has less entries than the minimum required.
     */
    public function test_array_minimum_constraint() {
        $this->assert_throws_response(
            response_id: "LENGTH_VALIDATOR_MINIMUM_CONSTRAINT",
            code: 400,
            callable: function () {
                $test_validator = new LengthValidator(minimum: 10, maximum: 120);
                $test_validator->validate(["not", "enough", "entries!"]);
            }
        );
    }

    /**
     * Checks that the LengthValidator throws a validation error if an array has more entries than the maximum allowed.
     */
    public function test_array_maximum_constraint() {
        $this->assert_throws_response(
            response_id: "LENGTH_VALIDATOR_MAXIMUM_CONSTRAINT",
            code: 400,
            callable: function () {
                $test_validator = new LengthValidator(minimum: 0, maximum: 3);
                $test_validator->validate(["too", "many", "items", "!"]);
            }
        );
    }

    /**
     * Check that the NumericRangeValidator does not throw any errors for values that are within the range.
     */
    public function test_maximum_length_validator() {
        # Set validator with a specified range and ensure values within the range do not throw errors.
        $test_validator = new LengthValidator(minimum: 0, maximum: 5);
        $test_validator->validate("");
        $test_validator->validate("123");
        $test_validator->validate("12345");
        $test_validator->validate([]);
        $test_validator->validate(["1", "2", "3"]);
        $test_validator->validate(["1", "2", "3", "4", "5"]);
    }

}
