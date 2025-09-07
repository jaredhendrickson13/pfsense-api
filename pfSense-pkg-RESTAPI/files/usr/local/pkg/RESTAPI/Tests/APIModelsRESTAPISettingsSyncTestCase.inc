<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\RESTAPISettings;
use RESTAPI\Models\RESTAPISettingsSync;

class APIModelsRESTAPISettingsSyncTestCase extends TestCase {
    /**
     * Checks that the 'serialize_settings()` method correctly populates the 'sync_data' field.
     */
    public function test_validate_sync_data(): void {
        # Ensure a validation error is thrown if 'sync_data' cannot be unserialized
        $this->assert_throws_response(
            response_id: 'RESTAPI_SETTINGS_SYNC_SYNC_DATA_COULD_NOT_BE_UNSERIALIZED',
            code: 400,
            callable: function () {
                # Create a sync object to test with
                $settings_sync = new RESTAPISettingsSync();
                $settings_sync->sync_data->value = 'not a serialized value';
                $settings_sync->validate();
            },
        );

        # Ensure serialized values are accepted
        $this->assert_does_not_throw(
            callable: function () {
                # Create a sync object to test with
                $settings_sync = new RESTAPISettingsSync();
                $settings_sync->sync_data->value = serialize(RESTAPISettings::get_pkg_config());
                $settings_sync->validate();
            },
        );
    }

    /**
     * Checks that the 'create()' method correctly syncs the API configuration with the serialized sync data
     */
    public function test_create(): void {
        # Get the current API configuration
        $original_api_config = RESTAPISettings::get_pkg_config();
        $sync_api_config = $original_api_config;

        # Make some modifications to the API and serialize it
        $sync_api_config['enable'] = 'disabled';
        $sync_api_config['read_only'] = 'enabled';
        $sync_api_config['allowed_interfaces'] = 'lan';

        # Perform the sync against ourself and ensure it updates the API config
        $sync = new RESTAPISettingsSync(data: ['sync_data' => serialize($sync_api_config)]);
        $sync->create();

        # Read the API config now and ensure it matches the config that was synced
        $new_api_config = RESTAPISettings::get_pkg_config();
        $this->assert_not_equals($original_api_config, $new_api_config);
        $this->assert_equals($sync_api_config, $new_api_config);

        # Revert the API configuration to its original values via sync
        $sync->sync_data->value = serialize($original_api_config);
        $sync->create();

        # Read the API config now and ensure it matches the original config
        $new_api_config = RESTAPISettings::get_pkg_config();
        $this->assert_equals($original_api_config, $new_api_config);
        $this->assert_not_equals($sync_api_config, $new_api_config);
    }
}
