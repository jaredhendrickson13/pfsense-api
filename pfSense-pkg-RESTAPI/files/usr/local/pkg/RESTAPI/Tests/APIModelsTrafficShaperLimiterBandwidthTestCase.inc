<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallSchedule;
use RESTAPI\Models\TrafficShaperLimiter;
use RESTAPI\Models\TrafficShaperLimiterBandwidth;

class APIModelsTrafficShaperLimiterBandwidthTestCase extends TestCase
{
    /**
     * Checks that we can successfully create, update and delete limiters and that they are correctly represented
     * in dnctl
     */
    public function test_crud(): void
    {
        # Create a limiter to test with
        $limiter = new TrafficShaperLimiter(
            name: 'test',
            enabled: true,
            mask: 'none',
            aqm: 'droptail',
            sched: 'wf2q+',
            bandwidth: [['bw' => 20, 'bwscale' => 'Mb', 'sched' => 'none']],
            async: false,
        );
        $limiter->create(apply: true);

        # Ensure the limiter is seen in dnctl
        $dnctl = new Command('dnctl pipe show', trim_whitespace: true);
        $this->assert_str_contains($dnctl->output, '20.000 Mbit/s');

        # Update the limiter with new values and ensure it is updated in dnctl
        $limiter->from_representation(bandwidth: [['bw' => 400, 'bwscale' => 'Kb', 'sched' => 'none']]);
        $limiter->update(apply: true);
        $dnctl = new Command('dnctl pipe show', trim_whitespace: true);
        $this->assert_str_contains($dnctl->output, '400.000 Kbit/s');

        # Delete the limter and ensure it is no longer seen in dnctl
        $limiter->delete(apply: true);
        $dnctl = new Command('dnctl pipe show', trim_whitespace: true);
        $this->assert_str_does_not_contain($dnctl->output, $limiter->number->value . ':');
    }

    /**
     * Checks that TrafficShaperLimiterBandwidth objects can be assigned existing FirewallSchedules.
     */
    public function test_bwsched(): void {
        $sched = new FirewallSchedule(
            name: "test_schedule",
            timerange: [
                [
                    'month' => [1],
                    'day' => [5],
                    'hour' => '0:00-23:59'
                ]
            ],
            async: true
        );
        $sched->create();

        # Create a limiter to test with
        $limiter = new TrafficShaperLimiter(
            name: 'test',
            enabled: true,
            mask: 'none',
            aqm: 'droptail',
            sched: 'wf2q+',
            bandwidth: [['bw' => 20, 'bwscale' => 'Mb', 'sched' => $sched->name->value]],
            async: false,
        );
        $limiter->create(apply: true);

        # Ensure we can update the limiter to remove the schedule
        $limiter->from_representation(
            bandwidth: [['bw' => 20, 'bwscale' => 'Mb', 'sched' => 'none']],
        );
        $limiter->update(apply: true);

        # Delete the limiter
        $limiter->delete(apply: true);
    }
}
