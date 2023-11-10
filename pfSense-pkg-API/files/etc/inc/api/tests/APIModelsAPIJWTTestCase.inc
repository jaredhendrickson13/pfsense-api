<?php

namespace API\Tests;

use API\Core\Model;
use API\Core\TestCase;
use API\Models\APIJWT;
use API\Models\APISettings;

class APIModelsAPIJWTTestCase extends TestCase
{
    /**
     * Checks that the init_server_key() method correctly generates a server key that can be used to
     * generate an actual JWT.
     */
    public function test_init_server_key() {
        # First ensure there is not already a JWT server key
        $api_pkg_id = APISettings::get_api_config()["id"];
        Model::del_config("installedpackages/package/{$api_pkg_id}/conf/server_key");
        $api_pkg_conf = APISettings::get_api_config()["config"];
        $this->assert_is_empty($api_pkg_conf["server_key"]);

        # Ensure a server key is created after running init_jwt_server_key()
        APIJWT::init_server_key();
        $api_pkg_conf = APISettings::get_api_config()["config"];
        $this->assert_is_not_empty($api_pkg_conf["server_key"]);
        $first_server_key = APISettings::get_api_config()["config"]["server_key"];

        # Ensure the Jwt server key can be rotated
        APIJWT::init_server_key(rotate: true);
        $second_server_key = APISettings::get_api_config()["config"]["server_key"];

        # Ensure the server key changed during rotation
        $this->assert_not_equals($first_server_key, $second_server_key);

        # Remove the JWT server key created by generate_jwt()
        config_set_path("installedpackages/package/{$api_pkg_id}/conf/server_key", "");
        write_config("Unit test removed JWT server key used for testing");
    }

    /**
     * Checks that we are able to generate and decode JWTs.
     */
    public function test_create_and_decode_jwt() {
        # Get our API pkg config
        $api_pkg_id = APISettings::get_api_config()["id"];
        $api_settings = new APISettings();

        # Generate a JWT
        $jwt = new APIJWT();
        $jwt->client->username = "admin";
        $jwt->create();

        # Ensure the received JWT is a non-empty string
        $this->assert_type(value: $jwt->token->value, type: "string");
        $this->assert_is_not_empty(value: $jwt->token->value);

        # Attempt to decode the JWT and ensure the JWT payload values are expected
        $decoded_jwt = APIJWT::decode($jwt->token->value);
        $this->assert_equals($decoded_jwt["iss"], config_get_path("system/hostname"));
        $this->assert_equals($decoded_jwt["aud"], config_get_path("system/hostname"));
        $this->assert_equals($decoded_jwt["exp"], $decoded_jwt["nbf"] + $api_settings->jwt_exp->value);
        $this->assert_equals($decoded_jwt["data"], "admin");

        # Remove the JWT server key created by generate_jwt()
        config_set_path("installedpackages/package/{$api_pkg_id}/conf/server_key", "");
        write_config("Unit test removed JWT server key used for testing");
    }
}
