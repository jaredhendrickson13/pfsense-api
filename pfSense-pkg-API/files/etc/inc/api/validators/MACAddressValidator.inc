<?php

namespace API\Validators;

require_once("api/auto_loader.inc");

use API;
use API\Responses\ValidationError;

class MACAddressValidator extends API\Core\Validator {
    public array $allow_keywords;

    /**
     * Constructs the MACAddressValidator with requested parameters.
     * @param array $allow_keywords An array of specified non-MAC address keywords that should be considered valid.
     */
    public function __construct(array $allow_keywords = []) {
        $this->allow_keywords = $allow_keywords;
    }

    /**
     * Checks if a given value is a valid MAC address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Responses\ValidationError When the value is not a valid MAC address.
     */
    public function validate(mixed $value, string $field_name = "") {
        # Throw a ValidationError if this value is not a MAC address or an allowed keyword
        if (!is_macaddr($value) and !in_array($value, $this->allow_keywords)) {
            throw new ValidationError(
                message: "Field `$field_name` must be a valid MAC address, received `$value`.",
                response_id: "MAC_ADDRESS_VALIDATOR_FAILED"
            );
        }
    }
}
