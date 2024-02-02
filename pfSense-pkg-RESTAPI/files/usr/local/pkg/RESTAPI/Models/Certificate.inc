<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Fields\Base64Field;
use RESTAPI\Fields\StringField;
use RESTAPI\Fields\UIDField;
use RESTAPI\Responses\ForbiddenError;
use RESTAPI\Responses\ValidationError;
use RESTAPI\Validators\RegexValidator;
use RESTAPI\Validators\X509Validator;

class Certificate extends Model
{
    public StringField $descr;
    public UIDField $refid;
    public StringField $type;
    public Base64Field $crt;
    public Base64Field $prv;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options)
    {
        # Set model attributes
        $this->config_path = 'cert';
        $this->many = true;

        # Set model fields
        $this->descr = new StringField(
            required: true,
            validators: [new RegexValidator(pattern: "/[\?\>\<\&\/\\\"\']/", invert: true)],
            help_text: 'The descriptive name for this certificate.',
        );
        $this->refid = new UIDField(
            help_text: 'The unique ID assigned to this certificate for internal system use. This value is generated ' .
                'by this system and cannot be changed.',
        );
        $this->type = new StringField(
            default: 'server',
            choices: ['server', 'user'],
            help_text: 'The certificate type. Use `server` when this certificate is to be used by one or more ' .
                'services on this system. Use `user` when this certificate is intended to be assigned to a user for ' .
                'authentication purposes.',
        );
        $this->crt = new Base64Field(
            required: true,
            validators: [new X509Validator(allow_crt: true)],
            help_text: 'The X509 certificate string.',
        );
        $this->prv = new Base64Field(
            required: true,
            write_only: true,
            validators: [new X509Validator(allow_prv: true, allow_ecprv: true)],
            help_text: 'The X509 private key string.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Adds extra validation to the `prv` field.
     * @param string $prv The incoming value to be validated.
     * @returns string The validated value to be set.
     * @throws ValidationError When $prv and the `crt` field's current value are not a matching key pair
     */
    public function validate_prv(string $prv): string
    {
        # Throw a validation error if $prv and the `crt` field's current value are not a matching key pair
        if (!X509Validator::is_matching_keypair(public_key: $this->crt->value, private_key: $prv)) {
            throw new ValidationError(
                message: 'Field `prv` and field `crt` values must be a matching key pair.',
                response_id: 'CERTIFICATE_CRT_AND_PRV_NOT_KEYPAIR',
            );
        }

        return $prv;
    }

    /**
     * Deletes this Certificate object from configuration.
     * @throws ForbiddenError When the Certificate cannot be deleted because it is in use.
     */
    public function _delete(): void
    {
        # Do not allow this Certificate to be deleted if it is in use
        if (cert_in_use($this->refid->value)) {
            throw new ForbiddenError(
                message: 'Certificate cannot be deleted because it is in use by one or more services.',
                response_id: 'CERTIFICATE_CANNOT_BE_DELETED_WHILE_IN_USE',
            );
        }

        parent::_delete();
    }
}
