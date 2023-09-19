<?php

namespace API\Tests;

use API\Core\Model;
use API\Core\TestCase;
use API\Models\APISettings;
use const API\Models\API_SETTINGS_BACKUP_FILE_PATH;
use const API\Models\API_SETTINGS_BACKUP_NOT_CONFIGURED;
use const API\Models\API_SETTINGS_RESTORE_NO_BACKUP;
use const API\Models\API_SETTINGS_RESTORE_NO_CHANGE;
use const API\Models\API_SETTINGS_RESTORE_SUCCESS;

class APIModelsAPISettingsTestCase extends TestCase
{
    # TODO: Needs tests for API HA sync feature

    /**
     * Checks that the static `get_api_config()` method returns the API package's internal configuration along with
     * its ID in the configuration.
     */
    public function test_get_api_config() {
        $test_api_settings = new APISettings();
        $test_api_config = APISettings::get_api_config();

        # Ensure the API config's ID was returned and ensure it is an integer
        $this->assert_is_true(array_key_exists("id", $test_api_config));
        $this->assert_type($test_api_config["id"], "integer");

        # Ensure the API config was returned and return all expected fields are present
        $this->assert_is_true(array_key_exists("config", $test_api_config));

        foreach ($test_api_settings->get_fields() as $field) {
            $this->assert_is_true(array_key_exists($field, $test_api_config["config"]));
        }
    }

    /**
     * Checks that the static `backup_to_file()` method correctly backs up the API configuration when configured.
     */
    public function test_backup_to_file() {
        # Create an APISettings object to test with
        $api_settings = new APISettings();

        # Remove any existing backup file if present
        unlink(API_SETTINGS_BACKUP_FILE_PATH);

        # Disable `keep_backup` and ensure changes do not create backups.
        $api_settings->keep_backup->value = false;
        $api_settings->update(apply: true);
        $this->assert_is_false(is_file(API_SETTINGS_BACKUP_FILE_PATH));
        $this->assert_equals($api_settings->backup_to_file(), API_SETTINGS_BACKUP_NOT_CONFIGURED);

        # Re-enable `keep_backup` and ensure changes do create backups, and ensure the backup is valid JSON
        $api_settings->keep_backup->value = true;
        $api_settings->update(apply: true);
        $this->assert_is_true(is_file(API_SETTINGS_BACKUP_FILE_PATH));
        $this->assert_type(
            value: json_decode(file_get_contents(API_SETTINGS_BACKUP_FILE_PATH), associative: true),
            type: "array"
        );
    }

    /**
     * Checks that the static `restore_from_backup()` correctly restores the API configuration from the backup file if
     * present. Also ensures the correct return codes are provided when there is no change between the running
     * configuration and the backup, or there is backup file found.
     */
    public function test_restore_from_backup() {
        # Create an APISettings object to test with
        $api_settings = new APISettings();

        # Ensure the correct return code is given when a restore is requested by backups are not enabled
        $api_settings->keep_backup->value = false;
        $api_settings->update();
        $this->assert_equals($api_settings->restore_from_backup(), API_SETTINGS_BACKUP_NOT_CONFIGURED);

        # Re-enable backups, but remove any existing backups
        $api_settings->keep_backup->value = true;
        $api_settings->update();
        unlink(API_SETTINGS_BACKUP_FILE_PATH);

        # Ensure the correct return code is given when we try to restore from a backup that doesn't exist
        $this->assert_equals($api_settings->restore_from_backup(), API_SETTINGS_RESTORE_NO_BACKUP);

        # Take a backup of the current API configuration and immediately try to restore it. Ensure the return code
        # indicates the restore was skipped because there was no change.
        $api_settings->backup_to_file();
        $this->assert_equals($api_settings->restore_from_backup(), API_SETTINGS_RESTORE_NO_CHANGE);

        # Capture the current API config, delete the running API config, restore it from backup and ensure it matches
        $api_config_id = $api_settings->get_api_config()["id"];
        $original_api_config = $api_settings->get_api_config()["config"];
        config_set_path("installedpackages/package/$api_config_id/conf", ["keep_backup"=>"enabled", "bad-field"=>true]);
        write_config("Unit test removed API config to test restores.");
        $this->assert_equals($api_settings->restore_from_backup(), API_SETTINGS_RESTORE_SUCCESS);
        $this->assert_equals($original_api_config, $api_settings->get_api_config()["config"]);
    }

    /**
     * Checks that validation on the `enabled` field works as intended.
     */
    public function test_enabled() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["enabled" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `read_only` field works as intended.
     */
    public function test_read_only() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["read_only" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `keep_backup` field works as intended.
     */
    public function test_keep_backup() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["keep_backup" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `login_protection` field works as intended.
     */
    public function test_login_protection() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["login_protection" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `log_successful_auth` field works as intended.
     */
    public function test_log_successful_auth() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["log_successful_auth" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `allow_pre_releases` field works as intended.
     */
    public function test_allow_pre_releases() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["allow_pre_releases" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `allowed_interfaces` field works as intended.
     */
    public function test_allowed_interfaces()
    {
        # Ensure interfaces are represented as their internal values
        $api_settings = new APISettings(representation_data: ["represent_interfaces_as" => "id"]);
        $api_settings->update();

        # Ensure value must be array
        $this->assert_throws_response(
            "FIELD_INVALID_MANY_VALUE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["allowed_interfaces" => "not an array"]
                );
                $api_settings->validate();
            }
        );

        # Ensure value must be array of strings
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["allowed_interfaces" => [0, 1, 2]]
                );
                $api_settings->validate();
            }
        );

        # Ensure values must be existing interfaces
        $this->assert_throws_response(
            "FIELD_INVALID_CHOICE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["allowed_interfaces" => ["not a real interface"]]
                );
                $api_settings->validate();
            }
        );

        # Ensure the array can be empty
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["allowed_interfaces" => []]
                );
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["allowed_interfaces" => ["wan"]]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `represent_interfaces_as` field works as intended.
     */
    public function test_represent_interfaces_as() {
        # Ensure value must be a string
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["represent_interfaces_as" => 0]
                );
                $api_settings->validate();
            }
        );

        # Ensure values must be either `description`, `internal`, or `physical`
        $this->assert_throws_response(
            "FIELD_INVALID_CHOICE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["represent_interfaces_as" => "not a valid choice"]
                );
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new APISettings(representation_data: ["represent_interfaces_as" => "id"]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["represent_interfaces_as" => "descr"]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["represent_interfaces_as" => "if"]);
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `auth_mode` field works as intended.
     */
    public function test_auth_mode() {
        # Ensure value must be a string
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["auth_mode" => 0]
                );
                $api_settings->validate();
            }
        );

        # Ensure values must be either `local`, `jwt`, or `key`
        $this->assert_throws_response(
            "FIELD_INVALID_CHOICE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["auth_mode" => "not a valid choice"]
                );
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new APISettings(representation_data: ["auth_mode" => "local"]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["auth_mode" => "jwt"]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["auth_mode" => "key"]);
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `jwt_exp` field works as intended.
     */
    public function test_jwt_exp() {
        # Ensure value must be an integer
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["jwt_exp" => "not an integer"]
                );
                $api_settings->validate();
            }
        );

        # Ensure value minimum is enforced
        $this->assert_throws_response(
            "NUMERIC_RANGE_VALIDATOR_MINIMUM_CONSTRAINT",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["jwt_exp" => 299]
                );
                $api_settings->validate();
            }
        );

        # Ensure value maximum is enforced
        $this->assert_throws_response(
            "NUMERIC_RANGE_VALIDATOR_MAXIMUM_CONSTRAINT",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["jwt_exp" => 86401]
                );
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new APISettings(representation_data: ["jwt_exp" => 300]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["jwt_exp" => 5000]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["jwt_exp" => 86400]);
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `key_hash_algo` field works as intended.
     */
    public function test_key_hash_algo() {
        # Ensure value must be a string
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["key_hash_algo" => 0]
                );
                $api_settings->validate();
            }
        );

        # Ensure values must be either `sha256`, `sha384`, or `sha512`
        $this->assert_throws_response(
            "FIELD_INVALID_CHOICE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["key_hash_algo" => "not a valid choice"]
                );
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new APISettings(representation_data: ["key_hash_algo" => "sha256"]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["key_hash_algo" => "sha384"]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["key_hash_algo" => "sha512"]);
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `key_length_bytes` field works as intended.
     */
    public function test_key_length_bytes() {
        # Ensure value must be an integer
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["key_length_bytes" => "not an integer"]
                );
                $api_settings->validate();
            }
        );

        # Ensure values must be either `16`, `24`, `32` or `64`
        $this->assert_throws_response(
            "FIELD_INVALID_CHOICE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["key_length_bytes" => 12]
                );
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new APISettings(representation_data: ["key_length_bytes" => 16]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["key_length_bytes" => 24]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["key_length_bytes" => 32]);
                $api_settings->validate();
                $api_settings = new APISettings(representation_data: ["key_length_bytes" => 64]);
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `ha_sync` field works as intended.
     */
    public function test_ha_sync() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["ha_sync" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `ha_sync_validate_certs` field works as intended.
     */
    public function test_ha_sync_validate_certs() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["ha_sync_validate_certs" => "not a bool"]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `ha_sync_hosts` works as intended.
     */
    public function test_ha_sync_hosts() {
        # Ensure value must be array
        $this->assert_throws_response(
            "FIELD_INVALID_MANY_VALUE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["ha_sync_hosts" => "not an array"]
                );
                $api_settings->validate();
            }
        );

        # Ensure value must be array of strings
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["ha_sync_hosts" => [0, 1, 2]]
                );
                $api_settings->validate();
            }
        );

        # Ensure values must be IP addresses or FQDNs
        $this->assert_throws_response(
            "IP_ADDRESS_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: ["ha_sync_hosts" => ["not an IP address or FQDN!"]]
                );
                $api_settings->validate();
            }
        );

        # Ensure no more than 10 values can be specified
        $this->assert_throws_response(
            "LENGTH_VALIDATOR_MAXIMUM_CONSTRAINT",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(
                    representation_data: [
                        "ha_sync_hosts" => [
                            "127.0.0.1",
                            "127.0.0.2",
                            "127.0.0.3",
                            "127.0.0.4",
                            "127.0.0.5",
                            "1::",
                            "0::",
                            "1111:2222:3333:4444:aaaa:bbbb:cccc:dddd",
                            "www.example.com",
                            "www2.example.com",
                            "onetoomany.example.com"
                        ]
                    ]
                );
                $api_settings->validate();
            }
        );

        # Ensure empty value is allowed
        $this->assert_does_not_throw(
            function () {
                $api_settings = new APISettings(
                    representation_data: ["ha_sync_hosts" => []]
                );
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            function () {
                $api_settings = new APISettings(
                    representation_data: [
                        "ha_sync_hosts" => [
                            "127.0.0.1",
                            "1::",
                            "example.com"
                        ]
                    ]
                );
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `ha_sync_username` works as intended.
     */
    public function test_ha_sync_username() {
        # Ensure value must be string
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(representation_data: ["ha_sync_username" => 0]);
                $api_settings->validate();
            }
        );

        # Ensure empty values are allowed
        $this->assert_does_not_throw(
            function () {
                $api_settings = new APISettings(representation_data: ["ha_sync_username" => ""]);
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            function () {
                $api_settings = new APISettings(representation_data: ["ha_sync_username" => "admin"]);
                $api_settings->validate();
            }
        );
    }

    /**
     * Checks that validation on the `ha_sync_password` works as intended.
     */
    public function test_ha_sync_password() {
        # Ensure value must be string
        $this->assert_throws_response(
            "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $api_settings = new APISettings(representation_data: ["ha_sync_password" => 0]);
                $api_settings->validate();
            }
        );

        # Ensure empty values are allowed
        $this->assert_does_not_throw(
            function () {
                $api_settings = new APISettings(representation_data: ["ha_sync_password" => ""]);
                $api_settings->validate();
            }
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            function () {
                $api_settings = new APISettings(representation_data: ["ha_sync_password" => "example password"]);
                $api_settings->validate();
            }
        );

        # Ensure this field is write only and does not appear when reading representation data
        $api_settings = new APISettings(representation_data: ["ha_sync_password" => "example password"]);
        $this->assert_is_false(array_key_exists("ha_sync_password", $api_settings->to_representation()));
    }
}
