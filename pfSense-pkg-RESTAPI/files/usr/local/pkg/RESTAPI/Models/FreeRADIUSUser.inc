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
 * Defines a Model that represents FreeRADIUS Users
 */
class FreeRADIUSUser extends Model {
    public StringField $username;
    public StringField $password;
    public StringField $password_encryption;
    public StringField $motp_enable;
    public StringField $motp_authmethod;
    public StringField $motp_secret;
    public StringField $motp_pin;
    public IntegerField $motp_offset;
    public StringField $description;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # Set model attributes
        $this->packages = ['pfSense-pkg-freeradius3'];
        $this->package_includes = ['freeradius.inc'];
        $this->config_path = 'installedpackages/freeradius/config';
        $this->many = true;
        $this->always_apply = true;

        # Set model fields
        $this->username = new StringField(
            required: true,
            unique: true,
            internal_name: 'varusersusername',
            help_text: 'The username for this user.',
        );
        $this->password = new StringField(
            required: true,
            allow_empty: false,
            allow_null: false,
            sensitive: true,
            internal_name: 'varuserspassword',
            conditions: ['motp_enable' => 'off'],
            help_text: 'The password for this username.',
        );
        $this->password_encryption = new StringField(
            required: false,
            default: 'Cleartext-Password',
            choices: ['Cleartext-Password', 'MD5-Password', 'MD5-Password-hashed', 'NT-Password-hashed'],
            internal_name: 'varuserspasswordencryption',
            conditions: ['motp_enable' => 'off'],
            help_text: 'The encryption method for the password.',
        );
        $this->motp_enable = new StringField(
            required: true,
            choices: ['on', 'off'],
            internal_name: 'varusersmotpenable',
            help_text: 'Enable or disable the use of Mobile One-Time Password (MOTP) for this user.',
        );
        $this->motp_authmethod = new StringField(
            required: false,
            default: 'googleauth',
            choices: ['motp', 'googleauth'],
            internal_name: 'varusersauthmethod',
            conditions: ['motp_enable' => 'on'],
            help_text: 'The authentication method for the Mobile One-Time Password (MOTP).',
        );
        $this->motp_secret = new StringField(
            required: true,
            allow_null: false,
            sensitive: true,
            internal_name: 'varusersmotpinitsecret',
            conditions: ['motp_enable' => 'on'],
            help_text: 'The secret for the Mobile One-Time Password (MOTP).',
        );
        $this->motp_pin = new StringField(
            required: true,
            allow_null: false,
            sensitive: true,
            minimum_length: 4,
            maximum_length: 4,
            internal_name: 'varusersmotppin',
            conditions: ['motp_enable' => 'on'],
            help_text: 'The PIN for the Mobile One-Time Password (MOTP). It must be exactly 4 digits.',
        );
        $this->motp_offset = new IntegerField(
            required: false,
            default: 0,
            allow_null: false,
            internal_name: 'varusersmotpoffset',
            conditions: ['motp_enable' => 'on'],
            help_text: 'The timezone offset for this user.',
        );
        $this->description = new StringField(
            required: false,
            default: '',
            allow_empty: true,
            validators: [
                new RegexValidator(
                    pattern: "/^[a-zA-Z0-9 _,.;:+=()-]*$/",
                    error_msg: 'Value contains invalid characters.',
                ),
            ],
            help_text: 'A description for this user.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     *
     */
    public function _create() {
        $input_errors = [];

        if ($this->motp_enable->value == 'off') {
            $this->motp_enable->value = '';
        }

        $user = $this->to_internal();

        freeradius_validate_users($user, $input_errors);

        if (!empty($input_errors)) {
            throw new ServerError(
                message: "Some errors occured: input_errors={$input_errors[0]}",
                response_id: 'FIELD_INVALID_CHOICE',
            );
        }

        parent::_create();
    }

    /**
     * Apply the creation of this User.
     */
    public function apply_create() {
        freeradius_users_resync();
    }

    /**
     * Apply the deletion of this User.
     */
    public function apply_delete() {
        freeradius_users_resync();
    }
}
