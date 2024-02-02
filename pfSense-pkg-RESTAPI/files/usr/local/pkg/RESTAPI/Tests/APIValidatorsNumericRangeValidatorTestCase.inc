<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\NumericRangeValidator;

class APIValidatorsNumericRangeValidatorTestCase extends TestCase
{
    /**
     * Check that the validator throws a server error if the value being validated is not an integer or a float.
     */
    public function test_throws_server_error_for_non_int_float()
    {
        $this->assert_throws_response(
            response_id: 'NUMERIC_RANGE_VALIDATOR_UNSUPPORTED_TYPE',
            code: 500,
            callable: function () {
                $test_validator = new NumericRangeValidator();
                $test_validator->validate('not an integer or float');
            },
        );
    }

    /**
     * Checks that the NumericRangeValidator throws a validation error if the value is less than the minimum
     * parameter.
     */
    public function test_throws_validation_error_for_less_than_minimum()
    {
        $this->assert_throws_response(
            response_id: 'NUMERIC_RANGE_VALIDATOR_MINIMUM_CONSTRAINT',
            code: 400,
            callable: function () {
                $test_validator = new NumericRangeValidator(minimum: 5);
                $test_validator->validate(0);
            },
        );
    }

    /**
     * Checks that the NumericRangeValidator throws a validation error if the value is less than the minimum
     * parameter.
     */
    public function test_throws_validation_error_for_greater_than_maximum()
    {
        $this->assert_throws_response(
            response_id: 'NUMERIC_RANGE_VALIDATOR_MAXIMUM_CONSTRAINT',
            code: 400,
            callable: function () {
                $test_validator = new NumericRangeValidator(maximum: 5);
                $test_validator->validate(10);
            },
        );
    }

    /**
     * Check that the NumericRangeValidator does not throw any errors for values that are within the range.
     */
    public function test_numeric_range_validator()
    {
        # Set validator with a specified range and ensure values within the range do not throw errors.
        $test_validator = new NumericRangeValidator(minimum: 10, maximum: 100);
        $test_validator->minimum = 10;
        $test_validator->maximum = 100;
        $test_validator->validate(10);
        $test_validator->validate(15);
        $test_validator->validate(95);
        $test_validator->validate(100);
    }
}
