<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\CronJob;
use RESTAPI\Models\Package;
use RESTAPI\Models\ServiceWatchdog;

class APIModelsServiceWatchdogTestCase extends TestCase
{
    /**
     * Ensure the required package is installed before running tests.
     */
    public function setup(): void
    {
        # Install the service watchdog package
        $pkg = new Package(name: 'pfSense-pkg-Service_Watchdog');
        $pkg->create();
    }

    /**
     * Ensure the package is uninstalled after completing tests.
     */
    public function teardown(): void
    {
        # Uninstall the package
        $pkg = Package::query(name: 'pfSense-pkg-Service_Watchdog')->first();
        $pkg->delete();
    }

    /**
     * Checks that the `validate_description()` method forces the `description` field to match the description of the
     * service referenced in the `name` field.
     */
    public function test_description_must_match_service_description(): void
    {
        # Create a new Service Watchdog to test with and ensure the description was overwritten after validation.
        $watchdog = new ServiceWatchdog(name: 'unbound', description: 'will be overwritten!');
        $watchdog->validate();
        $this->assert_equals($watchdog->description->value, $watchdog->name->get_related_model()->description->value);
    }

    /**
     * Checks that the Service Watchdog cron job is present after creating a new service watchdog and is remove after
     * the last is removed.
     */
    public function test_crud(): void
    {
        # Create a new Service Watchdog to test with and ensure the Service Watchdog cron job is present afterward
        $watchdog = new ServiceWatchdog(name: 'unbound');
        $watchdog->create();
        $this->assert_is_true(CronJob::query(command: '/usr/local/pkg/servicewatchdog_cron.php')->exists());

        # Ensure we can update the watchdog item
        $watchdog->from_representation(name: 'syslogd');
        $watchdog->update();

        # Delete the watchdog and ensure the service watchdog cron job is no longer present
        $watchdog->delete();
        $this->assert_is_false(CronJob::query(command: '/usr/local/pkg/servicewatchdog_cron.php')->exists());
    }
}
