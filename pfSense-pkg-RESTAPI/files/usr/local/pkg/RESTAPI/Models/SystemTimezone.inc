<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a Model that represents the Timezone configuration on this system.
 */
class SystemTimezone extends Model {
    public StringField $timezone;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # Set model attributes
        $this->config_path = 'system';
        $this->update_strategy = 'merge';
        $this->always_apply = true;

        # Set model Fields
        $this->timezone = new StringField(
            default: 'Etc/GMT',
            allow_empty: false,
            allow_null: false,
            many: false,
            help_text: 'Set geographic region name (Continent/Location) to determine the timezone for the firewall.'
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Apply these Timezone changes to the system.
     */
    public function apply() {
        system_timezone_configure();
    }

    /*
     *
     */
    public function validate_extra(): void {
        $tzlist = system_get_timezone_list();

        $tz = $this->timezone->value;

        if ( ! in_array($tz, $tzlist) ) {
            throw new ValidationError(
                message: "Unknown timezone={$tz}",
                response_id: 'TIMEZONE_UNKNOWN_TIMEZONE',
            );
        }
    }
}
