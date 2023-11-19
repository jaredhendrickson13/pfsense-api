<?php

namespace API\Validators;

require_once("api/auto_loader.inc");

use API;
use API\Responses\ServerError;
use API\Responses\ValidationError;

class LengthValidator extends API\Core\Validator {
    public int $minimum;
    public int $maximum;

    /**
     * Assigns the threshold for length validation.
     * @param int $minimum The minimum length allowed by the value.
     * @param int $maximum The maximum length allowed by the value.
     */
    public function __construct(int $minimum, int $maximum) {
        $this->minimum = $minimum;
        $this->maximum = $maximum;
    }

    /**
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws ValidationError When the value is less than the minimum length requirement OR is greather than
     * the maximum length requirement.
     * @throws API\Responses\ServerError When the value was not a string or an array.
     */
    public function validate(mixed $value, string $field_name = "") {
        # Perform specific validation for string values
        if (is_string($value)) {
            # Throw an error if the string contains less characters than the specified minimum
            if (strlen($value) < $this->minimum) {
                throw new ValidationError(
                    message: "Field '$field_name' must contain a minimum of $this->maximum characters.",
                    response_id: "LENGTH_VALIDATOR_MINIMUM_CONSTRAINT"
                );
            }
            # Throw an error if the string contains more characters than the specified maximum
            if (strlen($value) > $this->maximum) {
                throw new ValidationError(
                    message: "Field '$field_name' exceeds the maximum character length of $this->maximum.",
                    response_id: "LENGTH_VALIDATOR_MAXIMUM_CONSTRAINT"
                );
            }
        }
        # Perform specific validation for array values
        elseif (is_array($value)) {
            # Throw an error if the array contains more array entries than the specified maximum
            if (count($value) < $this->minimum) {
                throw new ValidationError(
                    message: "Field '$field_name' must contain a minimum of $this->minimum entries.",
                    response_id: "LENGTH_VALIDATOR_MINIMUM_CONSTRAINT"
                );
            }
            # Throw an error if the array contains more array entries than the specified maximum
            if (count($value) > $this->maximum) {
                throw new ValidationError(
                    message: "Field '$field_name' exceeds the maximum array length of $this->maximum.",
                    response_id: "LENGTH_VALIDATOR_MAXIMUM_CONSTRAINT"
                );
            }
        }
        # Throw an error if the value was not a string or an array
        else {
            throw new ServerError(
                message: "Field '$field_name' requested max length validation but is not a string or an array.",
                response_id: "LENGTH_VALIDATOR_UNSUPPORTED_TYPE"
            );
        }
    }
}