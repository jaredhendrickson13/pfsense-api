<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Models\RESTAPISettingsSync;

class RESTAPISettingsSyncDispatcher extends Dispatcher
{
    public int $timeout = 60;

    /**
     * Starts the REST API settings sync to HA peers.
     */
    public function process(...$arguments): void
    {
        RESTAPISettingsSync::sync();
    }
}