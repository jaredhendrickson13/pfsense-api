<?php

namespace RESTAPI\Validators;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Validator;
use RESTAPI\Responses\ValidationError;

/**
 * Defines a Validator that determines if a given string is an X509 encoded object
 */
class X509Validator extends Validator {
    const CRT_BEGIN = 'BEGIN CERTIFICATE';
    const CRT_END = 'BEGIN CERTIFICATE';
    const CSR_BEGIN = 'BEGIN CERTIFICATE REQUEST';
    const CSR_END = 'END CERTIFICATE REQUEST';
    const CSR_BEGIN_NEW = 'BEGIN NEW CERTIFICATE REQUEST';
    const CSR_END_NEW = 'END NEW CERTIFICATE REQUEST';
    const PRV_BEGIN = 'BEGIN PRIVATE KEY';
    const PRV_END = 'END PRIVATE KEY';
    const ECPRV_BEGIN = 'BEGIN EC PRIVATE KEY';
    const ECPRV_END = 'END EC PRIVATE KEY';

    /**
     * Constructs the X509Validator object.
     * @param bool $allow_crt Set to true to consider X509 Certificate strings to be valid.
     * @param bool $allow_csr Set to true to consider X509 Certificate Signing Request strings to be valid.
     * @param bool $allow_prv Set to true to consider X509 Private Key strings to be valid.
     * @param bool $allow_ecprv Set to true to consider X509 EC Private Key strings to be valid.
     */
    public function __construct(
        public bool $allow_crt = true,
        public bool $allow_csr = false,
        public bool $allow_prv = false,
        public bool $allow_ecprv = false,
    ) {
    }

    /**
     * Checks if a given value is a valid X509 string.
     * @param mixed $value The incoming value to be validated.
     * @param string $field_name The name of the field being validated.
     * @throws ValidationError When the $value is not a valid X509 string.
     */
    public function validate(mixed $value, string $field_name = ''): void {
        # Accept X509 certificates if allowed
        if ($this->allow_crt and str_contains($value, self::CRT_BEGIN) and str_contains($value, self::CRT_END)) {
            return;
        }
        # Accept X509 CSRs if allowed
        if ($this->allow_csr and str_contains($value, self::CSR_BEGIN) and str_contains($value, self::CSR_END)) {
            return;
        }
        # Accept X509 New CSRs if allowed
        if (
            $this->allow_csr and
            str_contains($value, self::CSR_BEGIN_NEW) and
            str_contains($value, self::CSR_END_NEW)
        ) {
            return;
        }
        # Accept X509 Private Keys if allowed
        if ($this->allow_prv and str_contains($value, self::PRV_BEGIN) and str_contains($value, self::PRV_END)) {
            return;
        }
        # Accept X509 EC Private Keys if allowed
        if ($this->allow_ecprv and str_contains($value, self::ECPRV_BEGIN) and str_contains($value, self::ECPRV_END)) {
            return;
        }

        # Throw a validation error if no previous statements were matched
        throw new ValidationError(
            message: "Field `$field_name` is not a valid X509 string. Valid X509 types are: [{$this->get_options_str()}]",
            response_id: 'X509_VALIDATOR_INVALID_VALUE',
        );
    }

    /**
     * Uses the `allow_*` properties to format a string of value options this validator will accept.
     */
    private function get_options_str(): string {
        $options = [];
        $options += $this->allow_crt ? ['certificate'] : [];
        $options += $this->allow_csr ? ['certificate signing request'] : [];
        $options += $this->allow_prv ? ['private key'] : [];
        $options += $this->allow_ecprv ? ['EC private key'] : [];
        return implode(', ', $options);
    }

    /**
     * Checks if a given public key and private key are matching key pairs.
     * @param string $public_key The x509 public key to evaluate.
     * @param string $private_key the x509 private key to evaluate
     * @returns bool true if the public key and private key are matching key pairs.
     */
    public static function is_matching_keypair(string $public_key, string $private_key): bool {
        $crt_public_key = cert_get_publickey($public_key, decode: false, type: 'crt');
        $prv_public_key = cert_get_publickey($private_key, decode: false, type: 'prv');
        return $crt_public_key === $prv_public_key;
    }
}
