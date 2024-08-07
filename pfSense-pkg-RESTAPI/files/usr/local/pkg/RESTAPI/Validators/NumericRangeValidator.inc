<?php

namespace RESTAPI\Validators;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI;
use RESTAPI\Responses\ServerError;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a Validator that checks if a given value is within a specified numeric range.
 */
class NumericRangeValidator extends RESTAPI\Core\Validator {
    public int|float $minimum;
    public int|float $maximum;

    /**
     * Assigns the threshold for max length validation.
     * @param int $minimum The minimum value this value can be.
     * @param int $maximum The maximum value this value can be..
     */
    public function __construct(int $minimum = 0, int $maximum = 99999999999) {
        $this->minimum = $minimum;
        $this->maximum = $maximum;
    }

    /**
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws ValidationError When the value exceeds the maximum length specified.
     * @throws ServerError When the value was not a string or an array.
     */
    public function validate(mixed $value, string $field_name = '') {
        # Ensure the value being validated is either an integer or a float (double)
        if (!is_integer($value) and !is_float($value)) {
            throw new ServerError(
                message: 'NumericRangeValidator can only be used on integer or float values.',
                response_id: 'NUMERIC_RANGE_VALIDATOR_UNSUPPORTED_TYPE',
            );
        }

        # Throw a validation error if the value is less than the minimum value
        if ($value < $this->minimum) {
            throw new ValidationError(
                message: "Field '$field_name' must be greater than or equal to '$this->minimum'. Received '$value'",
                response_id: 'NUMERIC_RANGE_VALIDATOR_MINIMUM_CONSTRAINT',
            );
        }

        # Throw a validation error if the value is greater than the maximum value
        if ($value > $this->maximum) {
            throw new ValidationError(
                message: "Field '$field_name' must be less than or equal to '$this->maximum'. Received '$value'",
                response_id: 'NUMERIC_RANGE_VALIDATOR_MAXIMUM_CONSTRAINT',
            );
        }
    }
}
