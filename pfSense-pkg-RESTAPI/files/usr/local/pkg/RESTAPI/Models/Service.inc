<?php

namespace RESTAPI\Models;

use RESTAPI;
use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\StringField;

/**
 * Defines a Model that represents the status of services on this system.
 */
class Service extends Model {
    public StringField $name;
    public StringField $action;
    public StringField $description;
    public BooleanField $enabled;
    public BooleanField $status;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # Set Model attributes
        $this->internal_callable = 'get_services';
        $this->many = true;

        # Set Model Fields
        $this->name = new StringField(
            required: true,
            choices_callable: 'get_service_name_choices',
            help_text: 'The internal name of the service.',
        );
        $this->action = new StringField(
            required: true,
            choices: ['start', 'stop', 'restart'],
            write_only: true,
            help_text: 'The action to perform against this service.',
        );
        $this->description = new StringField(read_only: true, help_text: 'The full descriptive name of the service.');
        $this->enabled = new BooleanField(
            read_only: true,
            indicates_true: true,
            indicates_false: false,
            help_text: 'Indicates if the service is enabled.',
        );
        $this->status = new BooleanField(
            read_only: true,
            indicates_true: true,
            indicates_false: false,
            help_text: 'Indicates if the service is actively running.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Obtains all available services and their operating status. This method is the internal callable that populates
     * the internal field values for this Model.
     * @return array An array of available pfSense services and their current statuses.
     */
    public static function get_services(): array {
        # Variables
        $services = get_services();

        # Loop through each service and set the proper values
        foreach ($services as $id => $service) {
            # Ensure the $enabled and $status values are always present
            $services[$id]['enabled'] = is_service_enabled($service['name']);
            $services[$id]['status'] = is_service_running($service['name']);
        }

        return $services;
    }

    /**
     * Obtains all internal service choices for the `name` field.
     * @return array An array of available service names.
     */
    public function get_service_name_choices(): array {
        $choices = [];
        foreach ($this->get_services() as $service) {
            $choices[] = $service['name'];
        }
        return $choices;
    }

    /**
     * Gets the representation ID for a given service name.
     * @param string $name The internal service to obtain the ID for.
     * @return int The representation ID of the service with this name.
     */
    public function get_id_by_name(string $name): int {
        return $this->query(['name' => $name])->first()->id;
    }

    /**
     * Starts or stops a specified service.
     */
    public function _create() {
        # Trigger the requested service action
        switch ($this->action->value) {
            case 'start':
                service_control_start(name: $this->name->value, extras: []);
                break;
            case 'stop':
                service_control_stop(name: $this->name->value, extras: []);
                break;
            case 'restart':
                service_control_restart(name: $this->name->value, extras: []);
                break;
        }

        # Populate the current status of the service with this name so this object is up-to-date.
        $this->id = $this->get_id_by_name($this->name->value);
        $this->from_internal();
    }
}
