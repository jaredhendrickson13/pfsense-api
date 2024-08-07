<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Command;
use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for shutting down the system.
 */
class SystemHaltDispatcher extends Dispatcher {
    # Only allow one shutdown at a time
    public int $max_queue = 1;

    /**
     * Schedules a shutdown in 15 seconds
     */
    protected function _process(mixed ...$arguments): void {
        new Command('shutdown -h +15s');
    }
}
