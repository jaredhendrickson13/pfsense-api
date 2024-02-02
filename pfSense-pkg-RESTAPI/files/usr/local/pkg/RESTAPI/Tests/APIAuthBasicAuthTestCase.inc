<?php

namespace RESTAPI\Tests;

use RESTAPI\Auth\BasicAuth;
use RESTAPI\Core\TestCase;

class APIAuthBasicAuthTestCase extends TestCase
{
    /**
     * Checks that the _authenticate_method() correctly determines whether a client successfully authenticates or not.
     */
    public function test_authenticate(): void
    {
        # Simulate a client submitting valid basic auth credentials
        $_SERVER['PHP_AUTH_USER'] = 'admin';
        $_SERVER['PHP_AUTH_PW'] = 'pfsense';
        $auth = new BasicAuth();
        $this->assert_is_true($auth->authenticate());

        # Simulate a client submitting invalid basic auth credentials
        $_SERVER['PHP_AUTH_USER'] = 'admin';
        $_SERVER['PHP_AUTH_PW'] = 'bad_password';
        $auth = new BasicAuth();
        $this->assert_is_false($auth->authenticate());
    }
}
