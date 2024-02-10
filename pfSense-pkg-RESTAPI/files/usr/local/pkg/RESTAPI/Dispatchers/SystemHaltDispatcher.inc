<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Command;
use RESTAPI\Core\Dispatcher;

class SystemHaltDispatcher extends Dispatcher {
    # Only allow one shutdown at a time
    public int $max_concurrency = 1;

    /**
     * Schedules a shutdown in 15 seconds
     */
    protected function _process(mixed ...$arguments): void {
        new Command('shutdown -h +15s');
    }
}
