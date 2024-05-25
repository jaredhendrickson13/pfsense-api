<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher that applies pending Traffic Shaper changes.
 */
class TrafficShaperApplyDispatcher extends Dispatcher {
    /**
     * Reloads the traffic shaper services.
     */
    protected function _process(mixed ...$arguments): void {
        # Reload filter services
        filter_configure_sync();

        # Reset RRD graphing for shapers
        system('rm -f /var/db/rrd/*queuedrops.rrd');
        system('rm -f /var/db/rrd/*queues.rrd');
        enable_rrd_graphing();
    }
}
