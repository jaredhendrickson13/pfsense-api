<?php

namespace RESTAPI\Dispatchers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the WireGuard configuration.
 */
class VirtualIPApplyDispatcher extends Dispatcher {
    public int $max_queue = 1; // Only allow one Virtual IP apply operation at a time

    /**
     * Apply pending virtual IP changes.
     */
    protected function _process(mixed ...$arguments): void {
        applyVIP();
    }
}
