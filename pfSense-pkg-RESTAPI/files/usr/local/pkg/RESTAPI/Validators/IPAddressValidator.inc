<?php

namespace RESTAPI\Validators;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Responses\ValidationError;

class IPAddressValidator extends RESTAPI\Core\Validator {
    public bool $allow_ipv4;
    public bool $allow_ipv6;
    public bool $allow_ipv6_link_local;
    public bool $allow_alias;
    public bool $allow_fqdn;
    public array $allow_keywords;

    /**
     * @param bool $allow_ipv4 Allow value to be an IPv4 address.
     * @param bool $allow_ipv6 Allow value to be an IPv6 address.
     * @param bool $allow_ipv6_link_local Allow value to be an IPv6 link local address. Only applies if $allow_ipv6
     * is `true`.
     * @param bool $allow_fqdn Allow value to be an FQDN.
     * @param array $allow_keywords An array of non IP/FQDN values to allow.
     */
    public function __construct(
        bool $allow_ipv4 = true,
        bool $allow_ipv6 = true,
        bool $allow_ipv6_link_local = true,
        bool $allow_alias = false,
        bool $allow_fqdn = false,
        array $allow_keywords = [],
    ) {
        $this->allow_ipv4 = $allow_ipv4;
        $this->allow_ipv6 = $allow_ipv6;
        $this->allow_ipv6_link_local = $allow_ipv6_link_local;
        $this->allow_alias = $allow_alias;
        $this->allow_fqdn = $allow_fqdn;
        $this->allow_keywords = $allow_keywords;
    }

    /**
     * Checks if a given value is a valid IP address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws \RESTAPI\Responses\ValidationError When the value is not a valid IPv4 address.
     */
    public function validate(mixed $value, string $field_name = '') {
        # Non-port alias values are valid if allowed
        $alias_query = FirewallAlias::query(['name' => $value, 'type__except' => 'port']);
        if ($this->allow_alias and $alias_query->exists()) {
            $this->set_label('is_alias');
            return;
        }
        # IPv4 values are valid if allowed
        if ($this->allow_ipv4 and is_ipaddrv4($value)) {
            $this->set_label('is_ipaddrv4');
            return;
        }
        # Throw an error if the IPv6 address given is a link local address, but link local addresses are not allowed
        if ($this->allow_ipv6 and !$this->allow_ipv6_link_local and ip_in_subnet($value, 'fe80::/10')) {
            throw new ValidationError(
                message: "Field `$field_name` cannot be an IPv6 link-local address, received `$value`.",
                response_id: 'IP_ADDRESS_VALIDATOR_IPV6_LINK_LOCAL_NOT_ALLOWED',
            );
        }
        # IPv6 values are valid if allowed
        if ($this->allow_ipv6 and is_ipaddrv6($value)) {
            $this->set_label('is_ipaddrv6');
            return;
        }
        # FQDN values are valid if allowed
        if ($this->allow_fqdn and is_fqdn($value)) {
            $this->set_label('is_fqdn');
            return;
        }
        # Special keywords are valid if defined
        if (in_array($value, $this->allow_keywords)) {
            return;
        }

        # Format the error message to include what options are allowed
        $allowed_options = $this->allow_ipv4 ? ['IPv4 address'] : [];
        $allowed_options = $this->allow_ipv6 ? $allowed_options + ['IPv6 address'] : $allowed_options;
        $allowed_options = $this->allow_alias ? $allowed_options + ['alias'] : $allowed_options;
        $allowed_options = $this->allow_fqdn ? $allowed_options + ['FQDN'] : $allowed_options;
        $allowed_options_str = implode(', ', $allowed_options);

        # If the value was not allowed from an earlier statement, throw a validation error
        throw new ValidationError(
            message: "Field `$field_name` must be a valid $allowed_options_str, received `$value`.",
            response_id: 'IP_ADDRESS_VALIDATOR_FAILED',
        );
    }
}
