<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Core\Model;

/**
 * Defines a Dispatcher for applying changes to the DHCP server configuration.
 */
class DHCPServerApplyDispatcher extends Dispatcher {
    /**
     * Reloads the DHCP server and associated services.
     */
    protected function _process(mixed ...$arguments): void {
        dhcp_apply_changes();
        clear_subsystem_dirty('dhcpd');
    }
}
