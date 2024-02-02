<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\CronJob;

class APIModelsCronJobTestCase extends TestCase
{
    /**
     * Checks that the CronJob model requires the pfSense-pkg-Cron package unless $require_pkg is set to `false`
     * during construction. This is a parameter unique to the CronJob model since technically the Model can be
     * used without the package.
     */
    public function test_requires_pkg(): void
    {
        # Ensure pfSense-pkg-Cron is listed as a required package if $require_pkg was enabled during construction
        $cron = new CronJob(require_pkg: true);
        $this->assert_equals($cron->packages, ['pfSense-pkg-Cron']);

        # Ensure no packages are listed as a required package if $require_pkg was disabled during construction
        $cron = new CronJob(require_pkg: false);
        $this->assert_equals($cron->packages, []);
    }

    /**
     * Checks that an error is thrown if the `who` field is not the username of an existing OS user.
     */
    public function test_validate_who(): void
    {
        # Ensure an error is thrown if we try to create a CronJob with a non-existent user
        $this->assert_throws_response(
            response_id: 'CRON_JOB_WHO_USER_DOES_NOT_EXIST',
            code: 404,
            callable: function () {
                $cron = new CronJob(require_pkg: false);
                $cron->validate_who('not_a_real_user');
            },
        );

        # Ensure an error is thrown if we try to create a CronJob with an existing user
        $this->assert_does_not_throw(
            callable: function () {
                $cron = new CronJob(require_pkg: false);
                $cron->validate_who('root');
            },
        );
    }

    /**
     * Checks that the CronJob object is correctly represented in /etc/crontab after creation, modification and deletion.
     */
    public function test_crud(): void
    {
        # Create a CronJob to test with
        $cron_job = new CronJob(
            data: [
                'minute' => '15',
                'hour' => '6,12,18',
                'mday' => '5,15,20',
                'month' => '1',
                'wday' => '*',
                'who' => 'root',
                '',
                'command' => "echo 'restapi test'",
            ],
            require_pkg: false,
        );
        $cron_job->create();

        # Ensure the CronJob is found in /etc/crontab
        $minute = $cron_job->minute->value;
        $hour = $cron_job->hour->value;
        $mday = $cron_job->mday->value;
        $month = $cron_job->month->value;
        $wday = $cron_job->wday->value;
        $who = $cron_job->who->value;
        $command = $cron_job->command->value;
        $cron_line = "$minute $hour $mday $month $wday $who $command";
        $crontab = new Command('cat /etc/crontab', trim_whitespace: true);
        $this->assert_str_contains($crontab->output, $cron_line);

        # Update the CronJob to use an @ event and ensure it is correctly shown in /etc/crontab
        $prev_cron_line = $cron_line;
        $cron_job->minute->value = '@hourly';
        $cron_job->update();
        $cron_line = "{$cron_job->minute->value} {$cron_job->who->value} {$cron_job->command->value}";
        $crontab = new Command('cat /etc/crontab', trim_whitespace: true);
        $this->assert_str_does_not_contain($crontab->output, $prev_cron_line);
        $this->assert_str_contains($crontab->output, $cron_line);

        # Delete the cron job and ensure it is no longer present in /etc/crontab
        $cron_job->delete();
        $crontab = new Command('cat /etc/crontab', trim_whitespace: true);
        $this->assert_str_does_not_contain($crontab->output, $prev_cron_line);
        $this->assert_str_does_not_contain($crontab->output, $cron_line);
    }
}
