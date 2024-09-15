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

    /**
     * Checks that the 'sync()' method correctly syncs the API config to HA peers.
     */
    public function test_sync(): void {
        # Use a non-pfSense host as an HA peer
        $api_settings = new RESTAPISettings();
        $api_settings->ha_sync->value = true;
        $api_settings->ha_sync_hosts->value = ['www.example.com'];
        $api_settings->ha_sync_username->value = 'admin';
        $api_settings->ha_sync_password->value = 'pfsense';
        $api_settings->update();

        # Read the syslog and ensure the synced failed
        # TODO: This test is flaky and needs to be reworked
//        RESTAPISettingsSync::sync();
//        $syslog = file_get_contents('/var/log/system.log');
//        $this->assert_str_contains(
//            $syslog,
//            'Failed to sync REST API settings to example.com: received unexpected response.',
//        );

        # Use a non-existent host as an HA peer and ensure the sync failed
        $api_settings->ha_sync_hosts->value = ['127.1.2.3'];
        $api_settings->update();
        RESTAPISettingsSync::sync();
        $syslog = file_get_contents('/var/log/system.log');
        $this->assert_str_contains($syslog, 'Failed to sync REST API settings to 127.1.2.3: no response received.');

        # Use bad credentials and ensure the sync failed
        $api_settings->ha_sync_hosts->value = ['127.0.0.1'];
        $api_settings->ha_sync_password->value = 'bad password';
        $api_settings->update();
        RESTAPISettingsSync::sync();
        $syslog = file_get_contents('/var/log/system.log');
        $this->assert_str_contains($syslog, 'Failed to sync REST API settings to 127.0.0.1: Authentication failed');
    }
}
