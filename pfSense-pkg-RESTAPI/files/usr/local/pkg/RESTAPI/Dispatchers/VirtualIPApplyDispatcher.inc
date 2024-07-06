<?php

namespace RESTAPI\Dispatchers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the WireGuard configuration.
 */
class VirtualIPApplyDispatcher extends Dispatcher {
    /**
     * Apply pending virtual IP changes.
     */
    protected function _process(mixed ...$arguments): void {
        applyVIP();
    }
}
