<?php

namespace RESTAPI\Dispatchers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the BIND settings.
 */
class BINDApplyDispatcher extends Dispatcher {
    protected array $required_packages = ['pfSense-pkg-bind'];
    protected array $package_includes = ['bind.inc'];

    /**
     * Applies changes to the BIND settings.
     */
    protected function _process(mixed ...$arguments): void {
        bind_sync();
    }
}
