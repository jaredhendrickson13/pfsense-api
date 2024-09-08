<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\CertificateAuthorityGenerate;
use RESTAPI\Validators\X509Validator;

class APIModelsCertificateAuthorityGenerateTestCase extends TestCase
{
    /**
     * Ensure we can generate a new Certificate Authority with an RSA key.
     */
    public function test_generate_rsa_certificate_authority(): void {
        $ca = new CertificateAuthorityGenerate(
            descr: 'test_ca',
            trust: true,
            randomserial: true,
            is_intermediate: false,
            keytype: 'RSA',
            keylen: 2048,
            digest_alg: 'sha256',
            lifetime: 3650,
            dn_country: 'US',
            dn_state: 'UT',
            dn_city: 'Salt Lake City',
            dn_organization: 'ACME Org',
            dn_organizationalunit: 'IT'
        );
        $ca->always_apply = false; # Disable always_apply so we can test the create method without overloading cpu
        $ca->create();

        # Ensure we have a matching keypair
        $this->assert_is_not_empty($ca->crt->value);
        $this->assert_is_not_empty($ca->prv->value);
        $this->assert_is_true(X509Validator::is_matching_keypair($ca->crt->value, $ca->prv->value));

        # Ensure our RSA key length is correct
        $prv = openssl_pkey_get_private($ca->prv->value);
        $key_details = openssl_pkey_get_details($prv);
        $this->assert_is_true($key_details['type'] === OPENSSL_KEYTYPE_RSA);
        $this->assert_is_true($key_details['bits'] === 2048);

        # Cleanup the CA
        $ca->delete();
    }

    /**
     * Ensure we can generate a new Certificate Authority with an ECDSA key.
     */
    public function test_generate_ecdsa_certificate_authority(): void
    {
        $ca = new CertificateAuthorityGenerate(
            descr: 'test_ca',
            trust: true,
            randomserial: true,
            is_intermediate: false,
            keytype: 'ECDSA',
            ecname: 'prime256v1',
            digest_alg: 'sha256',
            lifetime: 3650,
            dn_country: 'US',
            dn_state: 'UT',
            dn_city: 'Salt Lake City',
            dn_organization: 'ACME Org',
            dn_organizationalunit: 'IT'
        );
        $ca->always_apply = false; # Disable always_apply so we can test the create method without overloading cpu
        $ca->create();

        # Ensure we have a matching keypair
        $this->assert_is_not_empty($ca->crt->value);
        $this->assert_is_not_empty($ca->prv->value);
        $this->assert_is_true(X509Validator::is_matching_keypair($ca->crt->value, $ca->prv->value));

        # Ensure our ECDSA key is correct
        $prv = openssl_pkey_get_private($ca->prv->value);
        $key_details = openssl_pkey_get_details($prv);
        $this->assert_is_true($key_details['type'] === OPENSSL_KEYTYPE_EC);
        $this->assert_is_true($key_details['ec']['curve_name'] === 'prime256v1');

        # Cleanup the CA
        $ca->delete();
    }

    /**
     * Ensure we can generate a new intermediate Certificate Authority.
     */
    public function test_generate_intermediate_certificate_authority(): void
    {
        $root_ca = new CertificateAuthorityGenerate(
            descr: 'root_ca',
            trust: true,
            randomserial: true,
            is_intermediate: false,
            keytype: 'RSA',
            keylen: 2048,
            digest_alg: 'sha256',
            lifetime: 3650,
            dn_country: 'US',
            dn_state: 'UT',
            dn_city: 'Salt Lake City',
            dn_organization: 'ACME Org',
            dn_organizationalunit: 'IT'
        );
        $root_ca->always_apply = false; # Disable always_apply so we can test the create method without overloading cpu
        $root_ca->create();

        $intermediate_ca = new CertificateAuthorityGenerate(
            descr: 'intermediate_ca',
            trust: true,
            randomserial: true,
            is_intermediate: true,
            caref: $root_ca->refid->value,
            keytype: 'RSA',
            keylen: 2048,
            digest_alg: 'sha256',
            lifetime: 3650,
            dn_country: 'US',
            dn_state: 'UT',
            dn_city: 'Salt Lake City',
            dn_organization: 'ACME Org',
            dn_organizationalunit: 'IT',
        );
        $intermediate_ca->always_apply = false; # Disable always_apply to test create method without overloading cpu
        $intermediate_ca->create();

        # Ensure we have a matching keypair
        $this->assert_is_not_empty($intermediate_ca->crt->value);
        $this->assert_is_not_empty($intermediate_ca->prv->value);
        $this->assert_is_true(X509Validator::is_matching_keypair($intermediate_ca->crt->value, $intermediate_ca->prv->value));

        # Ensure our RSA key length is correct
        $prv = openssl_pkey_get_private($intermediate_ca->prv->value);
        $key_details = openssl_pkey_get_details($prv);
        $this->assert_is_true($key_details['type'] === OPENSSL_KEYTYPE_RSA);
        $this->assert_is_true($key_details['bits'] === 2048);

        # Ensure the root CAs next serial has incremented
        $this->assert_is_greater_than(
            CertificateAuthority::query(refid: $root_ca->refid->value)->first()->serial->value,
            $root_ca->serial->value
        );

        # Ensure our intermediate CA is signed by the root CA
        $inter_ca_cert = openssl_x509_read($intermediate_ca->crt->value);
        $root_ca_cert = openssl_x509_read($root_ca->crt->value);
        $root_ca_cert = openssl_pkey_get_public($root_ca_cert);
        $this->assert_equals(openssl_x509_verify($inter_ca_cert, $root_ca_cert), 1);

        # Cleanup the CAs
        $intermediate_ca->delete();
        $root_ca->delete();
    }

    /**
     * Ensure the get_ecname_choices method returns EC names.
     */
    public function test_get_ecname_choices(): void {
        $ca = new CertificateAuthorityGenerate();
        $this->assert_is_not_empty($ca->get_ecname_choices());
    }

    /**
     * Ensure the get_digest_alg_choices method returns digest algorithms.
     */
    public function test_get_digest_alg_choices(): void
    {
        $ca = new CertificateAuthorityGenerate();
        $this->assert_is_not_empty($ca->get_digest_alg_choices());
    }

    /**
     * Ensure the get_country_choices method returns countries.
     */
    public function test_get_country_choices(): void {
        $ca = new CertificateAuthorityGenerate();
        $this->assert_is_not_empty($ca->get_country_choices());
        $this->assert_is_true(in_array('US', $ca->get_country_choices()));
        $this->assert_is_true(in_array('CA', $ca->get_country_choices()));
        $this->assert_is_true(in_array('GB', $ca->get_country_choices()));
    }
}