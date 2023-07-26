<?php

namespace API\Models;
use API;
use API\Validators\AliasNameValidator;
use API\Fields\StringField;

class FirewallAlias extends API\Core\Model {
    public StringField $name;
    public StringField $type;
    public StringField $descr;
    public StringField $address;
    public StringField $detail;

    public function __construct(array $initial_data = [], API\Core\Auth $client = null) {
        parent::__construct($initial_data, $client);
        $this->name = new StringField(
            validators: [new AliasNameValidator()],
            required: true,
            description: "Sets the unique name for the alias."
        );
        $this->type = new StringField(
            choices: ["host", "network", "port"],
            required: true,
            description: "Sets the type of alias this object will be. This directly impacts what values can be
                specified in the `address` field."
        );
        $this->descr = new StringField(
            allow_empty: true,
            default: "",
            description: "Sets a description to help specify the purpose or contents of the alias."
        );
        $this->address = new StringField(
            default: [],
            many: true,
            delimiter: " ",
            description: "Sets the host, network or port entries for the alias. When `type` is set to `host`, each
                entry must be a valid IP address or FQDN. When `type` is set to `network`, each entry must be a valid
                network CIDR or FQDN. When `type` is set to `port`, each entry must be a valid port or port range. You
                may also specify an existing alias's `name` as an entry to created nested aliases."
        );
        $this->detail = new StringField(
            default: [],
            many: true,
            delimiter: "||",
            description: "Sets descriptions for each alias `address`. Values must match the order of the `address` 
                value it relates to. For example, the first value specified here is the description for the first
                value specified in the `address` field. This value cannot contain "
        );
    }

    public function validate_address(string $value) {
        # Ensure value is a port, port range or port alias when `type` is `port`
        if ($this->type->value === "port" and !is_port_or_range_or_alias($value)) {
            throw new API\Core\ValidationError(
                message: "",
                return: ""
            );
        }
    }
}
