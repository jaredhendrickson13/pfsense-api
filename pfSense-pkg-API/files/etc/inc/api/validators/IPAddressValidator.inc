<?php

namespace API\Validators;

require_once("api/core/Validator.inc");

use API;

class IPAddressValidator extends API\Core\Validator {
    public bool $allow_fqdn;

    public function __construct(bool $allow_fqdn = false) {
        $this->allow_fqdn = $allow_fqdn;
    }

    /**
     * Checks if a given value is a valid IP address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Core\ValidationError When the value is not a valid IPv4 address.
     */
    public function validate(mixed $value, string $field_name = "") {
        # Allow IP addresses
        if (is_ipaddr($value)) {
            return;
        }
        # Allow FQDNs if enabled
        if ($this->allow_fqdn and is_fqdn($value)) {
            return;
        }

        # If the value was not allowed from an earlier statement, throw a validation error
        throw new API\Core\ValidationError(
            message: "Field `$field_name` must be a valid IP address, received `$value`.",
            response_id: "IP_ADDRESS_VALIDATOR_FAILED"
        );
    }
}
