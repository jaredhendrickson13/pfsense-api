<?php

namespace API\Dispatchers;

use API\Core\Dispatcher;

class FirewallApplyDispatcher extends Dispatcher
{
    # Do not limit the number concurrent filter reloads that can run
    public int $max_concurrency = 0;

    /**
     * Reloads the filter services.
     */
    public static function process(...$arguments)
    {
        filter_configure_sync();
    }
}