<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

class DNSForwarderApplyDispatcher extends Dispatcher
{
    # Only allow one Dnsmasq reload to occur at a time
    public int $max_concurrency = 1;

    public function process(...$arguments): void
    {
        # Reload the DNS Forwarder service
        services_dnsmasq_configure();

        # Reload other dependent services
        filter_configure();
        system_resolvconf_generate();
        system_dhcpleases_configure();
        clear_subsystem_dirty('hosts');
    }
}