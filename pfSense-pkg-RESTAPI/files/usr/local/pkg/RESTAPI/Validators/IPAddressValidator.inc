<?php

namespace RESTAPI\Validators;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a Validator that checks if a given value is a valid IPv4 or IPv6 address.
 */
class IPAddressValidator extends RESTAPI\Core\Validator {
    /**
     * @param bool $allow_ipv4 Allow value to be an IPv4 address.
     * @param bool $allow_ipv6 Allow value to be an IPv6 address.
     * @param bool $allow_ipv6_link_local Allow value to be an IPv6 link local address. Only applies if $allow_ipv6
     * is `true`.
     * @param bool $allow_fqdn Allow value to be an FQDN.
     * @param bool $allow_port Allow value to end with a port number (e.g. `:80`).
     * @param array $allow_keywords An array of non IP/FQDN values to allow.
     */
    public function __construct(
        public bool $allow_ipv4 = true,
        public bool $allow_ipv6 = true,
        public bool $allow_ipv6_link_local = true,
        public bool $allow_alias = false,
        public bool $allow_fqdn = false,
        public bool $allow_port = false,
        public array $allow_keywords = [],
    ) {}

    /**
     * Checks if a given value is a valid IP address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws \RESTAPI\Responses\ValidationError When the value is not a valid IPv4 address.
     */
    public function validate(mixed $value, string $field_name = ''): void {
        # Allow the value to end with a port number if allowed
        if ($this->allow_port) {
            $value = $this->validate_port_suffix($value, $field_name);
        }

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

    /**
     * Checks if a given value is a valid socket address (ADDR:PORT) and validates that the port number is valid.
     * @param mixed $value The value to be validated.
     * @param string $field_name
     * @return string
     */
    private function validate_port_suffix(mixed $value, string $field_name): string {
        # Check if the value has a port suffix, if not return the value as is
        if (!str_contains($value, ':')) {
            return $value;
        }

        # Split the value into the IP address and port number
        $split_value = explode(':', $value);
        $ip_address = $split_value[0];
        $port_number = $split_value[1];

        # Ensure the port number is a valid port number
        if (!is_port($port_number)) {
            throw new ValidationError(
                message: "Field `$field_name` contains an invalid port suffix `$port_number`.",
                response_id: 'IP_ADDRESS_VALIDATOR_INVALID_PORT_SUFFIX',
            );
        }

        return $ip_address;
    }
}
