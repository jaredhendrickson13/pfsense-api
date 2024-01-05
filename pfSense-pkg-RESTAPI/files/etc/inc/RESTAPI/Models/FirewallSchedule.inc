<?php

namespace RESTAPI\Models;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\FirewallApplyDispatcher;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\StringField;
use RESTAPI\Fields\UIDField;
use RESTAPI\Validators\FilterNameValidator;

class FirewallSchedule extends Model
{
    public UIDField $schedlabel;
    public StringField $name;
    public StringField $descr;
    public NestedModelField $timerange;
    
    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null, bool $async = true)
    {
        # Set model attributes
        $this->config_path = "schedules/schedule";
        $this->many = true;
        $this->always_apply = true;
        $this->sort_by_field = "name";
        $this->sort_option = SORT_ASC;
        
        # Set model fields
        $this->schedlabel = new UIDField(help_text: "A unique ID for this schedule used internally by the system.");
        $this->name = new StringField(
            required: true,
            unique: true,
            validators: [new FilterNameValidator()],
            help_text: "The unique name to assign this schedule."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "A description of this schedules purpose."
        );
        $this->timerange = new NestedModelField(
            model_class: "FirewallScheduleTimeRange",
            required: true,
            help_text: "The date/times this firewall schedule will be active."
        );
        
        parent::__construct(id:$id, data: $data, client: $client, async: $async);
    }
    
    /**
     * Applies the schedule by reloading the filter.
     */
    public function apply() {
        (new FirewallApplyDispatcher(async: $this->async))->spawn_process();
    }
}
