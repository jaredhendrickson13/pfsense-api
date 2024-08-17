<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\ACMEAccountKey;
use RESTAPI\Models\ACMEAccountKeyRegister;

class APIModelsACMEAccountKeyRegisterTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-acme'];

    /**
     * Checks that we can successfully register an ACME account key.
     */
    public function test_register_success(): void {
        # Create a new ACME account key
        $account_key = new ACMEAccountKey(name: 'test', acmeserver: 'letsencrypt-staging-2');
        $account_key->create();

        # Register the ACME account key
        $register = new ACMEAccountKeyRegister(name: $account_key->name->value);
        $register->create();
    }

    /**
     * Checks that failed registration correctly populates the success field.
     */
    public function test_register_failure(): void {
        # Google's ACME servers requires an email address to register an account. Try to register one without an email
        # to trigger a failure.
        $this->assert_throws_response(
            response_id: 'ACME_SERVER_ACCOUNT_KEY_REGISTER_FAILED',
            code: 500,
            callable: function () {
                $account_key = new ACMEAccountKey(name: 'test', acmeserver: 'google-staging');
                $account_key->create();

                $register = new ACMEAccountKeyRegister(name: $account_key->name->value);
                $register->create();
            },
        );
    }
}
