<?php

namespace API\Validators;

use API;

class MaxLengthValidator extends API\Core\Validator {
    public int $max_length;

    /**
     * Assigns the threshold for max length validation.
     * @param int $max_length The maximum length allowed by the value.
     */
    public function __construct(int $max_length) {
        $this->max_length = $max_length;
    }

    /**
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Core\ValidationError When the value exceeds the maximum length specified.
     * @throws API\Core\ServerError When the value was not a string or an array.
     */
    public function validate(mixed $value, string $field_name = "") {
        # For strings, throw an error if the character length of the value exceeds the max length
        if (is_string($value)) {
            if (strlen($value) > $this->max_length) {
                throw new API\Core\ValidationError(
                    message: "Field '$field_name' exceeds the maximum character length of $this->max_length.",
                    return: "EXCEEDS_MAX_LENGTH"
                );
            }
        }
        # For arrays, throw an error if the number of items in the value exceeds the max length
        elseif (is_array($value)) {
            if (count($value) > $this->max_length) {
                throw new API\Core\ValidationError(
                    message: "Field '$field_name' exceeds the maximum array length of $this->max_length.",
                    return: "EXCEEDS_MAX_LENGTH"
                );
            }
        }
        # Throw an error if the value was not a string or an array
        else {
            throw new API\Core\ServerError(
                message: "Field '$field_name' request max length validation but is not a string or an array.",
                return: "EXCEEDS_MAX_LENGTH"
            );
        }
    }
}
