<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallRule;
use RESTAPI\Models\RESTAPISettings;
use RESTAPI\Models\User;
use const RESTAPI\Models\API_SETTINGS_BACKUP_FILE_PATH;
use const RESTAPI\Models\API_SETTINGS_BACKUP_NOT_CONFIGURED;
use const RESTAPI\Models\API_SETTINGS_RESTORE_NO_BACKUP;
use const RESTAPI\Models\API_SETTINGS_RESTORE_NO_CHANGE;
use const RESTAPI\Models\API_SETTINGS_RESTORE_SUCCESS;

class APIModelsAPISettingsTestCase extends TestCase {
    # TODO: Needs Tests for API HA sync feature

    /**
     * Checks that the static `get_pkg_config()` method returns the API package's internal configuration and
     * the static `get_pkg_id()` method returns the API package's ID
     */
    public function test_get_api_config_and_id(): void {
        $test_api_settings = new RESTAPISettings();

        # Ensure the API config's ID was returned and ensure it is an integer
        # Note: this assumes pfSense-pkg-RESTAPI is the only add-on package installed when this test runs.
        $this->assert_equals(RESTAPISettings::get_pkg_id(), 0);

        foreach ($test_api_settings->get_fields() as $field) {
            $this->assert_is_true(array_key_exists($field, RESTAPISettings::get_pkg_config()));
        }
    }

    /**
     * Checks that the static `backup_to_file()` method correctly backs up the API configuration when configured.
     */
    public function test_backup_to_file() {
        # Create an RESTAPISettings object to test with
        $api_settings = new RESTAPISettings();

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
            type: 'array',
        );
    }

    /**
     * Checks that the static `restore_from_backup()` correctly restores the API configuration from the backup file if
     * present. Also ensures the correct return codes are provided when there is no change between the running
     * configuration and the backup, or there is backup file found.
     */
    public function test_restore_from_backup() {
        # Create an RESTAPISettings object to test with
        $api_settings = new RESTAPISettings();

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
        $api_config_id = $api_settings->get_pkg_id();
        $original_api_config = $api_settings->get_pkg_config();
        config_set_path("installedpackages/package/$api_config_id/conf", [
            'keep_backup' => 'enabled',
            'bad-field' => true,
        ]);
        write_config('Unit test removed API config to test restores.');
        $this->assert_equals($api_settings->restore_from_backup(), API_SETTINGS_RESTORE_SUCCESS);
        $this->assert_equals($original_api_config, $api_settings->get_pkg_config());
    }

    /**
     * Checks that validation on the `enabled` field works as intended.
     */
    public function test_enabled() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['enabled' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `read_only` field works as intended.
     */
    public function test_read_only() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['read_only' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `keep_backup` field works as intended.
     */
    public function test_keep_backup() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['keep_backup' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `login_protection` field works as intended.
     */
    public function test_login_protection() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['login_protection' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `log_successful_auth` field works as intended.
     */
    public function test_log_successful_auth() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['log_successful_auth' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `allow_pre_releases` field works as intended.
     */
    public function test_allow_pre_releases() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['allow_pre_releases' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `allowed_interfaces` field works as intended.
     */
    public function test_allowed_interfaces() {
        # Ensure interfaces are represented as their internal values
        $api_settings = new RESTAPISettings(data: ['represent_interfaces_as' => 'id']);
        $api_settings->update();

        # Ensure value must be array
        $this->assert_throws_response(
            'FIELD_INVALID_MANY_VALUE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['allowed_interfaces' => 'not an array']);
                $api_settings->validate();
            },
        );

        # Ensure value must be array of strings
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['allowed_interfaces' => [0, 1, 2]]);
                $api_settings->validate();
            },
        );

        # Ensure values must be existing interfaces
        $this->assert_throws_response(
            'FIELD_INVALID_CHOICE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['allowed_interfaces' => ['not a real interface']]);
                $api_settings->validate();
            },
        );

        # Ensure the array can be empty
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['allowed_interfaces' => []]);
                $api_settings->validate();
            },
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['allowed_interfaces' => ['wan']]);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `represent_interfaces_as` field works as intended.
     */
    public function test_represent_interfaces_as() {
        # Ensure value must be a string
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['represent_interfaces_as' => 0]);
                $api_settings->validate();
            },
        );

        # Ensure values must be either `description`, `internal`, or `physical`
        $this->assert_throws_response(
            'FIELD_INVALID_CHOICE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['represent_interfaces_as' => 'not a valid choice']);
                $api_settings->validate();
            },
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['represent_interfaces_as' => 'id']);
                $api_settings->validate();
                $api_settings = new RESTAPISettings(data: ['represent_interfaces_as' => 'descr']);
                $api_settings->validate();
                $api_settings = new RESTAPISettings(data: ['represent_interfaces_as' => 'if']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `auth_methods` field works as intended.
     */
    public function test_auth_methods(): void {
        # Ensure value must be a string
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(auth_methods: [0]);
                $api_settings->validate();
            },
        );

        # Ensure values must be an existing Auth class
        $this->assert_throws_response(
            'FIELD_INVALID_CHOICE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(auth_methods: ['not_a_valid_choice']);
                $api_settings->validate();
            },
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new RESTAPISettings(auth_methods: ['BasicAuth', 'KeyAuth', 'JWTAuth']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `jwt_exp` field works as intended.
     */
    public function test_jwt_exp() {
        # Ensure value must be an integer
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['jwt_exp' => 'not an integer']);
                $api_settings->validate();
            },
        );

        # Ensure value minimum is enforced
        $this->assert_throws_response(
            'NUMERIC_RANGE_VALIDATOR_MINIMUM_CONSTRAINT',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['jwt_exp' => 299]);
                $api_settings->validate();
            },
        );

        # Ensure value maximum is enforced
        $this->assert_throws_response(
            'NUMERIC_RANGE_VALIDATOR_MAXIMUM_CONSTRAINT',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['jwt_exp' => 86401]);
                $api_settings->validate();
            },
        );

        # Ensure valid values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['jwt_exp' => 300]);
                $api_settings->validate();
                $api_settings = new RESTAPISettings(data: ['jwt_exp' => 5000]);
                $api_settings->validate();
                $api_settings = new RESTAPISettings(data: ['jwt_exp' => 86400]);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `ha_sync` field works as intended.
     */
    public function test_ha_sync() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['ha_sync' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `ha_sync_validate_certs` field works as intended.
     */
    public function test_ha_sync_validate_certs() {
        # Ensure value must be boolean
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['ha_sync_validate_certs' => 'not a bool']);
                $api_settings->validate();
            },
        );
    }

    /**
     * Checks that validation on the `ha_sync_hosts` works as intended.
     */
    public function test_ha_sync_hosts() {
        # Ensure value must be array
        $this->assert_throws_response(
            'FIELD_INVALID_MANY_VALUE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['ha_sync_hosts' => 'not an array']);
                $api_settings->validate();
            },
        );

        # Ensure value must be array of strings
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['ha_sync_hosts' => [0, 1, 2]]);
                $api_settings->validate();
            },
        );

        # Ensure values must be IP addresses or FQDNs
        $this->assert_throws_response(
            'IP_ADDRESS_VALIDATOR_FAILED',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['ha_sync_hosts' => ['not an IP address or FQDN!']]);
                $api_settings->validate();
            },
        );

        # Ensure no more than 10 values can be specified
        $this->assert_throws_response(
            'LENGTH_VALIDATOR_MAXIMUM_CONSTRAINT',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(
                    data: [
                        'ha_sync_hosts' => [
                            '127.0.0.1',
                            '127.0.0.2',
                            '127.0.0.3',
                            '127.0.0.4',
                            '127.0.0.5',
                            '1::',
                            '0::',
                            '1111:2222:3333:4444:aaaa:bbbb:cccc:dddd',
                            'www.example.com',
                            'www2.example.com',
                            'onetoomany.example.com',
                        ],
                    ],
                );
                $api_settings->validate();
            },
        );

        # Ensure empty value is allowed
        $this->assert_does_not_throw(function () {
            $api_settings = new RESTAPISettings(data: ['ha_sync_hosts' => []]);
            $api_settings->validate();
        });

        # Ensure valid values are accepted
        $this->assert_does_not_throw(function () {
            $api_settings = new RESTAPISettings(
                data: [
                    'ha_sync_hosts' => ['127.0.0.1', '1::', 'example.com'],
                ],
            );
            $api_settings->validate();
        });
    }

    /**
     * Checks that validation on the `ha_sync_username` works as intended.
     */
    public function test_ha_sync_username() {
        # Ensure value must be string
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['ha_sync_username' => 0]);
                $api_settings->validate();
            },
        );

        # Ensure empty values are allowed
        $this->assert_does_not_throw(function () {
            $api_settings = new RESTAPISettings(data: ['ha_sync_username' => '']);
            $api_settings->validate();
        });

        # Ensure valid values are accepted
        $this->assert_does_not_throw(function () {
            $api_settings = new RESTAPISettings(data: ['ha_sync_username' => 'admin']);
            $api_settings->validate();
        });
    }

    /**
     * Checks that validation on the `ha_sync_password` works as intended.
     */
    public function test_ha_sync_password() {
        # Ensure value must be string
        $this->assert_throws_response(
            'FIELD_INVALID_TYPE',
            code: 400,
            callable: function () {
                $api_settings = new RESTAPISettings(data: ['ha_sync_password' => 0]);
                $api_settings->validate();
            },
        );

        # Ensure empty values are allowed
        $this->assert_does_not_throw(function () {
            $api_settings = new RESTAPISettings(data: ['ha_sync_password' => '']);
            $api_settings->validate();
        });

        # Ensure valid values are accepted
        $this->assert_does_not_throw(function () {
            $api_settings = new RESTAPISettings(data: ['ha_sync_password' => 'example password']);
            $api_settings->validate();
        });

        # Ensure this field is write only and does not appear when reading representation data
        $api_settings = new RESTAPISettings(data: ['ha_sync_password' => 'example password']);
        $this->assert_is_false(array_key_exists('ha_sync_password', $api_settings->to_representation()));
    }

    /**
     * Checks that the `hateoas` field successfully enables or disables HATEOAS links in representations
     */
    public function test_hateoas(): void {
        # Enable hateoas
        $api_settings = new RESTAPISettings();
        $api_settings->hateoas->value = true;
        $api_settings->update();

        # Ensure a Model's `to_representation()` method does include the _links field if hateoas is enabled
        $rule = new FirewallRule(id: 0);
        $this->assert_is_true(array_key_exists('_links', $rule->to_representation()));

        # Disable hateoas
        $api_settings = new RESTAPISettings();
        $api_settings->hateoas->value = false;
        $api_settings->update();

        # Ensure a Model's `to_representation()` method does not include the _links field if hateoas is disabled
        $rule = new FirewallRule(id: 0);
        $this->assert_is_false(array_key_exists('_links', $rule->to_representation()));
    }

    /**
     * Checks that the `expose_sensitive_fields` field successfully enables or disables exposing sensitive fields in
     * representations.
     */
    public function test_expose_sensitive_fields(): void {
        # Enable expose_sensitive_fields
        $api_settings = new RESTAPISettings(expose_sensitive_fields: true);
        $api_settings->update();

        # Ensure sensitive fields now show in the Model's representation
        $user = new User(id: 0);
        $this->assert_is_not_empty($user->to_representation()['password']);

        # Disable expose_sensitive_fields
        $api_settings = new RESTAPISettings(expose_sensitive_fields: false);
        $api_settings->update();

        # Ensure sensitive fields are now hidden in the Model's representation
        $user = new User(id: 0);
        $this->assert_is_empty($user->to_representation()['password']);
    }

    /**
     * Checks that the `override_sensitive_fields` field successfully overrides specified sensitive fields in the Model's
     * representation.
     */
    public function test_override_sensitive_fields(): void {
        # Ignore the sensitive flag for the User model's password field
        $api_settings = new RESTAPISettings(override_sensitive_fields: ['User:password']);
        $api_settings->update();

        # Ensure we can now read the password field in the User model's representation
        $user = new User(id: 0);
        $this->assert_is_not_empty($user->to_representation()['password']);

        # Reset the override_sensitive_fields setting
        $api_settings = new RESTAPISettings(override_sensitive_fields: []);
        $api_settings->update();

        # Ensure we can no longer read the password
        $user = new User(id: 0);
        $this->assert_is_empty($user->to_representation()['password']);
    }
}
