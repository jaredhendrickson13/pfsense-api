<?php

namespace RESTAPI\Validators;

use RESTAPI\Core\Validator;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a validator class that determines if a given field value is a valid email address.
 */
class EmailAddressValidator extends Validator {
    /**
     * Checks if a given value is a valid email address.
     * @param mixed $value The incoming value to be validated.
     * @param string $field_name The name of the field being validated.
     */
    public function validate(mixed $value, string $field_name = ''): void {
        if (!filter_var($value, FILTER_VALIDATE_EMAIL)) {
            throw new ValidationError(
                message: "Field `$field_name` must be a valid email address.",
                response_id: 'EMAIL_ADDRESS_VALIDATOR_FAILED',
            );
        }
    }
}
