<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\LogSettings;

class APIModelsLogSettingsTestCase extends TestCase {
    /**
     * Ensure we can update and read the LogSettings model.
     */
    public function test_crud(): void {
        # Update the log settings
        $log_settings = new LogSettings(
            logfilesize: 500000,
            rotatecount: 6,
            enableremotelogging: true,
            remoteserver: '127.0.0.1:514',
            logall: true,
        );

        $log_settings->update();

        # Read the log settings and ensure the values we assigned are present
        $log_settings = new LogSettings();
        $this->assert_equals($log_settings->logfilesize->value, 500000);
        $this->assert_equals($log_settings->rotatecount->value, 6);
        $this->assert_equals($log_settings->enableremotelogging->value, true);
        $this->assert_equals($log_settings->remoteserver->value, '127.0.0.1:514');
        $this->assert_equals($log_settings->logall->value, true);

        # Disable remote logging
        $log_settings->enableremotelogging->value = false;
        $log_settings->update();
    }
}
