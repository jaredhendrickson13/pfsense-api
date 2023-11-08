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

    /**
     * Checks that the init_jwt_server_key() method correctly generates a server key that can be used to
     * generate an actual JWT.
     */
    public function test_init_jwt_server_key() {
        # First ensure there is not already a JWT server key
        $api_pkg_id = APISettings::get_api_config()["id"];
        $api_pkg_conf = APISettings::get_api_config()["config"];
        $this->assert_is_empty($api_pkg_conf["server_key"]);

        # Ensure a server key is created after running init_jwt_server_key()
        Auth::init_jwt_server_key();
        $api_pkg_conf = APISettings::get_api_config()["config"];
        $this->assert_is_not_empty($api_pkg_conf["server_key"]);
        $first_server_key = APISettings::get_api_config()["config"]["server_key"];

        # Ensure the Jwt server key can be rotated
        Auth::init_jwt_server_key(rotate: true);
        $second_server_key = APISettings::get_api_config()["config"]["server_key"];

        # Ensure the server key changed during rotation
        $this->assert_not_equals($first_server_key, $second_server_key);

        # Remove the JWT server key created by generate_jwt()
        config_set_path("installedpackages/package/{$api_pkg_id}/conf/server_key", "");
        write_config("Unit test removed JWT server key used for testing");
    }

    /**
     * Checks that we are able to generate and decode JWTs
     */
    public function test_generate_and_decode_jwt() {
        # Get our API pkg config
        $api_pkg_id = APISettings::get_api_config()["id"];
        $api_pkg_conf = APISettings::get_api_config()["config"];

        # Generate a JWT
        $jwt = Auth::generate_jwt(username: "admin");

        # Ensure the received JWT is a non-empty string
        $this->assert_type(value: $jwt, type: "string");
        $this->assert_is_not_empty(value: $jwt);

        # Attempt to decode the JWT and ensure the JWT payload values are expected
        $decoded_jwt = Auth::decode_jwt($jwt);
        $this->assert_equals($decoded_jwt["iss"], config_get_path("system/hostname"));
        $this->assert_equals($decoded_jwt["aud"], config_get_path("system/hostname"));
        $this->assert_equals($decoded_jwt["exp"], $decoded_jwt["nbf"] + (int)$api_pkg_conf["jwt_exp"]);
        $this->assert_equals($decoded_jwt["data"], "admin");

        # Remove the JWT server key created by generate_jwt()
        config_set_path("installedpackages/package/{$api_pkg_id}/conf/server_key", "");
        write_config("Unit test removed JWT server key used for testing");
    }

    # TODO: Need test to check is_user_disabled()
}
