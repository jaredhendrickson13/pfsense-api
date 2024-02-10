<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Command;
use RESTAPI\Core\Dispatcher;

class SystemRebootDispatcher extends Dispatcher {
    # Only allow one reboot at a time
    public int $max_concurrency = 1;

    /**
     * Schedules a reboot in 15 seconds
     */
    protected function _process(mixed ...$arguments): void {
        new Command('shutdown -r +15s');
    }
}
