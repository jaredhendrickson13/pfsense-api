<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Command;
use RESTAPI\Core\Dispatcher;

class WebGUIRestartDispatcher extends Dispatcher
{
    # Don't limit the number of Web GUI restarts that can run at a time.
    public int $max_concurrency = 0;

    /**
     * Restarts the web GUI.
     */
    public function process(mixed ...$arguments): void
    {
        # Wait a second to prevent prematurely killing API calls
        sleep(3);
        new Command('/etc/rc.restart_webgui');
    }
}
