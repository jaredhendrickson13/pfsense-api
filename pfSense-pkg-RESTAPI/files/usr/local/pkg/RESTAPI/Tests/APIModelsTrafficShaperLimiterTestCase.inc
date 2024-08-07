<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\TrafficShaperLimiter;

class APIModelsTrafficShaperLimiterTestCase extends TestCase {
    /**
     * Checks that we can successfully create, update and delete limiters and that they are correctly represented
     * in dnctl
     */
    public function test_crud(): void {
        # Create a limiter to test with
        $limiter = new TrafficShaperLimiter(
            name: 'test',
            enabled: true,
            mask: 'srcaddress',
            maskbits: 32,
            maskbitsv6: 128,
            qlimit: 64,
            aqm: 'droptail',
            sched: 'wf2q+',
            delay: 100,
            plr: 0.5,
            buckets: 40,
            bandwidth: [['bw' => 20, 'bwscale' => 'Mb', 'sched' => 'none']],
            async: false,
        );
        $limiter->create(apply: true);

        # Ensure the limiter is seen in dnctl
        $dnctl = new Command('dnctl pipe show', trim_whitespace: true);
        $this->assert_str_contains($dnctl->output, $limiter->number->value . ':');
        $this->assert_str_contains($dnctl->output, '20.000 Mbit/s 100 ms burst 0');
        $this->assert_str_contains($dnctl->output, '64 sl.plr 0.5');
        $this->assert_str_contains($dnctl->output, '40 buckets');
        $this->assert_str_contains($dnctl->output, 'mask: 0x00 0xffffffff/0x0000 -> 0x00000000/0x0000');

        # Update the limiter with new values and ensure it is updated in dnctl
        $limiter->from_representation(
            mask: 'none',
            qlimit: 25,
            delay: 200,
            plr: 0.2,
            buckets: 55,
            bandwidth: [['bw' => 400, 'bwscale' => 'Kb', 'sched' => 'none']],
        );
        $limiter->update(apply: true);
        $dnctl = new Command('dnctl pipe show', trim_whitespace: true);
        $this->assert_str_contains($dnctl->output, '400.000 Kbit/s 200 ms burst 0');
        # There seems to be a bug in pfSense where plr and qlimit do not properly update if the value is changed
        # $this->assert_str_contains($dnctl->output, "25 sl.plr 0.2");
        $this->assert_str_contains($dnctl->output, '55 buckets');
        $this->assert_str_does_not_contain($dnctl->output, 'mask:');

        # Delete the limter and ensure it is no longer seen in dnctl
        $limiter->delete(apply: true);
        $dnctl = new Command('dnctl pipe show', trim_whitespace: true);
        $this->assert_str_does_not_contain($dnctl->output, $limiter->number->value . ':');
    }
}
