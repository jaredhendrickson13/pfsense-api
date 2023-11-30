<?php

namespace API\Models;

use API\Core\Auth;
use API\Core\Command;
use API\Core\Model;
use API\Fields\InterfaceField;
use API\Fields\StringField;
use API\Validators\FilterNameValidator;

class InterfaceGroup extends Model
{
    public StringField $ifname;
    public InterfaceField $members;
    public StringField $descr;

    /**
     * @inheritDoc
     */
    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null, bool $async = true)
    {
        # Set model attributes
        $this->config_path = "ifgroups/ifgroupentry";
        $this->many = true;
        $this->always_apply = true;

        # Set model fields
        $this->ifname = new StringField(
            required: true,
            unique: true,
            editable: false,
            maximum_length: 15,
            validators: [new FilterNameValidator()],
            help_text: "The name of this interface group."
        );
        $this->members = new InterfaceField(
            default: [],
            allow_empty: true,
            many: true,
            help_text: "The member interfaces to assign to this interface group."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "The description for this interface group."
        );

        parent::__construct(id: $id, data: $data, client: $client, async: $async);
    }

    /**
     * Applies the interface group changes
     */
    public function apply()
    {
        interface_group_setup($this->to_internal());
    }

    /**
     * Applies the deletion of an interface group.
     */
    public function apply_delete()
    {
        # Loop through each member interface and remove it from the group
        foreach ($this->members->get_interface_obj()->model_objects as $member) {
            new Command("/sbin/ifconfig {$member->if->value} -group " . $this->ifname->value);
        }
    }
}