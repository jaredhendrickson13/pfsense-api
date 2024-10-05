<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Model;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\CertificateAuthority;
use RESTAPI\Models\CertificateAuthorityGenerate;
use RESTAPI\Models\CertificateAuthorityRenew;

class APIModelsCertificateAuthorityRenewTestCase extends TestCase {
    /**
     * @var CertificateAuthority|Model $ca The Certificate Authority to renew.
     */
    private Model|CertificateAuthority $ca;

    /**
     * Set up the test case so we have a Certificate Authority to renew.
     */
    public function setup(): void {
        # Generate a new Certificate Authority
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
            dn_organizationalunit: 'IT',
        );
        $ca->always_apply = false; # Disable always_apply so we can test the create method without overloading cpu
        $ca->create();

        # Store the generated CA as a class property
        $this->ca = CertificateAuthority::query(refid: $ca->refid->value)->first();
    }

    /**
     * Tear down the test case so we clean up the Certificate Authority we created.
     */
    public function teardown(): void {
        # Cleanup the CA
        $this->ca->always_apply = false; # Disable always_apply so we can test the create method without overloading cpu
        $this->ca->delete();
    }

    /**
     * Ensure when we renew the CertificateAuthority while reusing the key and serial, both the certificate and
     * the key remain the same.
     */
    public function test_renew_certificate_authority_reuse(): void {
        # Before we renew, obtain the existing CA cert
        $old_cert = $this->ca->crt->value;
        $old_key = $this->ca->prv->value;

        # Renew the Certificate Authority
        $renew = new CertificateAuthorityRenew(
            caref: $this->ca->refid->value,
            reusekey: true,
            reuseserial: true,
            strictsecurity: false,
        );
        $renew->always_apply = false; # Prevent clobbering CPU during tests
        $renew->create();
        $renew->reload_config(true);

        # Refresh our CA object
        $this->ca = CertificateAuthority::query(refid: $this->ca->refid->value)->first();

        # Ensure the certificate, key and serial are the same
        $this->assert_equals($old_cert, $this->ca->crt->value);
        $this->assert_equals($old_key, $this->ca->prv->value);
        $this->assert_equals($renew->oldserial->value, $renew->newserial->value);
    }

    /**
     * Ensure when we renew the CertificateAuthority without reusing the key and serial, the certificate, key, and serial
     * all change.
     */
    public function test_renew_certificate_authority_no_reuse(): void {
        # Before we renew, obtain the existing CA cert, key, and serial
        $old_cert = $this->ca->crt->value;
        $old_key = $this->ca->prv->value;

        # Renew the Certificate Authority
        $renew = new CertificateAuthorityRenew(
            caref: $this->ca->refid->value,
            reusekey: false,
            reuseserial: false,
            strictsecurity: false,
        );
        $renew->always_apply = false; # Prevent clobbering CPU during tests
        $renew->create();

        # Refresh our CA object
        $this->ca = CertificateAuthority::query(refid: $this->ca->refid->value)->first();

        # Ensure the certificate, key, and serial have all changed
        $this->assert_not_equals($old_cert, $this->ca->crt->value);
        $this->assert_not_equals($old_key, $this->ca->prv->value);
        $this->assert_not_equals($renew->oldserial->value, $renew->newserial->value);
    }

    /**
     * Ensure the reusekey and reuseserial fields both default to rue internally
     */
    public function test_reusekey_and_reuseserial_default_to_true(): void {
        $renew = new CertificateAuthorityRenew();
        $this->assert_equals($renew->get_internal()['reusekey'], true);
        $this->assert_equals($renew->get_internal()['reuseserial'], true);
    }
}
