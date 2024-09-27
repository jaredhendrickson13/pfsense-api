<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\Certificate;
use RESTAPI\Models\CertificateSigningRequest;

class APIModelsCertificateSigningRequestTestCase extends TestCase {
    /**
     * Checks that we can generate a CSR and that we can complete the CSR process by updating the Certificate object's
     * 'crt' field with the signed certificate.
     */
    public function test_generate_and_complete_csr(): void {
        # Generate a new CSR
        $csr = new CertificateSigningRequest(
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
        $csr->create(apply: true);

        # Ensure the CSR was created
        $this->assert_str_contains($csr->csr->value, '-----BEGIN CERTIFICATE REQUEST-----');
        file_put_contents('/tmp/testca.csr', $csr->csr->value);

        # Generate a CA and sign the CSR
        new Command('openssl genpkey -algorithm RSA -out /tmp/testca.key');
        new Command('openssl req -new -x509 -key /tmp/testca.key -out /tmp/testca.crt -days 3650 -subj "/CN=Test CA"');
        new Command(
            'openssl x509 -req -in /tmp/testca.csr -CA /tmp/testca.crt -CAkey /tmp/testca.key -out /tmp/testca.crt -days 3650',
        );

        # Ensure we can complete the CSR by updating the Certificate object's 'crt' field with the signed certificate
        $cert = new Certificate(id: $csr->id);
        $cert->crt->value = file_get_contents('/tmp/testca.crt');
        $cert->update(apply: true);

        # Ensure the pending CSR was removed
        $this->assert_is_empty($cert->csr->value);

        # Remove the test files
        new Command('rm /tmp/testca.*');
    }
}
