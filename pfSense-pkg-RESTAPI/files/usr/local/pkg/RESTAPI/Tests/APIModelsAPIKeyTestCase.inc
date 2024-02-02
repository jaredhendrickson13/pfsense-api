<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Models\RESTAPIKey;

class APIModelsAPIKeyTestCase extends TestCase
{
    /**
     * Checks that API keys can only be issued to existing users.
     */
    public function test_api_keys_can_only_be_issued_to_existing_users()
    {
        $this->assert_throws_response(
            response_id: 'API_KEY_USERNAME_MUST_EXIST',
            code: 400,
            callable: function () {
                $key = new RESTAPIKey();
                $key->client->username = 'not_an_existing_user';
                $key->validate_username('not_an_existing_user');
            },
        );

        $this->assert_does_not_throw(
            callable: function () {
                $key = new RESTAPIKey();
                $key->client->username = 'admin';
                $key->validate_username('admin');
            },
        );
    }

    /**
     * Checks that the `key` and `hash` values are populated after API key creation.
     */
    public function test_key_and_hash_populated_after_creation()
    {
        # Construct an RESTAPIKey model object to test with
        $key = new RESTAPIKey();

        # Assign the client an actual username since it's required
        $key->client->username = 'admin';

        # Ensure there is no `key` or `hash` value set before creation.
        $this->assert_is_empty($key->key->value);
        $this->assert_is_empty($key->hash->value);

        # Create the RESTAPIKey and ensure it now has `key` and `hash` values
        $key->create();
        $this->assert_is_not_empty($key->key->value);
        $this->assert_is_not_empty($key->hash->value);

        # Ensure `hash` is a hashed represention of the `key` value
        $this->assert_equals($key->hash->value, hash($key->hash_algo->value, $key->key->value));

        # Ensure the `key` value is not stored internally
        $this->assert_is_empty($key->to_internal()['key']);

        # Ensure the `hash` value is not included in representation
        $this->assert_is_empty($key->to_representation()['hash']);
        $key->delete();
    }

    /**
     * Checks that the `authenticate()` method correctly authenticates API keys.
     */
    public function test_authenticate()
    {
        # Construct an RESTAPIKey model object to test with
        $key = new RESTAPIKey();
        $key->client->username = 'admin';
        $key->create();

        # Ensure we can authenticate using the actual `key`
        $this->assert_is_true($key->authenticate($key->key->value));

        # Ensure authentication fails when the `key` is incorrect
        $this->assert_is_false($key->authenticate('not our RESTAPI key!'));
        $key->delete();
    }

    /**
     * Checks that RESTAPIKey model objects cannot be updated.
     */
    public function test_api_keys_cannot_be_updated()
    {
        $this->assert_throws_response(
            response_id: 'API_KEY_CANNOT_BE_UPDATED',
            code: 500,
            callable: function () {
                $key = new RESTAPIKey();
                $key->client->username = 'admin';
                $key->create();
                $key->update();
            },
        );
    }
}
