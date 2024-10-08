<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\StringField;
use RESTAPI\ModelTraits\LogFileModelTraits;

/**
 * Defines a Model for interacting with the DHCP log at /var/log/dhcpd.log.
 */
class DHCPLog extends Model {
    use LogFileModelTraits;

    /**
     * @var string $log_file The path to the base DHCP log file this Model will read. Any log files that have been rotated
     * will be read as well.
     */
    public string $log_file = '/var/log/dhcpd.log';
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
    protected function get_dhcp_log(): array {
        return $this->read_log($this->log_file);
    }
}
