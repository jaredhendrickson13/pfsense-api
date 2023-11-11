<?php

namespace api\validators;

use API\Core\Validator;
use API\Responses\ValidationError;

class RegexValidator extends Validator
{
    public string $pattern;
    public string $error_msg;

    /**
     * Constructs the Validator object.
     * @param string $pattern The regex pattern to validate the value against.
     * @param string $error_msg An optional error message to include when validation fails. This helps provide context
     * to the regex pattern.
     */
    public function __construct(string $pattern, string $error_msg = "")
    {
        $this->pattern = $pattern;
        $this->error_msg = $error_msg;
    }

    /**
     * Validates a specified value against our regex pattern.
     * @param mixed $value The incoming value to be validated.
     * @param string $field_name The name of the field being validated.
     */
    public function validate(mixed $value, string $field_name = "")
    {
        if (preg_match(pattern: $this->pattern, subject: $value)) {
            throw new ValidationError(
                message: "Field `$field_name` failed regex validation. $this->error_msg.",
                response_id: "REGEX_VALIDATOR_FAILED"
            );
        }
    }
}
