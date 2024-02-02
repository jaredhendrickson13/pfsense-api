<?php

namespace RESTAPI\Tests;

use RESTAPI\Auth\BasicAuth;
use RESTAPI\Auth\JWTAuth;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\RESTAPIJWT;

class APIAuthJWTAuthTestCase extends TestCase
{
    /**
     * Checks that the _authenticate_method() correctly determines whether a client successfully authenticates or not.
     */
    public function test_authenticate(): void
    {
        # Simulate success jwt auth
        $jwt = new RESTAPIJWT();
        $jwt->client->username = 'admin';
        $jwt->create();
        $_SERVER['HTTP_AUTHORIZATION'] = 'bearer ' . $jwt->token->value;
        $auth = new JWTAuth();
        $this->assert_is_true($auth->authenticate());

        # Simulate a client submitting invalid jwt auth credentials
        $_SERVER['HTTP_AUTHORIZATION'] = 'bearer badtoken';
        $auth = new JWTAuth();
        $this->assert_is_false($auth->authenticate());
    }
}
