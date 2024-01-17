<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\TrafficShaper;
use RESTAPI\Models\TrafficShaperQueue;

class APIModelsTrafficShaperQueueTestCase extends TestCase
{
    /**
     * Ensure TrafficShaperQueues cannot be created for parent TrafficShapers using the CODELQ scheduler.
     */
    public function test_no_child_queues_when_codelq_parent(): void {
        # Create a parent TrafficShaper and child TrafficShaperQueue to use when testing
        $shaper = new TrafficShaper(
            data: [
                "interface" => "lan",
                "scheduler" => "PRIQ",
                "bandwidthtype" => "Mb",
                "bandwidth" => 100
            ]
        );
        $shaper->create();
        $queue = new TrafficShaperQueue(
            parent_id: $shaper->id,
            data: []
        );
        
        # Ensure the queue cannot be created because the parent uses the CODELQ scheduler
    }

}
