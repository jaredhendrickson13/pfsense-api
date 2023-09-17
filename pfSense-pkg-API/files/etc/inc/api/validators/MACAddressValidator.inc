<?php

namespace API\Validators;

require_once("api/auto_loader.inc");

use API;
use API\Responses\ValidationError;

class MACAddressValidator extends API\Core\Validator {
    /**
     * Checks if a given value is a valid MAC address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Responses\ValidationError When the value is not a valid MAC address.
     */
    public function validate(mixed $value, string $field_name = "") {
        # Throw a ValidationError if this value is not a MAC address
        if (!is_macaddr($value)) {
            throw new ValidationError(
                message: "Field `$field_name` must be a valid MAC address, received `$value`.",
                response_id: "MAC_ADDRESS_VALIDATOR_FAILED"
            );
        }
    }
}
