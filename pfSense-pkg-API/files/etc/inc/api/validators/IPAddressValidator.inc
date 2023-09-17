<?php

namespace API\Validators;

require_once("api/auto_loader.inc");

use API;
use API\Responses\ValidationError;

class IPAddressValidator extends API\Core\Validator {
    public bool $allow_ipv4;
    public bool $allow_ipv6;
    public bool $allow_fqdn;
    public array $allow_keywords;

    /**
     * @param bool $allow_ipv4 Allow value to be an IPv4 address
     * @param bool $allow_ipv6 Allow value to be an IPv6 address
     * @param bool $allow_fqdn Allow value to be an FQDN
     * @param array $allow_keywords An array of non IP/FQDN values to allow
     */
    public function __construct(
        bool $allow_ipv4 = true, bool $allow_ipv6 = true, bool $allow_fqdn = false, array $allow_keywords = []
    ) {
        $this->allow_ipv4 = $allow_ipv4;
        $this->allow_ipv6 = $allow_ipv6;
        $this->allow_fqdn = $allow_fqdn;
        $this->allow_keywords = $allow_keywords;
    }

    /**
     * Checks if a given value is a valid IP address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Responses\ValidationError When the value is not a valid IPv4 address.
     */
    public function validate(mixed $value, string $field_name = "") {
        # IPv4 values are valid if allowed
        if ($this->allow_ipv4 and is_ipaddrv4($value)) {
            return;
        }
        # IPv6 values are valid if allowed
        if ($this->allow_ipv4 and is_ipaddrv6($value)) {
            return;
        }
        # FQDN values are valid if allowed
        if ($this->allow_fqdn and is_fqdn($value)) {
            return;
        }
        # Special keywords are valid if defined
        if (in_array($value, $this->allow_keywords)) {
            return;
        }

        # If the value was not allowed from an earlier statement, throw a validation error
        throw new ValidationError(
            message: "Field `$field_name` must be a valid IP address, received `$value`.",
            response_id: "IP_ADDRESS_VALIDATOR_FAILED"
        );
    }
}
