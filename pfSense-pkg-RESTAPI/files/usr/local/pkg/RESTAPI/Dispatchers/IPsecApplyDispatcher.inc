<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the IPsec configuration.
 */
class IPsecApplyDispatcher extends Dispatcher {
    /**
     * Reloads IPsec and associated services.
     */
    protected function _process(mixed ...$arguments): void {
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
