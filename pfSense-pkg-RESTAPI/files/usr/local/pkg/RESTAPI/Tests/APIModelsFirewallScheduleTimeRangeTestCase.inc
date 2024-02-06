<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallScheduleTimeRange;

class APIModelsFirewallScheduleTimeRangeTestCase extends TestCase {
    /**
     * Checks that the `month` and `day` arrays must have the same number of array items.
     */
    public function test_month_and_day_must_have_same_number_of_items(): void {
        # Ensure an error is thrown if `month` and `day` have a mismatched number of items
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_MISMATCHED_MONTH_AND_DAY_VALUES',
            code: 400,
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->month->value = [1, 2, 3];
                $sched_time->day->value = [1, 2];
                $sched_time->validate_extra();
            },
        );

        # Ensure no error is thrown if `month` and `day` have the number of items
        $this->assert_does_not_throw(
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->month->value = [1, 2, 3];
                $sched_time->day->value = [1, 2, 3];
                $sched_time->validate_extra();
            },
        );
    }

    /**
     * Checks that the `day` value must a value within the number of days in the corresponding `month`
     */
    public function test_day_must_be_in_month(): void {
        # Ensure an error is thrown if `day` is greater than the number of days in the corresponding `month`
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_DAY_VALUE_IS_NOT_IN_MONTH',
            code: 400,
            callable: function () {
                # Try to set the date to February 31, which is not a valid day for February
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->month->value = [2];
                $sched_time->day->value = [31];
                $sched_time->validate_extra();
            },
        );

        # Ensure no error is thrown if `day` is within the corresponding `month`
        $this->assert_does_not_throw(
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->month->value = [2];
                $sched_time->day->value = [17];
                $sched_time->validate_extra();
            },
        );
    }

    /**
     * Checks that the `hour` field must be a range of valid 24-hour time strings.
     */
    public function test_hour_is_valid_24_hour_time_strings(): void {
        # Ensure an error is thrown if the starting value in the `hour` range is not a valid 24-hour time string
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_HOUR_MUST_BE_24_HOUR_TIME_RANGE',
            code: 400,
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: 'INVALID-23:59');
            },
        );

        # Ensure an error is thrown if the ending value in the `hour` range is not a valid 24-hour time string
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_HOUR_MUST_BE_24_HOUR_TIME_RANGE',
            code: 400,
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: '0:00-INVALID');
            },
        );

        # Ensure an error is thrown if `hour` is not a range at all`
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_HOUR_MUST_BE_24_HOUR_TIME_RANGE',
            code: 400,
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: 'INVALID');
            },
        );

        # Ensure no error is thrown if both values in the range are valid 24-hour time strings
        $this->assert_does_not_throw(
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: '0:00-23:59');
            },
        );
    }

    /**
     * Checks that the `hour` field must use time strings with supported minute values
     */
    public function test_hour_uses_supported_minutes(): void {
        # Ensure an error is thrown if the starting value in the `hour` range does not use a valid minutes option
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_HOUR_WITH_INVALID_MINUTES',
            code: 400,
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: '0:13-23:59');
            },
        );

        # Ensure an error is thrown if the end value in the `hour` range does not use a valid minutes option
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_HOUR_WITH_INVALID_MINUTES',
            code: 400,
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: '0:00-23:33');
            },
        );

        # Ensure no error is thrown if both values in the range use valid minute options
        $this->assert_does_not_throw(
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: '0:00-23:59');
                $sched_time->validate_hour(hour: '0:15-23:45');
                $sched_time->validate_hour(hour: '0:30-23:30');
                $sched_time->validate_hour(hour: '0:45-23:15');
                $sched_time->validate_hour(hour: '0:59-23:00');
            },
        );
    }

    /**
     * Checks that the `hour` range's starting value cannot be a time greater than it's ending time
     */
    public function test_hour_start_time_cannot_exceed_end_time(): void {
        # Ensure an error is thrown if the starting value in the `hour` is greater than the ending value
        $this->assert_throws_response(
            response_id: 'FIREWALL_SCHEDULE_TIME_RANGE_HOUR_START_TIME_GREATER_THAN_END_TIME',
            code: 400,
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: '23:59-23:45');
            },
        );

        # Ensure no error is thrown if the starting value in the `hour` is less than the ending value
        $this->assert_does_not_throw(
            callable: function () {
                $sched_time = new FirewallScheduleTimeRange();
                $sched_time->validate_hour(hour: '23:30-23:45');
            },
        );
    }

    /**
     * Checks that the `is_day_in_month()` method correctly determines if a given day value is within a given month.
     */
    public function test_is_day_in_month(): void {
        # Ensure false is returned if we try to specify a day that is one over the total number of days in the month
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 1, day: 32));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 2, day: 30));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 3, day: 32));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 4, day: 31));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 5, day: 32));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 6, day: 31));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 7, day: 32));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 8, day: 32));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 9, day: 31));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 10, day: 32));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 11, day: 31));
        $this->assert_is_false(FirewallScheduleTimeRange::is_day_in_month(month: 12, day: 32));

        # Ensure true is returned if the day value is within the total number of days in the month
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 1, day: 31));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 2, day: 29));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 3, day: 31));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 4, day: 30));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 5, day: 31));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 6, day: 30));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 7, day: 31));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 8, day: 31));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 9, day: 30));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 10, day: 31));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 11, day: 30));
        $this->assert_is_true(FirewallScheduleTimeRange::is_day_in_month(month: 12, day: 31));
    }

    /**
     * Checks that the `is_valid_24_hour_time()` correctly determines if a time string is a 24-hour time
     */
    public function test_is_valid_24_hour_time(): void {
        # Ensure valid 24-hour time strings are accepted
        $this->assert_is_true(FirewallScheduleTimeRange::is_valid_24_hour_time('0:00'));
        $this->assert_is_true(FirewallScheduleTimeRange::is_valid_24_hour_time('23:30'));
        $this->assert_is_true(FirewallScheduleTimeRange::is_valid_24_hour_time('9:15'));

        # Ensure invalid 24-hour time strings are not accepted
        $this->assert_is_false(FirewallScheduleTimeRange::is_valid_24_hour_time('24:30'));
        $this->assert_is_false(FirewallScheduleTimeRange::is_valid_24_hour_time('9:60'));
    }

    /**
     * Checks that the `is_valid_24_hour_time_range()` correctly determines if a 24-hour time string range is valid
     */
    public function test_is_valid_24_hour_time_range(): void {
        $this->assert_is_true(FirewallScheduleTimeRange::is_valid_24_hour_time_range('0:15', '2:00'));
        $this->assert_is_false(FirewallScheduleTimeRange::is_valid_24_hour_time_range('2:00', '0:15'));
    }
}
