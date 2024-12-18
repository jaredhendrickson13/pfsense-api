<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\ACMEAccountKey;
use RESTAPI\Models\ACMEAccountKeyRegister;
use RESTAPI\Models\ACMECertificate;
use RESTAPI\Models\ACMECertificateIssue;
use RESTAPI\Models\ACMECertificateRenew;

/**
 * @note To prevent redundant ACME registrations, this TestCase also contains tests for the following Models:
 * - ACMECertificate
 * - ACMECertificateAction
 * - ACMECertificateDomain
 * - ACMECertificateRenew
 * - ACMECertificateIssue
 */
class APIModelsACMECertificateTestCase extends TestCase {
    public ACMEAccountKey $acme_account_key;
    public array $required_packages = ['pfSense-pkg-acme'];

    /**
     * Setup the test case by creating a new ACME account to use.
     */
    public function setup(): void {
        # Create the ACME account key
        $this->acme_account_key = new ACMEAccountKey(
            name: 'test_key',
            descr: 'A test key',
            email: '',
            acmeserver: 'letsencrypt-staging-2',
        );
        $this->acme_account_key->create();

        # Ensure the ACME account key is registered
        $register = new ACMEAccountKeyRegister(name: $this->acme_account_key->name->value);
        $register->create();
    }

    /**
     * Cleanup the test case by deleting the ACME account key.
     */
    public function teardown(): void {
        # Delete the ACME account key
        $this->acme_account_key->delete();
    }

    /**
     * Ensures we can create, read, update and delete an ACME certificate. This test also ensure the Certificate can
     * be issued and renewed.
     * @note This test does not actually attempt to receive a certificate from the ACME server, but it does test
     * that the necessary verification steps are really taken.
     */
    public function test_crud(): void {
        # Create a new ACME certificate
        $cert = new ACMECertificate(
            name: 'test_cert',
            descr: 'A test certificate',
            acmeaccount: $this->acme_account_key->name->value,
            keylength: '2048',
            a_domainlist: [['name' => 'example.pfrest.org', 'status' => 'enable', 'method' => 'dns_manual']],
            a_actionlist: [['method' => 'shellcommand', 'command' => 'echo test', 'status' => 'active']],
        );
        $cert->create();

        # Ensure the ACME certificate was created
        $cert = ACMECertificate::query(name: 'test_cert')->first();
        $this->assert_equals($cert->name->value, 'test_cert');
        $this->assert_equals($cert->descr->value, 'A test certificate');
        $this->assert_equals($cert->acmeaccount->value, $this->acme_account_key->name->value);
        $this->assert_equals($cert->keylength->value, '2048');

        # Ensure we can issue the ACME certificate
        $issue = new ACMECertificateIssue(certificate: $cert->name->value, async: false);
        $issue->create();
        $this->assert_equals($issue->status->value, 'completed');
        $this->assert_str_contains($issue->result_log->value, 'TXT value:');
        $this->assert_str_contains(
            $issue->result_log->value,
            "Domain: '_acme-challenge.{$cert->a_domainlist->value[0]['name']}'",
        );

        # Ensure the renew process is actually initiated
        $renew = new ACMECertificateRenew(certificate: $cert->name->value, async: false);
        $renew->create();
        $this->assert_equals($renew->status->value, 'completed');
        $this->assert_str_contains($renew->result_log->value, "Renewing: '{$cert->a_domainlist->value[0]['name']}'");

        # Delete the ACME certificate
        $cert->delete();
        $this->assert_equals(ACMECertificate::read_all()->count(), 0);
    }
}
