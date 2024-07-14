<?php

namespace RESTAPI\Models;

use RESTAPI\Core\LogFileTraits;
use RESTAPI\Core\Model;
use RESTAPI\Fields\StringField;

/**
 * Defines a Model for interacting with the firewall log at /var/log/filter.log.
 */
class FirewallLog extends Model {
    use LogFileTraits;

    public StringField $text;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->internal_callable = 'get_firewall_log';
        $this->many = true;

        $this->text = new StringField(default: '', help_text: 'The raw text of the firewall log entry.');

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Obtains the firewall log as an array. This method is the internal callable for this Model.
     * @return array The firewall log as an array of objects.
     */
    protected function get_firewall_log(): array {
        return $this->read_log('/var/log/filter.log');
    }
}
