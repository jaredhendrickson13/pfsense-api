<?php

namespace RESTAPI\Validators;

require_once("RESTAPI/autoloader.inc");

use RESTAPI;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Responses\ValidationError;

/**
 * Creates a validator object used to check if a given value is a valid IPv4 or IPv6 subnet CIDR
 */
class SubnetValidator extends RESTAPI\Core\Validator {
    public bool $allow_ipv4;
    public bool $allow_ipv6;
    public bool $allow_alias;
    public array $allow_keywords;

    /**
     * @param bool $allow_ipv4 Allow value to be an IPv4 subnet CIDR
     * @param bool $allow_ipv6 Allow value to be an IPv6 subnet CIDR
     * @param array $allow_keywords An array of non subnet CIDR values to allow
     */
    public function __construct(
        bool $allow_ipv4 = true,
        bool $allow_ipv6 = true,
        bool $allow_alias = false,
        array $allow_keywords = []
    ) {
        $this->allow_ipv4 = $allow_ipv4;
        $this->allow_ipv6 = $allow_ipv6;
        $this->allow_alias = $allow_alias;
        $this->allow_keywords = $allow_keywords;
    }

    /**
     * Checks if a given value is a valid subnet CIDR.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws RESTAPI\Responses\ValidationError When the value is not a valid subnet CIDR.
     */
    public function validate(mixed $value, string $field_name = "") {
        # Non-port alias values are valid if allowed
        $alias_query = FirewallAlias::query(["name" => $value, "type__except" => "port"]);
        if ($this->allow_alias and $alias_query->exists()) {
            $this->set_label("is_alias");
            return;
        }
        # IPv4 values are valid if allowed
        if ($this->allow_ipv4 and is_subnetv4($value)) {
            $this->set_label("is_subnetv4");
            return;
        }
        # IPv6 values are valid if allowed
        if ($this->allow_ipv6 and is_subnetv6($value)) {
            $this->set_label("is_subnetv6");
            return;
        }
        # Special keywords are valid if defined
        if (in_array($value, $this->allow_keywords)) {
            return;
        }

        # If the value was not allowed from an earlier statement, throw a validation error
        throw new ValidationError(
            message: "Field `$field_name` must be a valid subnet CIDR, received `$value`.",
            response_id: "SUBNET_VALIDATOR_FAILED"
        );
    }
}
