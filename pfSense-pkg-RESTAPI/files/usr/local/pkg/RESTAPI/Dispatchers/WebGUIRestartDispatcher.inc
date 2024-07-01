<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Command;
use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for restarting the web GUI.
 */
class WebGUIRestartDispatcher extends Dispatcher {
    /**
     * Restarts the web GUI.
     */
    protected function _process(mixed ...$arguments): void {
        # Wait a second to prevent prematurely killing API calls
        sleep(3);
        new Command('/etc/rc.restart_webgui');
    }
}
