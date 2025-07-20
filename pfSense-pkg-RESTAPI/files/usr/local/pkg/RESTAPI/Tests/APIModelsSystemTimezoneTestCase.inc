<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemTimezone;

class APIModelsSystemTimezoneTestCase extends TestCase {
    /**
     * Checks that the system timezone is configured after updating.
     */
    public function test_update(): void {
        # Create the SystemHostname model object to test with
        $timezone = new SystemTimezone(timezone: 'America/Denver');
        $timezone->update();

        # Ensure the timezone was updated correctly
        $zoneinfo = file_get_contents('/var/db/zoneinfo');
        $this->assert_equals($zoneinfo, 'America/Denver'.PHP_EOL);

        # Update the timezone again
        $timezone = new SystemTimezone(timezone: 'UTC');
        $timezone->update();

        # Ensure the timezone was updated correctly
        $zoneinfo = file_get_contents('/var/db/zoneinfo');
        $this->assert_equals($zoneinfo, 'UTC'.PHP_EOL);
    }

    /**
     * Checks that the get_timezone_choices method returns a valid list of timezones.
     */
    public function test_get_timezone_choices(): void {
        $timezone = new SystemTimezone();
        $choices = $timezone->get_timezone_choices();
        $expected_choices = ['America/Denver', 'America/New_York', 'UTC'];

        # Ensure the expected choices are in the list
        foreach ($expected_choices as $choice) {
            $this->assert_is_true(
                in_array($choice, $choices),
                message: "Expected timezone '$choice' not found in choices.",
            );
        }
    }
}
