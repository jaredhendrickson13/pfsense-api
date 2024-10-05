<?php

namespace RESTAPI\Validators;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI;
use RESTAPI\Core\Validator;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a Validator that checks if a given value is a valid URL
 */
class URLValidator extends Validator {
    /**
     * @var array $allow_keywords An array of specified non-URL keywords that should be considered valid.
     */
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
    public function validate(mixed $value, string $field_name = ''): void {
        # Allow this value if it matches an allowed keyword exactly
        if (in_array($value, $this->allow_keywords)) {
            return;
        }

        # Accept URL values
        if (filter_var($value, FILTER_VALIDATE_URL)) {
            return;
        }

        # Otherwise throw a ValidationError, this value is not valid
        throw new ValidationError(
            message: "Field `$field_name` must be a valid URL, received `$value`.",
            response_id: 'URL_VALIDATOR_FAILED',
        );
    }
}
