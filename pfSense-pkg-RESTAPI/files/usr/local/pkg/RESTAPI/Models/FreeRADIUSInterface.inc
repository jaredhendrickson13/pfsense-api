<?php

namespace RESTAPI\Models;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;
use RESTAPI\Fields\Base64Field;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\ObjectField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ConflictError;
use RESTAPI\Responses\ValidationError;
use RESTAPI\Responses\ServerError;
use RESTAPI\Validators\HostnameValidator;
use RESTAPI\Validators\IPAddressValidator;
use RESTAPI\Validators\RegexValidator;

/**
 * Defines a Model that represents FreeRADIUS Interfaces
 */
class FreeRADIUSInterface extends Model {

    public StringField $addr;
    public PortField $port;
    public StringField $type;
    public StringField $ipv;
    public StringField $description;

    /**
     *
     */
    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        #
        # Set model attributes
        #
        $this->packages = ['pfSense-pkg-freeradius3'];
        $this->package_includes = ['freeradius.inc'];
        $this->config_path = 'installedpackages/freeradiusinterfaces/config';
        $this->many = true;
        $this->always_apply = true;
        $this->unique_together_fields = ['addr', 'port', 'ipv'];

        #
        # Set model fields
        #
        $this->addr = new StringField(
            required: true,
//            unique: true,
            internal_name: 'varinterfaceip',
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true, allow_keywords: ['*'])],
            help_text: 'The IP address of the listening interface. If you choose * then it means all interfaces.'
        );
        $this->port = new PortField(
            required: false,
            internal_name: 'varinterfaceport',
            allow_alias: false,
            allow_range: false,
            default: '1812',
            help_text: 'The port number of the listening interface. Different interface types need different ports.'
        );
        $this->type = new StringField(
            required: false,
            internal_name: 'varinterfacetype',
            choices: [ 'auth', 'acct' ],
            default: 'auth',
            help_text: 'The type of the listening interface: Authentication/Accounting.'
        );
        $this->ipv = new StringField(
            internal_name: 'varinterfaceipversion',
            choices: [ 'ipaddr', 'ipv6addr' ],
            allow_empty: true,
            default: 'ipaddr',
            help_text: 'The IP version of the listening interface.'
        );
        $this->description = new StringField(
            required: false,
            allow_empty: true,
            default: "",
            validators: [
                new RegexValidator(pattern: "/^[a-zA-Z0-9 _,.;:+=()-]*$/", error_msg: 'Value contains invalid characters.'),
            ],
            help_text: 'The description for this interface.'
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }


    /**
     * Perform additional validation on the Model's fields and data.
     */
    public function validate_extra(): void {
        $input_errors = [];

        $iface_addr = $this->addr->value;
        if ( $iface_addr != '*' ) {
            if ( is_ipaddrv4($iface_addr) ) {
                $this->ipv->value = 'ipaddr';
            } elseif ( is_ipaddrv6($iface_addr) ) {
                $this->ipv->value = 'ipv6addr';
            } else {
                // we don't must be here because Model validator for $this->addr
                $input_errors[] = "Cann't recognize IP-address={$iface_addr}";
            }
        }

        # Run service level validations
        $iface = $this->to_internal();
        freeradius_validate_interfaces($iface, $input_errors);

        # If there were validation errors that were not caught by the model fields, throw a ValidationError.
        # Ideally the Model should catch all validation errors itself so prompt the user to report this error
        if (!empty($input_errors)) {
            throw new ValidationError(
                message: "An unexpected validation error has occurred: $input_errors[0]. Please report this issue at " .
                    'https://github.com/jaredhendrickson13/pfsense-api/issues/new',
                response_id: 'FREERADIUS_USER_UNEXPECTED_VALIDATION_ERROR',
            );
        }
    }


    /**
     * Apply the creation of this User.
     */
    public function apply() {
        freeradius_settings_resync();
    }
}
