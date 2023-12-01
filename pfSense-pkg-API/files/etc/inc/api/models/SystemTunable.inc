<?php

namespace API\Models;

use API\Core\Auth;
use API\Core\Model;
use API\Fields\StringField;
use api\validators\RegexValidator;

class SystemTunable extends Model
{
    public StringField $tunable;
    public StringField $value;
    public StringField $descr;
    
    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null, bool $async = true)
    {
        # Set model attributes
        $this->config_path = "sysctl/item";
        $this->many = true;
        $this->subsystem = "sysctl";
        
        # Set model fields
        $this->tunable = new StringField(
            required: true,
            unique: true,
            help_text: "The name of the tunable to set."
        );
        $this->value = new StringField(
            required: true,
            validators: [new RegexValidator(pattern: "/[^a-zA-Z0-9.\-_%\/]/", invert: true)],
            help_text: "The value to assign this tunable."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "A description for this tunable."
        );
        
        parent::__construct(id: $id, data: $data, client: $client, async: $async);
    }

    /**
     * Apply all configured sysctl tunables.
     */
    public function apply()
    {
        system_setup_sysctl();
    }
}
