<?php

namespace RESTAPI\Tests;

use RESTAPI\Auth\BasicAuth;
use RESTAPI\Auth\JWTAuth;
use RESTAPI\Auth\KeyAuth;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\RESTAPIJWT;
use RESTAPI\Models\RESTAPIKey;

class APIAuthKeyAuthTestCase extends TestCase
{
    /**
     * Checks that the _authenticate_method() correctly determines whether a client successfully authenticates or not.
     */
    public function test_authenticate(): void {
        # Simulate success key auth
        $jwt = new RESTAPIKey();
        $jwt->client->username = "admin";
        $jwt->create();
        $_SERVER["HTTP_AUTHORIZATION"] = "key ". $jwt->key->value;
        $auth = new KeyAuth();
        $this->assert_is_true($auth->authenticate());

        # Simulate a client submitting invalid key auth credentials
        $_SERVER["HTTP_AUTHORIZATION"] = "key badtoken";
        $auth = new KeyAuth();
        $this->assert_is_false($auth->authenticate());
    }
}