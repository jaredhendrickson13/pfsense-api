<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\StringField;

/**
 * Defines a Model for interacting with the firewall log at /var/log/filter.log.
 */
class FirewallLog extends Model
{
    public StringField $text;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
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
        $firewall_log = [];
        $log = file_get_contents('/var/log/filter.log');
        $log_lines = explode("\n", $log);
        $log = array_filter($log_lines);
        unset($log[count($log_lines) - 1]);
        foreach ($log_lines as $line) {
            $firewall_log[] = ['text' => $line];
        }
        return $firewall_log;
    }
}