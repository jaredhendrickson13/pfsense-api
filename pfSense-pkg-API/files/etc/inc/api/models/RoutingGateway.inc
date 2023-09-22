<?php

namespace API\Models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\BooleanField;
use API\Fields\IntegerField;
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
    public StringField $descr;
    public BooleanField $disabled;
    public StringField $ipprotocol;
    public InterfaceField $interface;
    public StringField $gateway;
    public BooleanField $monitor_disable;
    public StringField $monitor;
    public BooleanField $action_disable;
    public BooleanField $force_down;
    public BooleanField $dpinger_dont_add_static_route;
    public StringField $gw_down_kill_states;
    public BooleanField $nonlocalgateway;
    public IntegerField $weight;
    public IntegerField $data_payload;
    public IntegerField $latencylow;
    public IntegerField $latencyhigh;
    public IntegerField $losslow;
    public IntegerField $losshigh;
    public IntegerField $interval;
    public IntegerField $loss_interval;
    public IntegerField $time_period;
    public IntegerField $alert_interval;

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
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "Sets a descriptions for the gateway."
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
        $this->monitor_disable = new BooleanField(
            default: false,
            indicates_true: "yes",
            help_text: "Disable gateway monitoring for this gateway."
        );
        $this->monitor =  new StringField(
            required: false,
            allow_null: true,
            conditions: ["monitor_disable" => false],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "Sets a different IP address to use when monitoring this gateway. This is typically only
                necessary if the gateway IP does not accept ICMP probes."
        );
        $this->action_disable = new BooleanField(
            default: false,
            indicates_true: "yes",
            help_text: "Disable actions from taking place when gateway events occur. The gateway will always be
                considered up."
        );
        $this->force_down = new BooleanField(
            default: false,
            indicates_true: "yes",
            help_text: "Always consider this gateway to be up."
        );
        $this->dpinger_dont_add_static_route = new BooleanField(
            default: false,
            indicates_true: "",
            help_text: "Prevents gateway monitoring from adding a static route for this gateway's monitor IP."
        );
        $this->gw_down_kill_states = new StringField(
            default: "",
            choices: ["", "none", "down"],
            allow_empty: true,
            help_text: "Controls the state killing behavior when this specific gateway goes down. Killing states for ".
                "specific down gateways only affects states created by policy routing rules and reply-to. Has no ".
                "effect if gateway monitoring or its action are disabled or if the gateway is forced down. May not ".
                "have any effect on dynamic gateways during a link loss event."
        );
        $this->nonlocalgateway = new BooleanField(
            default: true,
            read_only: true,
            indicates_true: "yes",
            help_text: "Allows or disallows gateway IPs that are not a part of the parent interface's subnet(s)."
        );
        $this->weight = new IntegerField(
            default: 1,
            minimum: 1,
            maximum: 30,
            help_text: "Sets the weight for this gateway when used in a Gateway Group."
        );
        $this->data_payload = new IntegerField(
            default: 1,
            minimum: 1,
            help_text: "Sets the data payload to send in ICMP packets to gateway monitor IP."
        );
        $this->latencylow = new IntegerField(
            default: 200,
            minimum: 1,
            help_text: "Sets the threshold to consider latency as low."
        );
        $this->latencyhigh = new IntegerField(
            default: 500,
            minimum: 2,
            help_text: "Sets the threshold to consider latency as high. This value must be greater than `latencylow`."
        );
        $this->losslow = new IntegerField(
            default: 200,
            minimum: 1,
            maximum: 99,
            help_text: "Sets the threshold to consider packet loss as low."
        );
        $this->losshigh = new IntegerField(
            default: 20,
            minimum: 2,
            maximum: 100,
            help_text: "Sets the threshold to consider packet loss as high. This value must be greater than `losslow`."
        );
        $this->interval = new IntegerField(
            default: 500,
            minimum: 1,
            maximum: 3600000,
            help_text: "Sets how often ICMP probes will be sent in milliseconds."
        );
        $this->loss_interval = new IntegerField(
            default: 2000,
            minimum: 1,
            help_text: "Sets the time interval in milliseconds before packets are treated as lost."
        );
        $this->time_period = new IntegerField(
            default: 60000,
            minimum: 1,
            help_text: "Sets the time period in milliseconds over which results are averaged."
        );
        $this->alert_interval = new IntegerField(
            default: 1000,
            minimum: 1,
            help_text: "Sets the time interval in milliseconds between checking for an alert conditions."
        );

        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds custom validation to the `disabled` field.
     * @param bool $disabled The current $disabled value to validate.
     * @return bool The validated disabled value to set.
     */
    public function validate_disabled(bool $disabled) : bool {
        # When disabling this gateway is requested, ensure it is not in use first
        if ($disabled) {
            # Ensure the gateway cannot be disabled while it is in use by a gateway group
            # TODO: replace this with a `referenced_by` Field value when the GatewayGroups Model is developed
            foreach ($this->get_config(path: 'gateways/gateway_group', default: []) as $gw_group_id => $gw_group) {
                foreach ($gw_group['item'] as $item) {
                    $items = explode("|", $item);
                    if ($items[0] == $this->name->value) {
                        throw new ValidationError(
                            message: "Gateway cannot be disabled because it is in use by gateway " .
                            "group with ID `$gw_group_id`",
                            response_id: "ROUTING_GATEWAY_DISABLED_NOT_ALLOWED_WITH_EXISTING_GW_GROUP"
                        );
                    }
                }
            }

            # Ensure the gateway cannot be disabled while it is in use by a static route
            # TODO: replace with a Model query when the StaticRoute Model is developed
            foreach ($this->get_config('staticroutes/route', []) as $route_id => $route) {
                if ($route['gateway'] == $this->name->value) {
                    if (!$route['disabled']) {
                        throw new ValidationError(
                            message: "Gateway cannot be disabled because it is in use by gateway " .
                            "route with ID `$route_id`",
                            response_id: "ROUTING_GATEWAY_DISABLED_NOT_ALLOWED_WITH_EXISTING_ROUTE"
                        );
                    }
                }
            }

            # Ensure the gateway cannot be disabled while it is in use by a system DNS server
            # TODO: replace with a Model query when the SystemDNSServer Model is developed
            $dnsgw_counter = 1;
            while (isset($config["system"]["dns{$dnsgw_counter}gw"])) {
                if ($this->name->value == $config["system"]["dns{$dnsgw_counter}gw"]) {
                    throw new ValidationError(
                        message: "Gateway cannot be disabled because it is in use by a system DNS " .
                        "server with ID `$dnsgw_counter`",
                        response_id: "ROUTING_GATEWAY_DISABLED_NOT_ALLOWED_WITH_EXISTING_ROUTE"
                    );
                }
                $dnsgw_counter++;
            }
        }

        return $disabled;
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
        if ($this->ipprotocol->value === "inet" and !is_ipaddrv4($gateway) and $gateway !== "dynamic") {
            throw new ValidationError(
                message: "Field `gateway` must be an IPv4 address when `ipprotocol` is set to `inet`",
                response_id: "ROUTING_GATEWAY_NO_IPV6_WITH_INET"
            );
        }

        # Only allow IPv4 addresses if `ipprotocol` is `inet6`
        if ($this->ipprotocol->value === "inet6" and !is_ipaddrv6($gateway) and $gateway !== "dynamic") {
            throw new ValidationError(
                message: "Field `gateway` must be an IPv6 address when `ipprotocol` is set to `inet6`",
                response_id: "ROUTING_GATEWAY_NO_IPV4_WITH_INET6"
            );
        }

        # For non-dynamic IPv4 gateways, format the $gateway value's subnet to use the network address
        if ($this->ipprotocol->value === "inet" and $gateway !== "dynamic") {
            $cidr_components = explode("/", $gateway);
            $gateway = gen_subnet($cidr_components[0])."/".$cidr_components[1];
        }

        # For non-dynamic IPv6 gateways, format the $gateway value's subnet to use the network address
        if ($this->ipprotocol->value === "inet" and $gateway !== "dynamic") {
            $cidr_components = explode("/", $gateway);
            $gateway = gen_subnetv6($cidr_components[0])."/".$cidr_components[1];
        }


        return $gateway;
    }

    /**
     * Adds custom validation for the `latencyhigh` field.
     * @param int $latencyhigh The incoming `latencyhigh` value to validate.
     * @return int The validated `latencyhigh`value to set.
     */
    public function validate_latencyhigh(int $latencyhigh) : int {
        # Do not allow the `latencylow` value to be greater than `latencyhigh`
        if ($this->latencylow->value > $latencyhigh) {
            throw new ValidationError(
                message: "Routing gateway `latencyhigh` must be greater than `latencylow`.",
                response_id: "ROUTING_GATEWAY_WITH_LATENCY_LOW_GREATER_THAN_HIGH"
            );
        }

        return $latencyhigh;
    }

    /**
     * Adds custom validation for the `losshigh` field.
     * @param int $losshigh The incoming `losshigh` value to validate.
     * @return int The validated `losshigh`value to set.
     */
    public function validate_losshigh(int $losshigh) : int {
        # Do not allow the `losslow` value to be greater than `losshigh`
        if ($this->losslow->value > $losshigh) {
            throw new ValidationError(
                message: "Routing gateway `losshigh` must be greater than `losslow`.",
                response_id: "ROUTING_GATEWAY_WITH_LOSS_LOW_GREATER_THAN_HIGH"
            );
        }

        return $losshigh;
    }

    /**
     * Adds custom validation for the `loss_interval` field.
     * @param int $loss_interval The incoming `loss_interval` value to validate.
     * @return int The validated `loss_interval`value to set.
     */
    public function validate_loss_interval(int $loss_interval) : int {
        # Do not allow the `loss_interval` value to be less than `latencyhigh`
        if ($this->latencyhigh->value > $loss_interval) {
            throw new ValidationError(
                message: "Routing gateway `loss_interval` must be greater than or equal to `latencyhigh`.",
                response_id: "ROUTING_GATEWAY_WITH_LOSS_INTERVAL_GREATER_THAN_LATENCY_HIGH"
            );
        }

        return $loss_interval;
    }

    /**
     * Adds custom validation for the `time_period` field.
     * @param int $time_period The incoming `time_period` value to validate.
     * @return int The validated `time_period`value to set.
     */
    public function validate_time_period(int $time_period) : int {
        # Require `time_period` to be greater than twice the `interval` plus the `loss_interval`
        if ($time_period <= 2 * $this->interval->value + $this->loss_interval->value) {
            throw new ValidationError(
                message: "Routing gateway `time_period` must be greater than twice the `interval` value plus the ".
                    "`loss_interval` value.",
                response_id: "ROUTING_GATEWAY_TIME_PERIOD_UNDER_MINIMUM"
            );
        }

        return $time_period;
    }

    /**
     * Adds custom validation for the `alert_interval` field.
     * @param int $alert_interval The incoming `alert_interval` value to validate.
     * @return int The validated `alert_interval`value to set.
     */
    public function validate_alert_interval(int $alert_interval) : int {
        # Require `alert_interval` to be greater than or equal to the `interval`
        if ($alert_interval >= $this->interval->value) {
            throw new ValidationError(
                message: "Routing gateway `alert_interval` must be greater than or equal to the `interval`.",
                response_id: "ROUTING_GATEWAY_ALERT_INTERVAL_UNDER_MINIMUM"
            );
        }

        return $alert_interval;
    }

    /**
     * Define steps to create this RoutingGateway object internally.
     */
    public function _create() {
        # Call the pfSense `save_gateway()` function with the internal data of the gateway to create.
        save_gateway($this->to_internal());
    }

    /**
     * Define steps to update this RoutingGateway object internally.
     */
    public function _update() {
        # Call the pfSense `save_gateway()` function with the internal data of the gateway to update and its ID.
        save_gateway($this->to_internal(), $this->id);
    }

    /**
     * Define steps to delete this RoutingGateway object internally.
     */
    public function _delete() {
        # Local variables
        $gw_data = return_gateways_array(true, false, true, true);

        # Loop through the gateway days until we find the gateway to delete
        foreach ($gw_data as $gw) {
            # Check if this gateways 'attribute' matches the ID of the gateway we're deleting
            if ($this->id === $gw["attribute"]) {
                # Remove the gateway and monitor IPs
                route_del($gw["gateway"], $gw["ipprotocol"]);
                route_del($gw["monitor"], $gw["ipprotocol"]);

                # Delete the default route if this gateway is the default gateway
                if ($gw["isdefaultgw"]) {
                    route_del("default", $gw["ipprotocol"]);
                }
            }
        }

        # Remove this gateway from config
        $this->del_config("gateways/gateway_item/{$this->id}");
    }
}
