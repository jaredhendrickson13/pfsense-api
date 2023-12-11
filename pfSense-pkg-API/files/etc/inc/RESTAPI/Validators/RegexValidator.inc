<?php

namespace RESTAPI\Validators;

use RESTAPI\Core\Validator;
use RESTAPI\Responses\ValidationError;

class RegexValidator extends Validator
{
    public string $pattern;
    public bool $invert;
    public string $error_msg;

    /**
     * Constructs the Validator object.
     * @param string $pattern The regex pattern to validate the value against.
     * @param bool $invert Invert the context of the pattern. If `true`, any value that does NOT match the pattern
     * will be considered valid.
     * @param string $error_msg An optional error message to include when validation fails. This helps provide context
     * to the regex pattern.
     */
    public function __construct(string $pattern, bool $invert = false, string $error_msg = "")
    {
        $this->pattern = $pattern;
        $this->invert = $invert;
        $this->error_msg = ($error_msg) ?: "Value must match pattern: `$pattern`.";
    }

    /**
     * Validates a specified value against our regex pattern.
     * @param mixed $value The incoming value to be validated.
     * @param string $field_name The name of the field being validated.
     */
    public function validate(mixed $value, string $field_name = "")
    {
        # Consider this value invalid if the context is not inverted and the value does not match the pattern
        if (!$this->invert and !preg_match(pattern: $this->pattern, subject: $value)) {
            throw new ValidationError(
                message: "Field `$field_name` failed regex validation. $this->error_msg",
                response_id: "REGEX_VALIDATOR_FAILED"
            );
        }
        # Consider this value invalid if the context is inverted and the value does match the pattern
        if ($this->invert and preg_match(pattern: $this->pattern, subject: $value)) {
            throw new ValidationError(
                message: "Field `$field_name` failed regex validation. $this->error_msg",
                response_id: "REGEX_VALIDATOR_FAILED"
            );
        }
    }
}
