<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Core\Model;

/**
 * Defines a Dispatcher for applying changes to the system hostname configuration.
 */
class SystemHostnameApplyDispatcher extends Dispatcher {
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
