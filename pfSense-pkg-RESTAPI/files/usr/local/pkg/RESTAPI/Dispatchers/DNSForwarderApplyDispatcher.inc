<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

class DNSForwarderApplyDispatcher extends Dispatcher {
    /**
     * Reloads the DNS Forwarder and associated services.
     */
    public function process(...$arguments): void {
        # Reload the DNS Forwarder service
        services_dnsmasq_configure();

        # Reload other dependent services
        filter_configure();
        system_resolvconf_generate();
        system_dhcpleases_configure();
        clear_subsystem_dirty('hosts');
    }
}
