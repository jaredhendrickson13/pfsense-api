<?php

namespace RESTAPI\Tests;

use RESTAPI\Auth\BasicAuth;
use RESTAPI\Core\Auth;
use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\RESTAPIJWT;
use RESTAPI\Models\RESTAPIKey;
use RESTAPI\Models\RESTAPISettings;
use RESTAPI\Models\User;

class APICoreAuthTestCase extends TestCase
{
    /**
     * Check that Auth objects are constructed with the remote client's IP address as the `ip_address` property.
     */
    public function test_construct_obtains_ip_address_from_client(): void {
        # Mock the client IP by assigning $_SERVER["REMOTE_ADDR"] a value directly
        $_SERVER["REMOTE_ADDR"] = "1.2.3.4";
        $auth = new Auth();
        $this->assert_equals($auth->ip_address, "1.2.3.4");
    }

    /**
     * Checks that Auth classes must have their _authenticate() method overridden or an error is thrown.
     */
    public function test_auth_class_must_override_authenticate(): void {
        $this->assert_throws_response(
            response_id: "AUTH_CLASS_WITHOUT_OVERRIDDEN_AUTHENTICATE_METHOD",
            code: 500,
            callable: function() {
                # _authentcate() is a protected method, but we can trigger it with authenticate()
                $auth = new Auth();
                $auth->authenticate();
            }
        );
    }

    /**
     * Checks that the authenticate() method correctly identifies when a client has authenticated.
     */
    public function test_authenticate(): void {
        # Since the core Auth must be overridden to authenticate, use BasicAuth to test
        $auth = new BasicAuth();

        # Mock successful auth by manually setting the basic auth headers
        $_SERVER["PHP_AUTH_USER"] = "admin";
        $_SERVER["PHP_AUTH_PW"] = "pfsense";

        # Ensure authentication is successful and values were populated
        $this->assert_is_true($auth->authenticate());
        $this->assert_is_true($auth->is_authenticated);
        $this->assert_equals($auth->username, "admin");
        $this->assert_equals($auth->username, $_SESSION["Username"]);

        # Mock failed auth by manually setting the basic auth headers
        $_SERVER["PHP_AUTH_USER"] = "admin";
        $_SERVER["PHP_AUTH_PW"] = "bad_password";

        # Mock having been previously authorized, so we can ensure failed auth removes existing authorization
        $auth->is_authorized = true;

        # Ensure authentication is not successful and values were unset
        $this->assert_is_false($auth->authenticate());
        $this->assert_is_false($auth->is_authenticated);
        $this->assert_is_false($auth->is_authorized);
        $this->assert_is_empty($_SESSION["Username"]);
    }

    /**
     * Checks that the authorize() method correctly identifies when a client is authorized.
     */
    public function test_authorize(): void {
        # Mock successful authorization
        $auth = new Auth(required_privileges: ["page-all"]);
        $auth->username = "admin";
        $this->assert_is_true($auth->authorize());
        $this->assert_is_true($auth->is_authorized);

        # Mock failed authorization due to missing privileges
        $auth = new Auth(required_privileges: ["some-privilege-we-dont-have"]);
        $auth->username = "admin";
        $this->assert_is_false($auth->authorize());
        $this->assert_is_false($auth->is_authorized);
    }

    /**
     * Checks if the is_user_enabled() method correctly identifies whether a given user exists and if they are
     * currently enabled.
     */
    public function test_is_user_enabled(): void {
        # Ensure the method correctly identifies a user that exists and is not disabled
        $this->assert_is_true(Auth::is_user_enabled("admin"));

        # Ensure the method correctly identifies a user that does not exist
        $this->assert_is_false(Auth::is_user_enabled("not_a_real_user"));

        # Ensure the method correctly identifies a user that is disabled
        $user = new User(name: "testuser", password: "testpass", disabled: true);
        $user->create();
        $this->assert_is_false(Auth::is_user_enabled($user->name->value));

        # Enable the user and ensure it is now seen
        $user->disabled->value = false;
        $user->update();
        $this->assert_is_true(Auth::is_user_enabled($user->name->value));
        $user->delete();
    }

    /**
     * Checks that authentication is logged.
     */
    public function test_auth_is_logged(): void {
        # Enable login protection to enable authentication logging
        $api_settings = new RESTAPISettings();
        $api_settings->login_protection->value = true;
        $api_settings->update();

        # Use basic auth to mock a failed authentication attempt and ensure it was logged
        $_SERVER["PHP_AUTH_USER"] = "test_user";
        $_SERVER["PHP_AUTH_PW"] = "bad_password";
        file_put_contents("/var/log/auth.log", "");
        $auth = new BasicAuth();
        $auth->ip_address = "1.2.3.4";
        $auth->authenticate();
        $auth_log = file_get_contents("/var/log/auth.log");
        $this->assert_str_contains(
            $auth_log,
            "webConfigurator authentication error for user '$auth->username' from: $auth->ip_address"
        );

        # Ensure failed authentication is not logged if `login_protecton` is disabled
        $api_settings->login_protection->value = false;
        $api_settings->update();
        $_SERVER["PHP_AUTH_USER"] = "test_user";
        $_SERVER["PHP_AUTH_PW"] = "bad_password";
        file_put_contents("/var/log/auth.log", "");
        $auth = new BasicAuth();
        $auth->ip_address = "1.2.3.4";
        $auth->authenticate();
        $auth_log = file_get_contents("/var/log/auth.log");
        $this->assert_str_does_not_contain(
            $auth_log,
            "webConfigurator authentication error for user '$auth->username' from: $auth->ip_address"
        );

        # Re-enable login protection and enable logging successful authentication and ensure successful auth is logged
        $api_settings->login_protection->value = true;
        $api_settings->log_successful_auth->value = true;
        $api_settings->update();
        $_SERVER["PHP_AUTH_USER"] = "admin";
        $_SERVER["PHP_AUTH_PW"] = "pfsense";
        file_put_contents("/var/log/auth.log", "");
        $auth = new BasicAuth();
        $auth->ip_address = "1.2.3.4";
        $auth->authenticate();
        $auth_log = file_get_contents("/var/log/auth.log");
        $this->assert_str_contains(
            $auth_log,
            "Successful login for user '$auth->username' from: $auth->ip_address (Local Database)"
        );
    }

//    This test is flaky, find a better way to test this
//    /**
//     * Checks that login protection blocks successive failed authentication attempts
//     */
//    public function test_login_protection(): void {
//        # Enable login protection to enable authentication logging
//        $api_settings = new RESTAPISettings();
//        $api_settings->login_protection->value = true;
//        $api_settings->update();
//
//        # Use basic auth to mock failed authentication attempts
//        $_SERVER["PHP_AUTH_USER"] = "test_user";
//        $_SERVER["PHP_AUTH_PW"] = "bad_password";
//        file_put_contents("/var/log/auth.log", "");
//        $auth = new BasicAuth();
//        $auth->ip_address = "4.3.2.1";
//
//        # Perform many failed authentication requests to trigger login protection to block the IP
//        foreach (range(0, 10) as $ignored) {
//            $auth->authenticate();
//        }
//
//        # Ensure the IP now shows in the sshguard table as expected
//        $sshguard = new Command("pfctl -t sshguard -Ts");
//        $this->assert_str_contains(
//            $sshguard->output,
//            $auth->ip_address
//        );
//
//        # Flush the sshguard table to remove the block
//        new Command("pfctl -t sshguard -T flush");
//    }

    /**
     * Checks that the get_auth_key() is correctly able to identify the key included in authorization headers with a
     * specific identifier.
     */
    public function test_get_auth_key(): void {
        # Simulate a client sending a bearer token
        $_SERVER["HTTP_AUTHORIZATION"] = "bearer some_auth_key_here";

        # Ensure get_auth_key() is able to obtain the bearer token correctly
        $this->assert_equals(Auth::get_auth_key(identifier: "bearer"), "some_auth_key_here");

        # Ensure the identifier is case insensitive
        $_SERVER["HTTP_AUTHORIZATION"] = "BEARER some_auth_key_here";
        $this->assert_equals(Auth::get_auth_key(identifier: "Bearer"), "some_auth_key_here");
    }

    /**
     * Checks that the ensure_class_exists() method correctly identifies Auth classes
     */
    public function test_ensure_class_exists(): void {
        $this->assert_does_not_throw(
            callable: function () {
                Auth::ensure_class_exists("BasicAuth");
                Auth::ensure_class_exists("KeyAuth");
                Auth::ensure_class_exists("JWTAuth");
            }
        );

        $this->assert_throws_response(
            response_id: "AUTH_CLASS_DOES_NOT_EXIST",
            code: 500,
            callable: function () {
                Auth::ensure_class_exists("NotAnExistingAuthClass");
            }
        );
    }

    /**
     * Checks that the attempt_multi_auth() correctly authenticates users using the specified Auth classes.
     */
    public function test_attempt_multi_auth(): void {
        # Ensure authentication fails when we expect it
        $this->assert_throws_response(
            response_id: "AUTH_AUTHENTICATION_FAILED",
            code: 401,
            callable: function () {
                # Simulate a user using basic auth with bad credentials
                $_SERVER["PHP_AUTH_USER"] = "admin";
                $_SERVER["PHP_AUTH_PW"] = "bad_password";
                Auth::attempt_multi_auth(auth_classes: ["BasicAuth", "JWTAuth"]);
            }
        );

        # Ensure authorization fails when we expect it
        $this->assert_throws_response(
            response_id: "AUTH_AUTHORIZATION_FAILED",
            code: 403,
            callable: function () {
                # Simulate a user using basic auth with good credentials but bad privileges
                $_SERVER["PHP_AUTH_USER"] = "admin";
                $_SERVER["PHP_AUTH_PW"] = "pfsense";
                Auth::attempt_multi_auth(auth_classes: ["BasicAuth", "JWTAuth"], required_privileges: ["missing-priv"]);
            }
        );

        # Ensure authenticate succeeds as long as one Auth class succeeds
        $this->assert_does_not_throw(
            callable: function () {
                # Perform authentication with good BasicAuth but bad JWTAuth and KeyAuth
                $_SERVER["PHP_AUTH_USER"] = "admin";
                $_SERVER["PHP_AUTH_PW"] = "pfsense";
                Auth::attempt_multi_auth(auth_classes: ["BasicAuth", "JWTAuth", "KeyAuth"]);

                # Perform authentication with good JWTAuth but bad BasicAuth and KeyAuth
                unset($_SERVER["PHP_AUTH_USER"]);
                unset($_SERVER["PHP_AUTH_PW"]);
                $jwt = new RESTAPIJWT();
                $jwt->client->username = "admin";
                $jwt->create();
                $_SERVER["HTTP_AUTHORIZATION"] = "bearer ".$jwt->token->value;
                Auth::attempt_multi_auth(auth_classes: ["BasicAuth", "JWTAuth", "KeyAuth"]);

                # Perform authentication with good KeyAuth but bad BasicAuth and JWTAuth
                $key = new RESTAPIKey();
                $key->client->username = "admin";
                $key->create();
                $_SERVER["HTTP_AUTHORIZATION"] = "key ".$key->key->value;
                Auth::attempt_multi_auth(auth_classes: ["BasicAuth", "JWTAuth", "KeyAuth"]);

                # Delete the API key
                $key->delete();
            }
        );
    }
}
