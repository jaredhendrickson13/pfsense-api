<?php

namespace API\Validators;

require_once("api/auto_loader.inc");

use API;
use API\Responses\ValidationError;

class GatewayValidator extends API\Core\Validator {
    public bool $allow_ipv4;
    public bool $allow_ipv6;
    public bool $allow_empty;
    public string|null $interface;

    /**
     * @param bool $allow_ipv4 Allow value to be an IPv4 address
     * @param bool $allow_ipv6 Allow value to be an IPv6 address
     * @param bool $allow_empty Allow empty strings as a valid option
     * @param string|null $interface Only allow gateways assigned to this specific interface. This value must be a pfSense
     * interface ID (e.g. wan, lan, opt1) or be left blank to allow any interface. This value is passed in by reference.
     */
    public function __construct(
        bool $allow_ipv4 = true, bool $allow_ipv6 = true, bool $allow_empty = true, string|null &$interface = ""
    ) {
        $this->allow_ipv4 = $allow_ipv4;
        $this->allow_ipv6 = $allow_ipv6;
        $this->allow_empty = $allow_empty;
        $this->interface = &$interface;
    }

    /**
     * Checks if a given value is a valid IP address.
     * @param mixed $value The value to validate.
     * @param string $field_name The field name of the value being validated. This is used for error messages.
     * @throws API\Responses\ValidationError When the value is not a valid gateway.
     */
    public function validate(mixed $value, string $field_name = "") {
        # Allow empty values if requested
        if ($this->allow_empty and $value === "") {
            return;
        }

        # Gather details about configured gateways
        $gateways = return_gateways_array();

        # Loop through each gateway and see if name matches
        foreach ($gateways as $gateway) {
            # Continue if this value does not match this gateway's name
            if ($value != $gateway["name"]) {
                continue;
            }
            # Continue if this is an IPv4 gateway but IPv4 gateways are not allowed
            elseif (!$this->allow_ipv4 and $gateway["ipprotocol"] === "inet") {
                continue;
            }
            # Continue if this is an IPv6 gateway but IPv6 gateways are not allowed
            elseif (!$this->allow_ipv6 and $gateway["ipprotocol"] === "inet6") {
                continue;
            }
            # Continue if only gateways of a specific interface are allowed
            elseif ($this->interface and $this->interface !== $gateway["friendlyiface"]) {
                continue;
            }

            # Value is valid if the loop was not continued by a previous statement
            return;
        }

    # If the value was not allowed from an earlier statement, throw a validation error
        throw new ValidationError(
            message: "Field `$field_name` must be a valid gateway, received `$value`.",
            response_id: "GATEWAY_VALIDATOR_FAILED"
        );
    }
}
