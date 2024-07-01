<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\TrafficShaperLimiter;
use RESTAPI\Models\TrafficShaperLimiterQueue;

class APIModelsTrafficShaperLimiterQueueTestCase extends TestCase {
    /**
     * Checks that we can successfully create, update and delete limiters and that they are correctly represented
     * in dnctl
     */
    public function test_crud(): void {
        # Create a parent limiter to test with
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

        # Create a child queue to test with
        $queue = new TrafficShaperLimiterQueue(
            parent_id: $limiter->id,
            name: 'test_queue',
            enabled: true,
            mask: 'srcaddress',
            maskbits: 32,
            maskbitsv6: 128,
            qlimit: 65,
            aqm: 'droptail',
            weight: 30,
            buckets: 120,
            async: false,
        );
        $queue->create(apply: true);

        # Ensure the limiter is seen in dnctl
        $dnctl = new Command('dnctl queue show', trim_whitespace: true);
        $this->assert_str_contains($dnctl->output, '65 sl.');
        $this->assert_str_contains($dnctl->output, 'weight 30');
        $this->assert_str_contains($dnctl->output, '120 buckets');
        $this->assert_str_contains($dnctl->output, 'mask: 0x00 0xffffffff/0x0000 -> 0x00000000/0x0000');

        # Update the limiter with new values and ensure it is updated in dnctl
        $queue->from_representation(qlimit: 80, weight: 50, buckets: 200);
        $queue->update(apply: true);
        $dnctl = new Command('dnctl queue show', trim_whitespace: true);
        $this->assert_str_contains($dnctl->output, '80 sl.');
        $this->assert_str_contains($dnctl->output, 'weight 50');
        $this->assert_str_contains($dnctl->output, '200 buckets');

        # Delete the limiter and ensure it is no longer seen in dnctl
        $queue->delete(apply: true);
        $dnctl = new Command('dnctl queue show', trim_whitespace: true);
        $this->assert_is_empty($dnctl->output);
        $limiter->delete(apply: true);
    }
}
