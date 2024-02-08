<?php

namespace RESTAPI\Dispatchers;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Dispatcher;

class WireGuardSettingsDispatcher extends Dispatcher
{
    # Only allow one dispatcher process to run at a time
    public int $max_concurrency = 1;
    
    /**
     * Starts/stops the WireGuard service and applies changes to tunnels.
     */
    public function process(...$arguments): void
    {
        # TODO: Allow Models to pass $package_includes to the Dispatcher and import these automatically
        require_once("wireguard/includes/wg_service.inc");
        require_once("wireguard/includes/wg.inc");
        
        # Enable or disable WireGuard
        wg_toggle_wireguard();

        # Apply changes to tunnels if WireGuard is running
        if (wg_is_service_running()) {
            $tunnels_to_apply = wg_apply_list_get('tunnels');
            wg_tunnel_sync(tunnel_names: $tunnels_to_apply, restart_services: true, resolve_endpoints: true);
        }
    }
}
