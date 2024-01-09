<?php

namespace RESTAPI\Validators;

use RESTAPI\Core\Validator;
use RESTAPI\Responses\ValidationError;

class RegexValidator extends Validator
{
    /**
     * Constructs the Validator object.
     * @param string $pattern The regex pattern to validate the value against.
     * @param bool $invert Invert the context of the pattern. If `true`, any value that does NOT match the pattern
     * will be considered valid. This does not invert the context of $allowed_keywords.
     * @param array $allowed_keywords An array of values that should be allowed even if they do not match the pattern.
     * @param string $error_msg An optional error message to include when validation fails. This helps provide context
     * to the regex pattern.
     */
    public function __construct(
        public string $pattern, 
        public bool $invert = false, 
        public array $allowed_keywords = [], 
        public string $error_msg = ""
    )
    {
        # If no custom error message was given, at least provide the regex pattern
        $invert_context_str = ($this->invert) ? "not match" : "match";
        $this->error_msg = ($this->error_msg) ?: "Value must $invert_context_str pattern: `$this->pattern`.";
    }

    /**
     * Validates a specified value against our regex pattern.
     * @param mixed $value The incoming value to be validated.
     * @param string $field_name The name of the field being validated.
     */
    public function validate(mixed $value, string $field_name = ""): void
    {
        # If this value is an allowed keyword, allow it
        if (in_array($value, $this->allowed_keywords)) {
            return;
        }
        
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
