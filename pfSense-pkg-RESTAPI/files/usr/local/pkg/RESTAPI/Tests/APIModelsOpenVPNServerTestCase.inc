<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\CertificateAuthority;

class APIModelsOpenVPNServerTestCase extends TestCase
{
    private string $ca_refid;
    private string $tls_key;

    /**
     * Setup the test environment before starting tests.
     */
    public function setup(): void
    {
        # Create a CA we can test with
        $ca = new CertificateAuthority(
            descr: 'test',
            crt: file_get_contents("/usr/local/pkg/RESTAPI/Tests/assets/test_x509_certificate.crt"),
            prv: file_get_contents("/usr/local/pkg/RESTAPI/Tests/assets/test_x509_rsa.key")
        );
        $ca->create();
        $this->ca_refid = $ca->refid->value;

        # Obtain an OpenVPN key to test with
        $this->tls_key = file_get_contents("/usr/local/pkg/RESTAPI/Tests/assets/test_openvpn.key");
    }

    /**
     * Remove the CA used for testing after tests complete.
     */
    public function teardown(): void
    {
        $ca = CertificateAuthority::query(refid: $this->ca_refid)->first();
        $ca->delete();
    }
}