<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

class IPsecApplyDispatcher extends Dispatcher
{
    public $max_concurrency = 1;
    public $timeout = 120;
    public static function process(...$arguments)
    {
        # Restart IPsec and related services
        $ipsec_dynamic_hosts = ipsec_configure();
        ipsec_reload_package_hook();
        filter_configure();

        # Clear the pending changes banner if present
        if ($ipsec_dynamic_hosts >= 0 and is_subsystem_dirty('ipsec')) {
            clear_subsystem_dirty('ipsec');
        }
    }
}