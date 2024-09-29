<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\CertificateAuthorityGenerate;
use RESTAPI\Models\CertificateSigningRequest;
use RESTAPI\Models\CertificateSigningRequestSign;

class APIModelsCertificateSigningRequestSignTestCase extends TestCase {
    public CertificateSigningRequest $csr;
    public CertificateAuthorityGenerate $ca;

    /**
     * Create a new CSR and CA to use for testing
     */
    public function setup(): void {
        # Create a CSR
        $this->csr = new CertificateSigningRequest(
            descr: 'test_csr',
            keytype: 'RSA',
            keylen: 2048,
            digest_alg: 'sha256',
            dn_country: 'US',
            dn_state: 'CA',
            dn_city: 'San Francisco',
            dn_organization: 'ACME Corp',
            dn_organizationalunit: 'IT',
            lifetime: 3650,
            dn_commonname: 'test.example.com',
        );
        $this->csr->create(apply: true);

        # Create CA
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
            dn_organizationalunit: 'IT',
        );
        $this->ca->create(apply: true);
    }

    /**
     * Remove the CSR and CA after testing
     */
    public function teardown(): void {
        $this->csr->delete();
        $this->ca->delete();
    }

    /**
     * Ensure we can sign an existing CSR.
     */
    public function test_sign(): void {
        # Sign our CSR
        $sign = new CertificateSigningRequestSign(
            descr: 'test_csr_sign',
            caref: $this->ca->refid->value,
            csr: $this->csr->csr->value,
            digest_alg: 'sha256',
            lifetime: 3650,
        );
        $sign->create(apply: true);

        # Ensure the CSR was signed and the CSR value was removed
        $this->assert_str_contains($sign->crt->value, '-----BEGIN CERTIFICATE-----');

        # Ensure the CA's next serial has incremented
        $prev_ca_serial = $this->ca->serial->value;
        $this->ca->from_internal();
        $this->assert_is_greater_than($this->ca->serial->value, $prev_ca_serial);

        # Delete the signed certificate
        $sign->delete();
    }
}
