<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the firewall filter configuration.
 */
class FirewallApplyDispatcher extends Dispatcher {
    /**
     * Reloads the filter services.
     */
    protected function _process(mixed ...$arguments): void {
        if ($this->async) {
            filter_configure();
        } else {
            filter_configure_sync();
        }
    }
}
