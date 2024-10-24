<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Models\RESTAPISettingsSync;

/**
 * Defines a Dispatcher for syncing REST API settings to HA peers.
 */
class RESTAPISettingsSyncDispatcher extends Dispatcher {
    public int $timeout = 60;
    public int $max_queue = 1024;

    /**
     * Starts the REST API settings sync to HA peers.
     */
    public function process(...$arguments): void {
        RESTAPISettingsSync::sync();
    }
}
