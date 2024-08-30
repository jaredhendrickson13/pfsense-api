<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for resetting the state table. This dispatcher allows the state table to be reset in the
 * background and on a delay to prevent the API from hanging or timing out while the state table is being reset.
 */
class FirewallStateTableResetDispatcher extends Dispatcher {
    /**
     * The number of seconds to delay before resetting the state table in the background.
     */
    const RESET_DELAY = 5;

    /**
     * Resets the entire state table.
     */
    protected function _process(mixed ...$arguments): void {
        sleep(self::RESET_DELAY);
        filter_flush_state_table();
    }
}
