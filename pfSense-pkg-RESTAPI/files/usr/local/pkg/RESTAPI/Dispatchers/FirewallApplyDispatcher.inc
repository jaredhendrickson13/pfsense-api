<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the firewall filter configuration.
 */
class FirewallApplyDispatcher extends Dispatcher {
    # Do not limit the number concurrent filter reloads that can run
    public int $max_concurrency = 0;

    /**
     * Reloads the filter services.
     */
    protected function _process(mixed ...$arguments): void {
        if ($this->async) {
            filter_configure();
        }
        else {
            filter_configure_sync();
        }
    }
}
