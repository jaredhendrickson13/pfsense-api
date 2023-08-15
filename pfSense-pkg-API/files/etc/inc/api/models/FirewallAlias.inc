<?php

namespace API\Models;

require_once("api/auto_loader.inc");

use API\Core\Model;
use API\Core\Auth;
use API\Fields\StringField;
use API\Responses\ValidationError;

/**
 * Defines a model object for firewall aliases.
 */
class FirewallAlias extends Model {
    public StringField $name;
    public StringField $type;
    public StringField $descr;
    public StringField $address;
    public StringField $detail;

    public function __construct(mixed $id=null, mixed $representation_data=[], Auth $client=null) {
        $this->config_path = "aliases/alias";
        $this->subsystem = "aliases";
        $this->many = true;
        $this->name = new StringField(
            required: true,
            unique: true,
            editable: false,
            maximum_length: 31,
            help_text: "Sets the name for the alias. This name must be unique from all other aliases."
        );
        $this->type = new StringField(
            required: true,
            choices: ["host", "network", "port"],
            help_text: "Sets the type of alias this object will be. This directly impacts what values can be
                specified in the `address` field."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "Sets a description to help specify the purpose or contents of the alias."
        );
        $this->address = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            delimiter: " ",
            help_text: "Sets the host, network or port entries for the alias. When `type` is set to `host`, each
                entry must be a valid IP address or FQDN. When `type` is set to `network`, each entry must be a valid
                network CIDR or FQDN. When `type` is set to `port`, each entry must be a valid port or port range. You
                may also specify an existing alias's `name` as an entry to created nested aliases."
        );
        $this->detail = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            delimiter: "||",
            help_text: "Sets descriptions for each alias `address`. Values must match the order of the `address` 
                value it relates to. For example, the first value specified here is the description for the first
                value specified in the `address` field. This value cannot contain "
        );

        # Needs to be constructed after fields are defined
        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds custom validation to the `name` field.
     * @param string $name The alias name to validate.
     * @returns string The validated $name to set.
     * @throws ValidationError When the `name` value is invalid.
     */
    public function validate_name(string $name) : string {
        # Throw an exception if this name is not a valid alias name
        if (!is_validaliasname($name)) {
            throw new ValidationError(
                message: "Invalid firewall alias name '$name'.",
                response_id: "INVALID_FIREWALL_ALIAS_NAME"
            );
        }

        return $name;
    }

    /**
     * Adds custom validation to the `address` field.
     * @param array $addresses The array of `address` items to validate.
     * @returns array The validated $addresses to set.
     * @throws ValidationError When the `address` value is invalid.
     */
    public function validate_address(array $addresses) : array {
        # Loop through each address item to validate.
        foreach ($addresses as $address) {
            # Ensure value is a port, port range or port alias when `type` is `port`
            if ($this->type->value === "port" and !is_port_or_range_or_alias($address)) {
                throw new ValidationError(
                    message: "Port alias 'address' value '$address' is not a valid port, range, or alias.",
                    response_id: "INVALID_PORT_ALIAS_ADDRESS"
                );
            }

            # Ensure value is an IP, FQDN or alias when `type` is `host`
            if ($this->type->value === "host" and !is_ipaddroralias($address) and !is_fqdn($address)) {
                throw new ValidationError(
                    message: "Host alias 'address' value '$address' is not a valid IP, FQDN, or alias.",
                    response_id: "INVALID_HOST_ALIAS_ADDRESS"
                );
            }

            # Ensure value is a CIDR, FQDN or alias when `type` is `network`
            if ($this->type->value === "network") {
                if (!is_subnet($address) and alias_get_type($address) != "network" and !is_fqdn($address)) {
                    throw new ValidationError(
                        message: "Host alias 'address' value '$address' is not a valid CIDR, FQDN, or alias.",
                        response_id: "INVALID_NETWORK_ALIAS_ADDRESS"
                    );
                }
            }
        }

        return $addresses;
    }

    /**
     * Adds custom validation to `detail` field
     * @param array $detail The array of `detail` fields to validate.
     * @returns array The validated $detail to set.
     * @throws ValidationError When the `detail` value is invalid.
     */
    public function validate_detail(array $detail) : array {
        # Throw an error if there are more `detail` items than `address` items
        if (count($detail) > count($this->address->value)) {
            throw new ValidationError(
                message: "Aliases cannot contain more 'detail' items than 'address' items.",
                response_id: "TOO_MANY_ALIAS_DETAILS"
            );
        }

        # Automatically generate detail items if there are less details than addresses
        while (count($detail) < count($this->address->value)) {
            $detail[] = "Entry added " . date('r');
        }

        return $detail;
    }

    /**
     * Set the apply method to reload the firewall filter after successfully validating and writing firewall alias
     * changes to config.
     */
    public function apply() {
        filter_configure_sync();
    }
}
