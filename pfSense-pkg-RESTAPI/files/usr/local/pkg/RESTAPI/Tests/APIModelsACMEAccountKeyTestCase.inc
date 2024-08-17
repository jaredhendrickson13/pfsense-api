<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\ACMEAccountKey;

class APIModelsACMEAccountKeyTestCase extends TestCase
{
    public array $required_packages = ['pfSense-pkg-acme'];

    /**
     * Ensure we can create, read, update and delete an ACMEAccountKey object.
     */
    public function test_create_read_update_delete_acme_account_key(): void {
        # Create a new ACMEAccountKey object
        $account_key = new ACMEAccountKey(
            name: 'test_key',
            descr: 'A test key',
            email: 'test@example.com',
            acmeserver: 'letsencrypt-staging-2'
        );
        $account_key->create();

        # Ensure the ACMEAccountKey object was created
        $account_key = ACMEAccountKey::query(name: 'test_key')->first();
        $this->assert_equals($account_key->name->value, 'test_key');
        $this->assert_equals($account_key->descr->value, 'A test key');
        $this->assert_equals($account_key->email->value, 'test@example.com');
        $this->assert_equals($account_key->acmeserver->value, 'letsencrypt-staging-2');
        $this->assert_str_contains($account_key->accountkey->value, '-----BEGIN PRIVATE KEY-----');

        # Update the ACMEAccountKey object
        $new_key = str_replace('PRIVATE KEY', 'RSA PRIVATE KEY', $account_key->accountkey->value);
        $account_key->from_representation(
            name: 'updated_test_key',
            descr: 'An updated test key',
            email: 'updated@example.com',
            acmeserver: 'google-production',
            accountkey: $new_key
        );
        $account_key->update();

        # Ensure the ACMEAccountKey object was updated
        $account_key = ACMEAccountKey::query(name: 'updated_test_key')->first();
        $this->assert_equals($account_key->name->value, 'updated_test_key');
        $this->assert_equals($account_key->descr->value, 'An updated test key');
        $this->assert_equals($account_key->email->value, 'updated@example.com');
        $this->assert_equals($account_key->acmeserver->value, 'google-production');
        $this->assert_str_contains($account_key->accountkey->value, '-----BEGIN RSA PRIVATE KEY-----');

        # Delete the ACMEAccountKey object
        $account_key->delete();
        $this->assert_equals(ACMEAccountKey::read_all()->count(), 0);
    }

    /**
     * Ensure the 'email' field can be empty
     */
    public function test_email_field_can_be_empty(): void
    {
        $this->assert_does_not_throw(
            callable: function () {
                # Create a new ACMEAccountKey object with an empty email field
                $account_key = new ACMEAccountKey(
                    name: 'test_key',
                    descr: 'A test key',
                    email: '',
                    acmeserver: 'letsencrypt-staging-2'
                );
                $account_key->validate();
            }
        );
    }

    /**
     * Ensure the 'get_acmeserver_choices' method returns choices correctly
     */
    public function test_get_acmeserver_choices(): void {
        $account_key = new ACMEAccountKey();
        $this->assert_is_not_empty($account_key->get_acmeserver_choices());
        $this->assert_is_true(in_array('letsencrypt-staging-2', $account_key->get_acmeserver_choices()));
    }

    /**
     * Ensure the generate_rsa_key method generates a valid RSA key.
     */
    public function test_generate_rsa_key(): void
    {
        $rsa_key = ACMEAccountKey::generate_rsa_key();
        $this->assert_str_contains($rsa_key, '-----BEGIN PRIVATE KEY-----');
        $this->assert_str_contains($rsa_key, '-----END PRIVATE KEY-----');

        # Ensure key strengths work
        $rsa_key_2048 = ACMEAccountKey::generate_rsa_key(key_bits: 2048);
        $rsa_key_4096 = ACMEAccountKey::generate_rsa_key(key_bits: 4096);
        $this->assert_is_greater_than(strlen($rsa_key_4096), strlen($rsa_key_2048));
    }

    /**
     * Ensure the get_acme_server_url() method works correctly
     */
    public function test_get_acme_server_url(): void {
        $account_key = new ACMEAccountKey(acmeserver: 'letsencrypt-staging-2');
        $this->assert_equals($account_key->get_acme_server_url(), 'https://acme-staging-v02.api.letsencrypt.org/directory');

    }
}