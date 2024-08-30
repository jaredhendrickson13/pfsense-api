<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\ACMEAccountKey;
use RESTAPI\Models\ACMEAccountKeyRegister;

class APIModelsACMEAccountKeyRegisterTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-acme'];

    /**
     * Sets up the test case.
     */
    public function setup(): void {
        # This test requires system time to be accurate so ntp must be synced
        new Command('pkill ntpd');
        new Command('ntpdate pool.ntp.org');
    }

    /**
     * Checks that we can successfully register an ACME account key.
     */
    public function test_register_success(): void {
        # Create a new ACME account key
        $account_key = new ACMEAccountKey(name: 'test', acmeserver: 'letsencrypt-staging-2');
        $account_key->create();

        # Register the ACME account key
        $register = new ACMEAccountKeyRegister(name: $account_key->name->value, async: false);
        $register->create();
        $this->assert_equals($register->status->value, 'registered');
    }

    /**
     * Checks that failed registration correctly populates the success field.
     */
    public function test_register_failure(): void {
        # Google's ACME servers requires an email address to register an account. Try to register one without an email
        # to trigger a failure.
        $account_key = new ACMEAccountKey(name: 'test', acmeserver: 'google-staging');
        $account_key->create();

        $register = new ACMEAccountKeyRegister(name: $account_key->name->value, async: false);
        $register->create();
        $this->assert_equals($register->status->value, 'failed');
    }
}
