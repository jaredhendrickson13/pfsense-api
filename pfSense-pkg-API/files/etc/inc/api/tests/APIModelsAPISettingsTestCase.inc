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
}
