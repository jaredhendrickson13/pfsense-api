<?php

namespace API\Dispatchers;

use API\Core\Dispatcher;

class TestDispatcher extends Dispatcher
{
    public int $max_concurrency = 1;
    public int $timeout = 30;

    /**
     * Emulate a process that takes a long time
     */
    public static function process(...$arguments)
    {
        sleep(60);
    }
}
