<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallSchedule;

class APIModelsFirewallScheduleTestCase extends TestCase
{
    /**
     * Checks that the firewall schedules reports active during one of the time ranges, and inactive when not.
     */
    public function test_schedule_shows_active(): void
    {
        # Get integers of the current month and day. Ensure there are no conflicts between tests dates and todays date
        $current_month = (int) date('m');
        $current_day = (int) date('d');
        $test_day = $current_day !== 15 ? 15 : 14;

        # Create a FirewallSchedule to test with
        $sched = new FirewallSchedule(
            data: [
                'name' => 'test_schedule',
                'timerange' => [
                    [
                        'month' => [$current_month],
                        'day' => [$current_day],
                        'hour' => '0:00-23:59',
                    ],
                ],
            ],
        );
        $sched->create();

        # Ensure the 'active' field shows `true` since the schedule should be active all day
        $this->assert_is_true($sched->active->value);

        # Change the schedule so it is no longer active and ensure the active value changes to false.
        $sched->timerange->value[0]['day'] = [$test_day];
        $sched->update();
        $this->assert_is_false($sched->active->value);

        # Delete the schedule
        $sched->delete();
    }
}
