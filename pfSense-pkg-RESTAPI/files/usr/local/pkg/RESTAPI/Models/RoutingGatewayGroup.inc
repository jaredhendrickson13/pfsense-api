<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\RoutingApplyDispatcher;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\NotFoundError;
use RESTAPI\Validators\FilterNameValidator;
use RESTAPI\Validators\RegexValidator;

/**
 * Defines a Model for interacting with a Gateway Group priority item.
 */
class RoutingGatewayGroup extends Model {
    public StringField $name;
    public StringField $trigger;
    public StringField $descr;
    public NestedModelField $priorities;

    public function __construct(
        mixed $id = null,
        mixed $parent_id = null,
        mixed $data = [],
        mixed ...$options,
    ) {
        # Set model attributes
        $this->config_path = 'gateways/gateway_group';
        $this->many = true;

        # Set model fields
        $this->name = new StringField(
            required: true,
            unique: true,
            editable: false,
            maximum_length: 31,
            referenced_by: ['StaticRoute' => 'gateway', 'FirewallRule' => 'gateway'],
            validators: [new FilterNameValidator()],
            help_text: "The name of the gateway group."
        );
        $this->trigger = new StringField(
            default: "down",
            choices: ["down", "downloss", "downlatency", "downlosslatency"],
            help_text: "The trigger that will cause a gateway to be excluded from the group."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            maximum_length: 255,
            help_text: "A description of the gateway group."
        );
        $this->priorities = new NestedModelField(
            model_class: 'RoutingGatewayGroupPriority',
            default: [],
            help_text: "The priorities of the gateways in this group."
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Apply changes to cron.
     */
    public function apply(): void {
        (new RoutingApplyDispatcher(async: $this->async))->spawn_process();
    }
}
