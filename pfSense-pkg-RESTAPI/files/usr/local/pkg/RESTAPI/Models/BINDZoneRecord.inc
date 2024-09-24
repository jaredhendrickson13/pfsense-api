<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\BINDApplyDispatcher;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\SubnetValidator;

/**
 * Defines a model that interacts with BIND zone records.
 */
class BINDZoneRecord extends Model {
    public StringField $name;
    public StringField $type;
    public StringField $data;
    public IntegerField $priority;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->parent_model_class = 'BINDZone';
        $this->config_path = 'row';
        $this->packages = ['pfSense-pkg-bind'];
        $this->package_includes = ['bind.inc'];
        $this->many = true;
        $this->always_apply = true;

        # Set model fields
        $this->name = new StringField(
            required: true,
            internal_name: 'hostname',
            help_text: 'The domain name for this record.',
        );
        $this->type = new StringField(
            required: true,
            choices: ['A', 'AAAA', 'CNAME', 'MX', 'NS', 'LOC', 'PTR', 'SRV', 'TXT', 'SPF'],
            internal_name: 'hosttype',
            help_text: 'The type of record.',
        );
        $this->data = new StringField(
            required: true,
            allow_empty: true,
            internal_name: 'hostdst',
            help_text: 'The data for this record. This can be an IP address, domain name, or other data depending on the record type.',
        );
        $this->priority = new IntegerField(
            required: false,
            internal_name: 'hostvalue',
            conditions: ['type' => ['MX', 'SRV']],
            help_text: 'The priority for this record.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Applies changes to the BIND zone records
     */
    public function apply(): void {
        (new BINDApplyDispatcher(async: $this->async))->spawn_process();
    }
}
