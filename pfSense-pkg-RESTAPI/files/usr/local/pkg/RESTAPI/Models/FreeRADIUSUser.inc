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
 * Defines a Model that represents OpenVPN Client config Export.
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

    /**
     *
     */
    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        #
        # Set model attributes
        #
        $this->packages = ['pfSense-pkg-freeradius3'];
        $this->package_includes = ['freeradius.inc'];
        $this->config_path = 'installedpackages/freeradius/config';
        $this->many = true;
        $this->always_apply = true;

        #
        # Set model fields
        #
        $this->username = new StringField(required: true, unique: true, internal_name: 'varusersusername');

        $this->password = new StringField(
            required: true,
            conditions: ['motp_enable' => 'off'],
            allow_empty: false,
            allow_null: false,
            internal_name: 'varuserspassword',
            sensitive: true,
        );
        $this->password_encryption = new StringField(
            required: false,
            conditions: ['motp_enable' => 'off'],
            choices: ['Cleartext-Password', 'MD5-Password', 'MD5-Password-hashed', 'NT-Password-hashed'],
            default: 'Cleartext-Password',
            internal_name: 'varuserspasswordencryption',
        );

        $this->motp_enable = new StringField(
            required: true,
            choices: ['on', 'off'],
            internal_name: 'varusersmotpenable',
        );
        $this->motp_authmethod = new StringField(
            required: false,
            conditions: ['motp_enable' => 'on'],
            choices: ['motp', 'googleauth'],
            default: 'googleauth',
            internal_name: 'varusersauthmethod',
        );
        $this->motp_secret = new StringField(
            required: true,
            conditions: ['motp_enable' => 'on'],
            allow_null: false,
            internal_name: 'varusersmotpinitsecret',
            sensitive: true,
        );
        $this->motp_pin = new StringField(
            required: true,
            conditions: ['motp_enable' => 'on'],
            allow_null: false,
            minimum_length: 4,
            maximum_length: 4,
            internal_name: 'varusersmotppin',
            sensitive: true,
        );
        $this->motp_offset = new IntegerField(
            required: false,
            conditions: ['motp_enable' => 'on'],
            allow_null: false,
            default: 0,
            internal_name: 'varusersmotpoffset',
        );

        $this->description = new StringField(
            required: false,
            allow_empty: true,
            default: '',
            validators: [
                new RegexValidator(
                    pattern: "/^[a-zA-Z0-9 _,.;:+=()-]*$/",
                    error_msg: 'Value contains invalid characters.',
                ),
            ],
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
