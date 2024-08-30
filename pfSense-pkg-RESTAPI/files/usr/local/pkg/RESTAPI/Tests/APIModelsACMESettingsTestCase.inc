<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\ACMESettings;
use RESTAPI\Models\CronJob;

class APIModelsACMESettingsTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-acme'];

    /**
     * Checks that enable the ACME renewal job correctly creates the cron job necessary for triggering renewals.
     */
    public function test_update_acme_settings_enable(): void {
        # Enable the ACME renewal job and ensure we now have a cron job for the ACME renewal job
        $settings = new ACMESettings(enable: true);
        $settings->update(apply: true);
        $this->assert_is_true(CronJob::query(command__contains: '/usr/local/pkg/acme/acme_command.sh')->exists());

        # Disable the ACME renewal job and ensure the cron job is removed
        $settings->enable->value = false;
        $settings->update(apply: true);
        $this->assert_is_false(CronJob::query(command__contains: '/usr/local/pkg/acme/acme_command.sh')->exists());
    }
}
