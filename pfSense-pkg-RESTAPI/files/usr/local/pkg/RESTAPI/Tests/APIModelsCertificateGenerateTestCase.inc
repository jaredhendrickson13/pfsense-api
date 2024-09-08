<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Model;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\CertificateAuthorityGenerate;
use RESTAPI\Models\CertificateGenerate;
use RESTAPI\Validators\X509Validator;

class APIModelsCertificateGenerateTestCase extends TestCase
{
    /**
     * @var CertificateAuthority|Model $ca The CertificateAuthority model to use for testing.
     */
    private CertificateAuthority|Model $ca;

    /**
     * Setup a parent CA to use when signing certificates.
     */
    public function setup(): void {
        $this->ca = new CertificateAuthorityGenerate(
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
        $this->ca->create();
        $this->ca = CertificateAuthority::query(refid: $this->ca->refid->value)->first();
    }

    /**
     * Cleanup the parent CA.
     */
    public function teardown(): void {
        $this->ca->delete();
    }

    /**
     * Ensure we can generate a new Certificate with an RSA key.
     */
    public function test_generate_rsa_certificate(): void {
        $cert = new CertificateGenerate(
            descr: 'testcert',
            caref: $this->ca->refid->value,
            keytype: 'RSA',
            keylen: 2048,
            digest_alg: 'sha256',
            lifetime: 3650,
            type: 'user',
            dn_country: 'US',
            dn_state: 'UT',
            dn_city: 'Salt Lake City',
            dn_organization: 'ACME Org',
            dn_organizationalunit: 'IT',
            dn_commonname: 'testcert.example.com',
            dn_dns_sans: ['testcert2.example.com', 'testcert3.example.com'],
            dn_email_sans: ['example@example.com'],
            dn_ip_sans: ['127.0.0.1'],
            dn_uri_sans: ['http://example.com'],
        );
        $cert->create();

        # Ensure we have a matching keypair
        $this->assert_is_not_empty($cert->crt->value);
        $this->assert_is_not_empty($cert->prv->value);
        $this->assert_is_true(X509Validator::is_matching_keypair($cert->crt->value, $cert->prv->value));

        # Ensure our RSA key length is correct
        $prv = openssl_pkey_get_private($cert->prv->value);
        $key_details = openssl_pkey_get_details($prv);
        $this->assert_is_true($key_details['type'] === OPENSSL_KEYTYPE_RSA);
        $this->assert_is_true($key_details['bits'] === 2048);

        # Ensure the DN fields are present in the certificate
        $cert_data = openssl_x509_parse($cert->crt->value);
        $this->assert_equals($cert_data['subject']['C'], 'US');
        $this->assert_equals($cert_data['subject']['ST'], 'UT');
        $this->assert_equals($cert_data['subject']['L'], 'Salt Lake City');
        $this->assert_equals($cert_data['subject']['O'], 'ACME Org');
        $this->assert_equals($cert_data['subject']['OU'], 'IT');
        $this->assert_equals($cert_data['subject']['CN'], 'testcert.example.com');
        $this->assert_str_contains($cert_data['extensions']['subjectAltName'], 'DNS:testcert2.example.com');
        $this->assert_str_contains($cert_data['extensions']['subjectAltName'], 'DNS:testcert3.example.com');
        $this->assert_str_contains($cert_data['extensions']['subjectAltName'], 'email:example@example.com');
        $this->assert_str_contains($cert_data['extensions']['subjectAltName'], 'IP Address:127.0.0.1');
        $this->assert_str_contains($cert_data['extensions']['subjectAltName'], 'URI:http://example.com');

        # Cleanup the CA
        $cert->delete();
    }

    /**
     * Ensure the get_ecname_choices method returns EC names.
     */
    public function test_get_ecname_choices(): void {
        $cert = new CertificateGenerate();
        $this->assert_is_not_empty($cert->get_ecname_choices());
    }

    /**
     * Ensure the get_digest_alg_choices method returns digest algorithms.
     */
    public function test_get_digest_alg_choices(): void
    {
        $cert = new CertificateGenerate();
        $this->assert_is_not_empty($cert->get_digest_alg_choices());
    }

    /**
     * Ensure the get_country_choices method returns countries.
     */
    public function test_get_country_choices(): void {
        $cert = new CertificateGenerate();
        $this->assert_is_not_empty($cert->get_country_choices());
        $this->assert_is_true(in_array('US', $cert->get_country_choices()));
        $this->assert_is_true(in_array('CA', $cert->get_country_choices()));
        $this->assert_is_true(in_array('GB', $cert->get_country_choices()));
    }
}