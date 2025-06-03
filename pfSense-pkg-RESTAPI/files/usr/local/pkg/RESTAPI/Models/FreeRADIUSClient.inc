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
class FreeRADIUSClient extends Model {

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
        $this->config_path = 'installedpackages/freeradiusclients/config';
        $this->many = true;
        $this->always_apply = true;

        #
        # Set model fields
        #
        $this->addr = new StringField(
            internal_name: 'varclientip',
            required: true,
            unique: true,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: 'The IP address or network of the RADIUS client(s) in CIDR notation. This is the IP of the NAS (switch, access point, firewall, router, etc.)'
        );
        $this->ipv = new StringField(
            internal_name: 'varclientipversion',
            choices: [ 'ipaddr', 'ipv6addr' ],
            allow_empty: true,
            default: 'ipaddr',
            help_text: 'The IP version of the this Client.'
        );
        $this->shortname = new StringField(
            internal_name: 'varclientshortname',
            required: true,
            allow_null: false,
            help_text: 'A short name for the client. This is generally the hostname of the NAS.'
        );
        $this->secret = new StringField(
            internal_name: 'varclientsharedsecret',
            required: true,
            sensitive: true,
            allow_empty: false,
            help_text: 'This is the shared secret (password) which the NAS (switch, accesspoint, etc.) needs to communicate with the RADIUS server.'
        );

        $this->proto = new StringField(
            internal_name: 'varclientproto',
            choices: ['udp', 'tcp'],
            allow_empty: true,
            default: 'udp',
            help_text: 'The protocol the client uses. (Default: udp)'
        );
        $this->nastype = new StringField(
            internal_name: 'varclientnastype',
            choices: ['cisco', 'cvx', 'computone', 'digitro', 'livingston', 'juniper', 'max40xx', 'mikrotik', 'mikrotik_snmp', 'dot1x', 'other'],
            allow_empty: true,
            default: 'other',
            help_text: 'The NAS type of the client. This is used by checkrad.pl for simultaneous use checks. (Default: other)'
        );
        $this->msgauth = new StringField(
            internal_name: 'varrequiremessageauthenticator',
            choices: ['yes', 'no'],
            default: 'no',
            help_text: 'RFC5080 requires Message-Authenticator in Access-Request. But older NAS (switches or accesspoints) do not include that. (Default: no)'
        );
        $this->maxconn = new IntegerField(
            internal_name: 'varclientmaxconnections',
            minimum: 1,
            maximum: 32,
            default: 16,
            help_text: 'Takes only effect if you use TCP as protocol. Limits the number of simultaneous TCP connections from a client. (max=32)'
        );
        $this->naslogin = new StringField(
            internal_name: 'varclientlogininput',
            allow_empty: true,
            default: '',
            help_text: 'If supported by your NAS, you can use SNMP or finger for simultaneous-use checks instead of (s)radutmp file and accounting.  Leave empty to choose (s)radutmp. (Default: empty) '
        );
        $this->naspassword= new StringField(
            internal_name: 'varclientpasswordinput',
            allow_empty: true,
            default: '',
            sensitive: true,
            help_text: 'If supported by your NAS, you can use SNMP or finger for simultaneous-use checks instead of (s)radutmp file and accounting.  Leave empty to choose
(s)radutmp. (Default: empty) '
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

/*
*/
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
        $client = $this->to_internal();
        freeradius_validate_clients($iface, $input_errors);

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
     * Apply specific action on Client(s)
     */
    public function apply() {
        freeradius_clients_resync();
    }
}
