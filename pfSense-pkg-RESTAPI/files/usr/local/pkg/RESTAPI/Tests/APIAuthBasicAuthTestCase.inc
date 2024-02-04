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

    /**
     * Checks that we are able to accurately determine if a user is requesting BasicAuth using the is_requested method
     */
    public function test_is_requested(): void {
        # Ensure is_requested() returns false if neither a basic username or password are specified
        $_SERVER['PHP_AUTH_USER'] = null;
        $_SERVER['PHP_AUTH_PW'] = null;
        $auth = new BasicAuth();
        $this->assert_is_false($auth->is_requested());

        # Ensure is_requested() returns true if either a basic username or password are specified
        $_SERVER['PHP_AUTH_USER'] = "admin";
        $_SERVER['PHP_AUTH_PW'] = null;
        $auth = new BasicAuth();
        $this->assert_is_true($auth->is_requested());
        $_SERVER['PHP_AUTH_USER'] = null;
        $_SERVER['PHP_AUTH_PW'] = "pfsense";
        $this->assert_is_true($auth->is_requested());
        $_SERVER['PHP_AUTH_USER'] = "admin";
        $_SERVER['PHP_AUTH_PW'] = "pfsense";
        $this->assert_is_true($auth->is_requested());
    }
}
