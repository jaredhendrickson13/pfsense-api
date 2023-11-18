<?php

namespace api\validators;

require_once("api/auto_loader.inc");

use API;
use API\Responses\ValidationError;

class HostnameValidator extends API\Core\Validator {
    public array $allow_keywords;

    /**
     * Constructs the HostnameValidator with requested parameters.
     * @param array $allow_keywords An array of specified non-hostname keywords that should be considered valid.
     */
    public function __construct(array $allow_keywords = []) {
        $this->allow_keywords = $allow_keywords;
    }

    /**
     * Checks if a given value is a valid hostname.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws ValidationError When the value is not a valid hostname
     */
    public function validate(mixed $value, string $field_name = "") {
        # Throw a ValidationError if this value is not a hostname or an allowed keyword
        if (!is_hostname($value) and !in_array($value, $this->allow_keywords)) {
            throw new ValidationError(
                message: "Field `$field_name` must be a valid hostname, received `$value`.",
                response_id: "HOSTNAME_VALIDATOR_FAILED"
            );
        }
    }
}
