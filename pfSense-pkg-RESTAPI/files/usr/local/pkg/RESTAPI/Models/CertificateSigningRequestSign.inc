<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\Base64Field;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\StringField;
use RESTAPI\Fields\UIDField;
use RESTAPI\ModelTraits\CertificateModelTraits;
use RESTAPI\Responses\ServerError;
use RESTAPI\Validators\EmailAddressValidator;
use RESTAPI\Validators\HostnameValidator;
use RESTAPI\Validators\IPAddressValidator;
use RESTAPI\Validators\RegexValidator;
use RESTAPI\Validators\URLValidator;
use RESTAPI\Validators\X509Validator;

/**
 * Defines a Model for signing existing CSRs.
 */
class CertificateSigningRequestSign extends Model {
    use CertificateModelTraits;

    public StringField $descr;
    public UIDField $refid;
    public ForeignModelField $caref;
    public Base64Field $csr;
    public Base64Field $crt;
    public Base64Field $prv;
    public StringField $digest_alg;
    public IntegerField $lifetime;
    public StringField $type;
    public StringField $dn_dns_sans;
    public StringField $dn_email_sans;
    public StringField $dn_ip_sans;
    public StringField $dn_uri_sans;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # Set model attributes
        $this->config_path = 'cert';
        $this->many = true;
        $this->always_apply = true;

        # Set model fields
        $this->descr = new StringField(
            required: true,
            validators: [new RegexValidator(pattern: "/[\?\>\<\&\/\\\"\']/", invert: true)],
            help_text: 'The descriptive name for this certificate.',
        );
        $this->refid = new UIDField(
            help_text: 'The unique ID assigned to this certificate for internal system use. This value is ' .
                'generated by this system and cannot be changed.',
        );
        $this->caref = new ForeignModelField(
            model_name: 'CertificateAuthority',
            model_field: 'refid',
            required: true,
            help_text: 'The certificate authority to sign the certificate with.',
        );
        $this->csr = new Base64Field(
            required: true,
            representation_only: true,
            validators: [new X509Validator(allow_crt: false, allow_csr: true)],
            help_text: 'The X509 certificate signing request to sign.',
        );
        $this->crt = new Base64Field(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: 'The X509 certificate string.',
        );
        $this->prv = new Base64Field(
            default: null,
            allow_null: true,
            sensitive: true,
            help_text: 'The X509 private key string.',
        );
        $this->digest_alg = new StringField(
            required: true,
            choices_callable: 'get_digest_alg_choices',
            representation_only: true,
            help_text: 'The digest method used when the certificate is signed.',
        );
        $this->lifetime = new IntegerField(
            default: 3650,
            representation_only: true,
            minimum: 1,
            maximum: 12000,
            help_text: 'The number of days the certificate is valid for.',
        );
        $this->type = new StringField(
            default: 'user',
            choices: ['server', 'user'],
            help_text: 'The type of certificate to generate.',
        );
        $this->dn_dns_sans = new StringField(
            default: [],
            allow_empty: true,
            representation_only: true,
            many: true,
            validators: [new HostnameValidator(allow_hostname: true, allow_domain: true, allow_fqdn: true)],
            help_text: 'The DNS Subject Alternative Names (SANs) for the certificate.',
        );
        $this->dn_email_sans = new StringField(
            default: [],
            allow_empty: true,
            representation_only: true,
            many: true,
            validators: [new EmailAddressValidator()],
            help_text: 'The Email Subject Alternative Names (SANs) for the certificate.',
        );
        $this->dn_ip_sans = new StringField(
            default: [],
            allow_empty: true,
            representation_only: true,
            many: true,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: 'The IP Subject Alternative Names (SANs) for the certificate.',
        );
        $this->dn_uri_sans = new StringField(
            default: [],
            allow_empty: true,
            representation_only: true,
            many: true,
            validators: [new URLValidator()],
            help_text: 'The URI Subject Alternative Names (SANs) for the certificate.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Extends the default _create method to ensure the certificate is generated before it is written to config.
     */
    protected function _create(): void {
        # Sign the CSR
        $this->sign_csr();

        # Call the parent _create method to write the certificate to config
        parent::_create();
    }

    /**
     * Signs a CSR using the requested parameters. This populates the `crt` fields
     * @throws ServerError When the CSR fails to be signed.
     */
    private function sign_csr(): void {
        global $config;

        # Define a placeholder for sign_csr() to populate and obtain the CA config
        $ca = $this->caref->get_related_model();
        $ca_conf = $ca->to_internal();

        # Generate the CSR and key pair
        $cert = csr_sign(
            csr: $this->csr->value,
            ca: $ca_conf,
            duration: $this->lifetime->value,
            type: $this->type->value,
            altnames: $this->to_x509_dn($this)['subjectAltName'],
            digest_alg: $this->digest_alg->value,
        );

        # Throw a server error if the CSR fails to be signed
        if (!$cert) {
            throw new ServerError(
                message: 'Failed to sign the certificate signing request for unknown reason.',
                response_id: 'CERTIFICATE_SIGNING_REQUEST_SIGN_FAILED',
            );
        }

        # Populate the `crt` fields with the generated values and assign changes to our CA
        $this->crt->value = $cert;
        $this->set_config("ca/$ca->id/serial", $ca->serial->value + 1);
    }
}
