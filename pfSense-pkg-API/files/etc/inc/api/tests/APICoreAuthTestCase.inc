<?php

namespace API\Tests;

use API\Core\Auth;
use API\Core\TestCase;
use API\Models\APIKey;
use API\Models\APISettings;

class APICoreAuthTestCase extends TestCase
{
    /**
     * Checks that we are able to create the Auth object with requested parameters.
     */
    public function test_auth_construct() {
        # Create an Auth object to test with
        $test_auth = new Auth(
            required_privileges: ["some-required-privilege"],
            mode: "jwt",
            read_only: false
        );

        $this->assert_equals($test_auth->required_privileges, ["some-required-privilege"]);
        $this->assert_equals($test_auth->mode, "jwt");
        $this->assert_equals($test_auth->read_only, false);
    }

    /**
     * Checks the bad authentication attempts against the local database fail, and good ones succeed.
     */
    public function test_authenticate_local_database() {
        # Create an Auth object that uses local authentication
        $test_auth = new Auth(mode: "local");

        # Ensure bad passwords result in authentication failure
        $this->assert_is_false($test_auth->authenticate(secret: "bad password", username: "admin"));

        # Ensure bad usernames but good passwords result in authentication failure
        $this->assert_is_false($test_auth->authenticate(secret: "pfsense", username: "bad user"));

        # Ensure good credentials result in authentication success
        $this->assert_is_true($test_auth->authenticate(secret: "pfsense", username: "admin"));
    }

    /**
     * Checks that `key` authentication is successful with a valid key, and fails with an invalid key
     */
    public function test_authenticate_key() {
        # Create an API key to test with
        $key = (new APIKey(representation_data: ["name" => "test", "username" => "admin"]))->create();

        # Create an Auth object that uses key authentication
        $test_auth = new Auth(mode: "key");

        # Ensure bad keys result in authentication failure
        $this->assert_is_false($test_auth->authenticate(secret: "bad password"));

        # Ensure good keys result in authentication success
        $this->assert_is_true($test_auth->authenticate(secret: $key->key->value));
    }

    /**
     * Checks that JWT authentication is successful with a good JWT, and fails with a bad JWT.
     */
    public function test_authenticate_jwt() {
        # Create an Auth object that uses JWT authentication and generate a new JWT
        $test_auth = new Auth(mode: "jwt");
        $jwt = $test_auth->generate_jwt("admin");

        # Ensure bad credentials result in authentication failure
        $this->assert_is_false($test_auth->authenticate(secret: "bad jwt"));

        # Ensure good credentials result in authentication success
        $this->assert_is_true($test_auth->authenticate(secret: $jwt));

        # Remove the JWT server key created by generate_jwt()
        $api_pkg_id = APISettings::get_api_config()["id"];
        config_set_path("installedpackages/package/{$api_pkg_id}/conf/server_key", "");
        write_config("Unit test removed JWT server key used for testing");
    }

    # TODO: Need test to check is_user_disabled()
}
