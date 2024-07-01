<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\StringField;

/**
 * Defines a Model for interacting with the DHCP log at /var/log/dhcpd.log.
 */
class DHCPLog extends Model {
    public StringField $text;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->internal_callable = 'get_dhcp_log';
        $this->many = true;

        $this->text = new StringField(default: '', help_text: 'The raw text of the DHCP log entry.');

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Obtains the DHCP log as an array. This method is the internal callable for this Model.
     * @return array The DHCP log as an array of objects.
     */
    protected function get_DHCP_log(): array {
        $dhcp_log = [];
        $log = file_get_contents('/var/log/dhcpd.log');
        $log_lines = explode("\n", $log);
        $log = array_filter($log_lines);
        unset($log[count($log_lines) - 1]);
        foreach ($log_lines as $line) {
            $dhcp_log[] = ['text' => $line];
        }
        return $dhcp_log;
    }
}
