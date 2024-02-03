<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\TrafficShaperApplyDispatcher;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\FloatField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\FilterNameValidator;

class TrafficShaperLimiter extends Model
{
    public StringField $name;
    public IntegerField $number;
    public BooleanField $enabled;
    public StringField $mask;
    public IntegerField $maskbits;
    public IntegerField $maskbitsv6;
    public IntegerField $qlimit;
    public BooleanField $ecn;
    public StringField $description;
    public StringField $aqm;
    public StringField $sched;
    public IntegerField $param_codel_target;
    public IntegerField $param_codel_interval;
    public IntegerField $param_pie_target;
    public IntegerField $param_pie_tupdate;
    public IntegerField $param_pie_alpha;
    public IntegerField $param_pie_beta;
    public IntegerField $param_pie_max_burst;
    public IntegerField $param_pie_max_ecnth;
    public IntegerField $param_red_w_q;
    public IntegerField $param_red_min_th;
    public IntegerField $param_red_max_th;
    public IntegerField $param_red_max_p;
    public IntegerField $param_gred_w_q;
    public IntegerField $param_gred_min_th;
    public IntegerField $param_gred_max_th;
    public IntegerField $param_gred_max_p;
    public IntegerField $param_fq_codel_target;
    public IntegerField $param_fq_codel_interval;
    public IntegerField $param_fq_codel_quantum;
    public IntegerField $param_fq_codel_limit;
    public IntegerField $param_fq_codel_flows;
    public IntegerField $param_fq_pie_target;
    public IntegerField $param_fq_pie_tupdate;
    public IntegerField $param_fq_pie_alpha;
    public IntegerField $param_fq_pie_beta;
    public IntegerField $param_fq_pie_max_burst;
    public IntegerField $param_fq_pie_max_ecnth;
    public IntegerField $param_fq_pie_quantum;
    public IntegerField $param_fq_pie_limit;
    public IntegerField $param_fq_pie_flows;
    public IntegerField $delay;
    public FloatField $plr;
    public IntegerField $buckets;
    public NestedModelField $bandwidth;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->config_path = 'dnshaper/queue';
        $this->subsystem = 'shaper';
        $this->many = true;
        
        # Set model fields
        $this->name = new StringField(
            required: true,
            unique: true,
            validators: [new FilterNameValidator()],
            help_text: 'The unique name for this limiter.',
        );
        $this->number = new IntegerField(
            unique: true,
            default: rand(1, 65535), // TODO: Use a default callable for get_next_limiter_number here
            editable: false,
            read_only: true,
            help_text: 'A unique number auto-assigned to this limiter. This is only used internally by the system and ' .
                'cannot be manually set or changed.',
        );
        $this->enabled = new BooleanField(
            default: false,
            indicates_true: 'on',
            help_text: 'Enables or disables this limiter and its child queues.',
        );
        $this->mask = new StringField(
            default: 'none',
            choices: ['none', 'srcaddress', 'dstaddress'],
            help_text: 'If `source` or `destination` slots is chosen a dynamic pipe with the bandwidth, delay, ' .
                'packet loss and queue size given above will be created for each source/destination IP address ' .
                'encountered, respectively. This makes it possible to easily specify bandwidth limits per host ' .
                'or subnet.',
        );
        $this->maskbits = new IntegerField(
            default: 32,
            minimum: 1,
            maximum: 32,
            conditions: ['mask' => ['srcaddress', 'dstaddress']],
            help_text: 'The IPv4 mask bits to use when determine the scope of the dynamic pipe for IPv4 traffic.',
        );
        $this->maskbitsv6 = new IntegerField(
            default: 128,
            minimum: 1,
            maximum: 128,
            conditions: ['mask' => ['srcaddress', 'dstaddress']],
            help_text: 'The IPv6 mask bits to use when determine the scope of the dynamic pipe for IPv4 traffic.',
        );
        $this->qlimit = new IntegerField(
            default: null,
            allow_null: true,
            minimum: 1,
            help_text: "The length of the limiter's queue which the scheduler and AQM are responsible for. Set to " .
                '`null` to assume default.',
        );
        $this->ecn = new BooleanField(
            default: false,
            indicates_true: 'on',
            conditions: [
                'aqm' => ['codel', 'pie', 'red', 'gred'],
                'sched' => ['fq_codel', 'fq_pie'],
            ],
            help_text: 'Enable or disable ECN. ECN sets a reserved TCP flag when the queue is nearing or exceeding ' .
                'capacity. Not all AQMs or schedulers support this.',
        );
        $this->description = new StringField(
            default: '',
            allow_empty: true,
            help_text: 'The verbose description for this limiter.',
        );
        $this->aqm = new StringField(
            required: true,
            choices: ['droptail', 'codel', 'pie', 'red', 'gred'],
            help_text: 'The Active Queue Management (AQM) algorithm to use for this limiter. AQM is the intelligent ' .
                'drop of network packets inside the limiter, when it becomes full or gets close to becoming full, ' .
                'with the goal of reducing network congestion.',
        );
        $this->sched = new StringField(
            required: true,
            choices: ['wf2q+', 'fifo', 'qfq', 'rr', 'prio', 'fq_codel', 'fq_pie'],
            help_text: 'The scheduler to use for this limiter. The scheduler manages the sequence of network packets ' .
                "in the limiter's queue.",
        );
        $this->param_codel_target = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'codel'],
            help_text: 'The value for the CoDel target parameter.',
        );
        $this->param_codel_interval = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'codel'],
            help_text: 'The value for the CoDel interval parameter.',
        );
        $this->param_pie_target = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'pie'],
            help_text: 'The value for the PIE target parameter.',
        );
        $this->param_pie_tupdate = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'pie'],
            help_text: 'The value for the PIE tupdate parameter.',
        );
        $this->param_pie_alpha = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'pie'],
            help_text: 'The value for the PIE alpha parameter.',
        );
        $this->param_pie_beta = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'pie'],
            help_text: 'The value for the PIE beta parameter.',
        );
        $this->param_pie_max_burst = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'pie'],
            help_text: 'The value for the PIE max_burst parameter.',
        );
        $this->param_pie_max_ecnth = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'pie'],
            help_text: 'The value for the PIE ecnth parameter.',
        );
        $this->param_red_w_q = new IntegerField(
            default: 1,
            conditions: ['aqm' => 'red'],
            help_text: 'The value for the RED w_q parameter.',
        );
        $this->param_red_min_th = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'red'],
            help_text: 'The value for the RED min_th parameter.',
        );
        $this->param_red_max_th = new IntegerField(
            default: 1,
            conditions: ['aqm' => 'red'],
            help_text: 'The value for the RED max_th parameter.',
        );
        $this->param_red_max_p = new IntegerField(
            default: 1,
            conditions: ['aqm' => 'red'],
            help_text: 'The value for the RED max_p parameter.',
        );
        $this->param_gred_w_q = new IntegerField(
            default: 1,
            conditions: ['aqm' => 'gred'],
            help_text: 'The value for the GRED w_q parameter.',
        );
        $this->param_gred_min_th = new IntegerField(
            default: 0,
            conditions: ['aqm' => 'gred'],
            help_text: 'The value for the GRED min_th parameter.',
        );
        $this->param_gred_max_th = new IntegerField(
            default: 1,
            conditions: ['aqm' => 'gred'],
            help_text: 'The value for the GRED max_th parameter.',
        );
        $this->param_gred_max_p = new IntegerField(
            default: 1,
            conditions: ['aqm' => 'gred'],
            help_text: 'The value for the GRED max_p parameter.',
        );
        $this->param_fq_codel_target = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_codel'],
            help_text: 'The value for the FQ CoDel target parameter.',
        );
        $this->param_fq_codel_interval = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_codel'],
            help_text: 'The value for the FQ CoDel interval parameter.',
        );
        $this->param_fq_codel_quantum = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ['sched' => 'fq_codel'],
            help_text: 'The value for the FQ CoDel quantum parameter.',
        );
        $this->param_fq_codel_limit = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ['sched' => 'fq_codel'],
            help_text: 'The value for the FQ CoDel limit parameter.',
        );
        $this->param_fq_codel_flows = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ['sched' => 'fq_codel'],
            help_text: 'The value for the FQ CoDel flows parameter.',
        );
        $this->param_fq_pie_target = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE target parameter.',
        );
        $this->param_fq_pie_tupdate = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE tupdate parameter.',
        );
        $this->param_fq_pie_alpha = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE alpha parameter.',
        );
        $this->param_fq_pie_beta = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE beta parameter.',
        );
        $this->param_fq_pie_max_burst = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE max_burst parameter.',
        );
        $this->param_fq_pie_max_ecnth = new IntegerField(
            default: 0,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE ecnth parameter.',
        );
        $this->param_fq_pie_quantum = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE quantum parameter.',
        );
        $this->param_fq_pie_limit = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE limit parameter.',
        );
        $this->param_fq_pie_flows = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ['sched' => 'fq_pie'],
            help_text: 'The value for the FQ PIE flows parameter.',
        );
        $this->delay = new IntegerField(
            default: null,
            allow_null: true,
            minimum: 0,
            maximum: 10000,
            help_text: 'The amount of delay (in milliseconds) added to traffic passing through this limiter.',
        );
        $this->plr = new FloatField(
            default: null,
            allow_null: true,
            minimum: 0,
            maximum: 1,
            help_text: 'The amount of packet loss (in percentage) added to traffic passing through the limiter.',
        );
        $this->buckets = new IntegerField(
            default: null,
            allow_null: true,
            minimum: 0,
            maximum: 65535,
            help_text: "The limiter's bucket size (slots).",
        );
        $this->bandwidth = new NestedModelField(
            model_class: 'TrafficShaperLimiterBandwidth',
            default: [],
            allow_empty: true,
            help_text: 'The bandwidth profiles for this limiter.',
        );

        parent::__construct($id, $parent_id, $data, $options);
    }

    /**
     * Reloads the filter, and resets applicable RRD graphs
     */
    public function apply(): void
    {
        (new TrafficShaperApplyDispatcher(async: $this->async))->spawn_process();
    }
}
