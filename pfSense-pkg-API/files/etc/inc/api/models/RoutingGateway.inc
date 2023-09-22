<?php

namespace API\Models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\BooleanField;
use API\Fields\InterfaceField;
use API\Fields\StringField;
use API\Responses\ValidationError;
use API\Validators\FilterNameValidator;
use API\Validators\IPAddressValidator;

/**
 * Defines a Model for reading, creating, updating and deleting routing gateways.
 */
class RoutingGateway extends Model
{
    public StringField $name;
    public BooleanField $disabled;
    public StringField $ipprotocol;
    public InterfaceField $interface;
    public StringField $gateway;

    public function __construct(mixed $id = null, mixed $representation_data = [], Auth $client = null)
    {
        # Define Model attributes
        $this->config_path = "gateways/gateway_item";
        $this->many = true;
        $this->unique_together_fields = ["interface", "ipprotocol", "gateway"];

        # Define Model fields
        $this->name = new StringField(
            required: true,
            unique: true,
            editable: false,
            maximum_length: 31,
            validators: [new FilterNameValidator()],
            help_text: "Sets a name for the gateway."
        );
        $this->disabled = new BooleanField(
            default: false,
            indicates_true: "yes",
            help_text: "Disable this gateway."
        );
        $this->ipprotocol = new StringField(
            required: true,
            choices: ["inet", "inet6"],
            help_text: "Sets the Internet Protocol version this gateway uses."
        );
        $this->interface = new InterfaceField(
            required: true,
            help_text: "Sets the interface this gateway will apply to."
        );
        $this->gateway =  new StringField(
            required: true,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true, allow_keywords: ["dynamic"])],
            help_text: "Sets the IP address of the remote gateway."
        );

        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds custom validation to the `interface` field.
     * @param string $interface The $interface value to validate.
     * @return string The validated interface value to set.
     */
    public function validate_interface(string $interface) : string {
        # Obtain the ID of the interface
        $if_id = $this->interface->get_interface_obj()->id;

        # Ensure the configured `interface` has an IPv4 address if `protocol` is set to `inet`
        if ($this->ipprotocol->value === "inet" and !get_interface_ip($if_id) or !get_interface_subnet($if_id)) {
            throw new ValidationError(
                message: "Field `interface` must be an interface with an existing IPv4 address when `ipprotocol` ".
                    "is set to `inet`",
                response_id: "ROUTING_GATEWAY_INTERFACE_HAS_NO_IPV4"
            );
        }

        # Ensure the configured `interface` has an IPv6 address if `protocol` is set to `inet6`
        if ($this->ipprotocol->value === "inet6" and !get_interface_ipv6($if_id) or !get_interface_subnetv6($if_id)) {
            throw new ValidationError(
                message: "Field `interface` must be an interface with an existing IPv4 address when `ipprotocol` ".
                "is set to `inet6`",
                response_id: "ROUTING_GATEWAY_INTERFACE_HAS_NO_IPV6"
            );
        }

        return $interface;
    }

    /**
     * Adds custom validation to the `gateway` field
     * @param string $gateway The gateway value to validate.
     * @return string The validated gateway value to set.
     */
    public function validate_gateway(string $gateway) : string {
        # Only allow IPv4 addresses if `ipprotocol` is `inet`
        if ($this->ipprotocol->value === "inet" and is_ipaddrv6($gateway)) {
            throw new ValidationError(
                message: "Field `gateway` must be an IPv4 address when `ipprotocol` is set to `inet`",
                response_id: "ROUTING_GATEWAY_NO_IPV6_WITH_INET"
            );
        }

        # Only allow IPv4 addresses if `ipprotocol` is `inet6`
        if ($this->ipprotocol->value === "inet6" and is_ipaddrv4($gateway)) {
            throw new ValidationError(
                message: "Field `gateway` must be an IPv6 address when `ipprotocol` is set to `inet6`",
                response_id: "ROUTING_GATEWAY_NO_IPV4_WITH_INET6"
            );
        }

        return $gateway;
    }
}