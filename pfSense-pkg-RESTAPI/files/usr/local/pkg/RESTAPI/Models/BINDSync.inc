<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\BINDApplyDispatcher;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\IPAddressValidator;

/**
 * Defines a model for interacting with the BIND sync settings.
 */
class BINDSync extends Model {
    public StringField $synconchanges;
    public IntegerField $synctimeout;
    public StringField $masterip;
    public NestedModelField $remotehosts;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->config_path = 'installedpackages/bindsync/config';
        $this->packages = ['pfSense-pkg-bind'];
        $this->package_includes = ['bind.inc'];
        $this->many = false;
        $this->always_apply = true;

        # Set model fields
        $this->synconchanges = new StringField(
            required: true,
            choices: ['disabled', 'manual', 'auto'],
            help_text: 'The sync mode to use.',
        );
        $this->synctimeout = new IntegerField(
            default: 30,
            choices: [30, 60, 90, 120, 150, 250],
            help_text: 'The timeout for the sync process.',
        );
        $this->masterip = new StringField(
            required: true,
            validators: [new IPAddressValidator(allow_fqdn: true)],
            help_text: 'The IP address of the master BIND server.',
        );
        $this->remotehosts = new NestedModelField(
            model_class: 'BINDSyncRemoteHost',
            default: [],
            help_text: 'The remote hosts to sync with.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Applies changes to the BIND sync settings.
     */
    public function apply(): void {
        (new BINDApplyDispatcher(async: $this->async))->spawn_process();
    }
}
