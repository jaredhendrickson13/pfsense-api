<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Core\Model;

class SystemHostnameApplyDispatcher extends Dispatcher {
    # Only allow the system hostname services to be applied one at a time
    public int $max_concurrency = 1;

    /**
     * Restarts system hostname services.
     */
    protected function _process(mixed ...$arguments): void {
        # Restart system hostname/DNS services
        system_hostname_configure();
        system_hosts_generate();
        system_resolvconf_generate();

        # Restart DNS server services if configured
        if (Model::is_config_enabled('dnsmasq')) {
            services_dnsmasq_configure();
        } elseif (Model::is_config_enabled('unbound')) {
            services_unbound_configure();
        }

        # Apply filter changes in case rules were affected by the changes
        filter_configure();
    }
}
