<?php

namespace RESTAPI\Dispatchers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Dispatcher;

class WireGuardApplyDispatcher extends Dispatcher {
    public int $max_concurrency = 1; // Only allow one dispatcher process to run at a time
    protected array $required_packages = ['pfSense-pkg-WireGuard'];
    protected array $package_includes = ['wireguard/includes/wg.inc', 'wireguard/includes/wg_service.inc'];

    /**
     * Starts/stops the WireGuard service and applies changes to tunnels.
     */
    protected function _process(mixed ...$arguments): void {
        # Enable or disable WireGuard
        wg_toggle_wireguard();

        # Apply changes to tunnels if WireGuard is running
        if (wg_is_service_running()) {
            $tunnels_to_apply = wg_apply_list_get('tunnels');
            wg_tunnel_sync(tunnel_names: $tunnels_to_apply, restart_services: true, resolve_endpoints: true);
        }
        
        clear_subsystem_dirty('wireguard');
    }
}
