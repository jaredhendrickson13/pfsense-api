<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Auth;
use RESTAPI\Core\Dispatcher;
use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\InterfaceField;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ValidationError;

class TrafficShaper extends Model
{
    public BooleanField $enabled;
    public InterfaceField $interface;
    public StringField $name;
    public StringField $scheduler;
    public StringField $bandwidthtype;
    public IntegerField $bandwidth;
    public IntegerField $qlimit;
    public IntegerField $tbrconfig;
    public NestedModelField $queue;

    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null, bool $async = true)
    {
        # Set model attributes
        $this->config_path = "shaper/queue";
        $this->many = true;
        $this->subsystem = "shaper";

        # Set model fields
        $this->enabled = new BooleanField(
            default: true,
            indicates_true: "on",
            help_text: "Enables or disables this traffic shaper."
        );
        $this->interface = new InterfaceField(
            required: true,
            unique: true,
            editable: false,
            help_text: "The interface this traffic shaper will be applied to."
        );
        $this->name = new StringField(
            allow_null: true,
            editable: false,
            read_only: true,
            help_text: "The name of this traffic shaper. This value is automatically set by the system and cannot ".
                "be changed."
        );
        $this->scheduler = new StringField(
            required: true,
            choices: ["HFSC", "CBQ", "FAIRQ", "CODELQ", "PRIQ"],
            help_text: "The scheduler type to use for this traffic shaper. Changing this value will automatically ".
                "update any child queues assigned to this traffic shaper."
        );
        $this->bandwidthtype = new StringField(
            required: true,
            choices: ["%", "b", "Kb", "Mb", "Gb"],
            help_text: "The scale type of the `bandwidth` field's value."
        );
        $this->bandwidth = new IntegerField(
            required: true,
            minimum: 1,
            help_text: "The total bandwidth amount allowed by this traffic shaper."
        );
        $this->qlimit = new IntegerField(
            default: 50,
            allow_null: true,
            minimum: 1,
            conditions: ["!scheduler" => ["CODELQ"]],
            help_text: "The number of packets that can be held in a queue waiting to be transmitted by the shaper."
        );
        $this->tbrconfig = new IntegerField(
            default: null,
            allow_null: true,
            minimum: 1,
            help_text: "The size, in bytes, of the token bucket regulator. If `null`, heuristics based on the ".
                "interface bandwidth are used to determine the size."
        );
        $this->queue = new NestedModelField(
            model_class: "TrafficShaperQueue",
            default: [],
            allow_empty: true,
            help_text: "The child queues assigned to this traffic shaper."
        );

        parent::__construct(id: $id, data: $data, client: $client, async: $async);
    }

    /**
     * Adds extra validation to the `bandwidth` field.
     * @param int $bandwidth The incoming value to be validated.
     * @returns int The validated value to be assigned.
     * @throws ValidationError When $bandwidth is greater than 100, but the `bandwithtype` field is `%`.
     */
    public function validate_bandwidth(int $bandwidth): int {
        # Do not allow the $bandwidth value to be greater than 100 if the bandwidthtype field is set %
        if ($this->bandwidthtype->value === "%" and $bandwidth > 100) {
            throw new ValidationError(
                message: "Field `bandwidth` cannot exceed 100 while `bandwidthtype` is set to `%`.",
                response_id: "TRAFFIC_SHAPER_BANDWIDTH_EXCEED_MAXIMUM_PERCENTAGE"
            );
        }

        return $bandwidth;
    }

    /**
     * Reloads the firewall filter and restarts queue graphing.
     */
    public function apply(): void
    {
        (new Dispatcher(async: $this->async))->spawn_process();
        system("rm -f /var/db/rrd/*queuedrops.rrd");
        system("rm -f /var/db/rrd/*queues.rrd");
        enable_rrd_graphing();
        clear_subsystem_dirty('shaper');
    }
}
