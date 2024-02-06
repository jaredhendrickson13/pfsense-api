<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher that applies pending Traffic Shaper changes.
 */
class TrafficShaperApplyDispatcher extends Dispatcher {
    # Do not limit the number concurrent traffic shaper reloads that can run
    public int $max_concurrency = 0;

    /**
     * Reloads the traffic shaper services.
     */
    public function process(mixed ...$arguments): void {
        # Reload filter services
        filter_configure_sync();

        # Reset RRD graphing for shapers
        system('rm -f /var/db/rrd/*queuedrops.rrd');
        system('rm -f /var/db/rrd/*queues.rrd');
        enable_rrd_graphing();
    }
}
