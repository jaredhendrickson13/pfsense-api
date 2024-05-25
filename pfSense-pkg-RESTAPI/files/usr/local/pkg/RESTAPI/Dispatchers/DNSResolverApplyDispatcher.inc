<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the DNS Resolver configuration.
 */
class DNSResolverApplyDispatcher extends Dispatcher {
    /**
     * Reloads the DNS Resolver and associated services.
     */
    protected function _process(mixed ...$arguments): void {
        $reload_unbound = 0;
        $reload_unbound |= services_unbound_configure();
        if ($reload_unbound === 0) {
            system_resolvconf_generate();
            system_dhcpleases_configure();
            clear_subsystem_dirty('unbound');
        }
    }
}
