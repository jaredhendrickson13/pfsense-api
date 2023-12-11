<?php

namespace RESTAPI\Validators;

require_once("RESTAPI/autoloader.inc");

use RESTAPI;
use RESTAPI\Responses\ValidationError;

class HostnameValidator extends RESTAPI\Core\Validator {
    public bool $allow_hostname;
    public bool $allow_domain;
    public bool $allow_fqdn;
    public bool $allow_uqdn;
    public array $allow_keywords;

    /**
     * Constructs the HostnameValidator with requested parameters.
     * @param bool $allow_hostname Allow values that are only the hostname portion of an FQDN.
     * @param bool $allow_domain Allow values that are only the domain portion of an FQDN.
     * @param bool $allow_fqdn Allow values that are a full FQDN.
     * @param array $allow_keywords An array of specified non-hostname keywords that should be considered valid.
     */
    public function __construct(
        bool $allow_hostname = true,
        bool $allow_domain = false,
        bool $allow_fqdn = false,
        array $allow_keywords = []
    ) {
        $this->allow_hostname = $allow_hostname;
        $this->allow_domain = $allow_domain;
        $this->allow_fqdn = $allow_fqdn;
        $this->allow_keywords = $allow_keywords;
    }

    /**
     * Checks if a given value is a valid hostname.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws ValidationError When the value is not a valid hostname
     */
    public function validate(mixed $value, string $field_name = "") {
        # Allow this value if it matches an allowed keyword exactly
        if (in_array($value, $this->allow_keywords)) {
            return;
        }

        # Accept hostname values if hostname values are allowed
        if ($this->allow_hostname and is_hostname($value)) {
            return;
        }

        # Accept domain values if domain values are allowed
        if ($this->allow_domain and is_domain($value)) {
            return;
        }

        # Accept FQDN values if hostname values are allowed
        if ($this->allow_fqdn and is_fqdn($value)) {
            return;
        }

        # Otherwise throw a ValidationError, this value is not valid
        throw new ValidationError(
            message: "Field `$field_name` must be a valid hostname, received `$value`.",
            response_id: "HOSTNAME_VALIDATOR_FAILED"
        );
    }
}
