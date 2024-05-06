<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\StringField;
use RESTAPI\Fields\UnixTimeField;

class ConfigHistoryRevision extends Model
{
    public UnixTimeField $time;
    public StringField $description;
    public StringField $version;
    public IntegerField $filesize;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->internal_callable = 'get_config_history';
        $this->verbose_name = 'Configuration History Entry';
        $this->many = true;

        # Set model fields
        $this->time = new UnixTimeField(
            required: true,
            help_text: "The time the configuration change was made."
        );
        $this->description = new StringField(
            default: '',
            help_text: "The description of the configuration change."
        );
        $this->version = new StringField(
            default: '',
            help_text: "The configuration version associated with this change."
        );
        $this->filesize = new IntegerField(
            default: 0,
            help_text: "The file size (in bytes) of the configuration file associated with this change."
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Deletes a configuration history entry.
     */
    public function _delete(): void {
        unlink("/cf/conf/backup/config-$this->time.xml");
    }
}