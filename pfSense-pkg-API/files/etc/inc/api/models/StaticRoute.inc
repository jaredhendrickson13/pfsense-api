<?php

namespace API\Models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\BooleanField;
use API\Fields\StringField;
use API\Responses\ValidationError;
use API\Validators\GatewayValidator;
use API\Validators\SubnetValidator;

/**
 * Defines a Model object for StaticRoutes
 */
class StaticRoute extends Model
{
    public StringField $network;
    public StringField $gateway;
    public StringField $descr;
    public BooleanField $disabled;

    /**
     * Constructs the StaticRoute object.
     */
    public function __construct(mixed $id = null, mixed $representation_data = [], Auth $client = null)
    {
        # Define model attributes
        $this->config_path = "staticroutes/route";
        $this->many = true;
        $this->subsystem = "staticroutes";

        # Define model fields
        $this->network = new StringField(
            required: true,
            validators: [new SubnetValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "Sets the destination network for this static route in CIDR notation."
        );
        $this->gateway = new StringField(
            required: true,
            validators: [new GatewayValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "Sets which gateway this route applies to."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "Sets a description for administrative reference."
        );
        $this->disabled = new BooleanField(
            default: false,
            help_text: "Disable this static route."
        );

        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds custom validation for the `gateway` field.
     * @param string $gateway The incoming `gateway` value to be validated.
     * @return string The validated `gateway` value to set.
     */
    public function validate_gateway(string $gateway) : string {
        # Fetch the gateway object of the value assigned
        $gateway_obj = RoutingGateway::read_all()->query(["name" => $gateway])->first();

        # Ensure the upstream gateways IP version matches the `network` assigned
        if ($gateway_obj->ipprotocol->value === "inet" and !is_subnetv4($this->network->value)) {
            throw new ValidationError(
                message: "Cannot assign non-IPv4 `network` to IPv4 `gateway`.",
                response_id: "STATIC_ROUTE_NON_IPV4_NETWORK_WITH_IPV4_GATEWAY"
            );
        }

        # Ensure the upstream gateways IP version matches the `network` assigned
        if ($gateway_obj->ipprotocol->value === "inet6" and !is_subnetv6($this->network->value)) {
            throw new ValidationError(
                message: "Cannot assign non-IPv6 `network` to IPv6 `gateway`.",
                response_id: "STATIC_ROUTE_NON_IPV6_NETWORK_WITH_IPV6_GATEWAY"
            );
        }

        return $gateway;
    }

    /**
     * Adds custom validation for the `disabled` field.
     * @param bool $disabled The incoming `disabled` value to be validated.
     * @return bool The validated `disabled` value to set.
     */
    public function validate_disabled(bool $disabled) : bool {
        # Fetch the gateway object of the gateway assigned
        $gateway_obj = RoutingGateway::read_all()->query(["name" => $this->gateway->value])->first();

        # Force route to be disabled if the parent gateway is disabled
        if ($gateway_obj->disabled->value and !$disabled) {
            throw new ValidationError(
                message: " Static route must also be `disabled` when the parent gateway is `disabled`.",
                response_id: "STATIC_ROUTE_GATEWAY_DISABLED"
            );
        }

        return $disabled;
    }

    /**
     * Applies pending static route changes.
     */
    public function apply() {
        (new RoutingApply())->apply();
    }

    /**
     * Ensures the static route deletion is performed before removing it from config
     */
    public function _delete() {
        delete_static_route($this->id, true);
        parent::_delete();
    }
}