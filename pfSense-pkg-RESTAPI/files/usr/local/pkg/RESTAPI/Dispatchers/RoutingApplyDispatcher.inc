<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Models\RoutingGateway;
use RESTAPI\Models\RoutingGatewayGroup;

/**
 * Defines a Dispatcher for applying changes to the routing configuration.
 */
class RoutingApplyDispatcher extends Dispatcher {
    /**
     * Reload routing services to apply routing changes.
     */
    public function process(...$arguments): void {
        global $g;

        # Check for the pending changes file and unserialize it
        if (file_exists("{$g['tmp_path']}/.system_routes.apply")) {
            $to_apply_list = unserialize(file_get_contents("{$g['tmp_path']}/.system_routes.apply"));

            # Run commands to apply these changes
            foreach ($to_apply_list as $to_apply) {
                mwexec("{$to_apply}");
            }

            @unlink("{$g['tmp_path']}/.system_routes.apply");
        }

        # Reload associated services
        system_routing_configure();
        system_resolvconf_generate();
        filter_configure();
        setup_gateways_monitor();
        send_multiple_events(['service reload dyndnsall', 'service reload ipsecdns', 'filter reload']);

        # Check if any gateway groups are dirty, if so, resync them with OpenVPN
        foreach (RoutingGatewayGroup::read_all()->model_objects as $gateway_group) {
            $gw_subsystem = "gwgroup.{$gateway_group->name->value}";
            if (is_subsystem_dirty($gw_subsystem)) {
                openvpn_resync_gwgroup($gateway_group->name->value);
                clear_subsystem_dirty($gw_subsystem);
            }
        }

        clear_subsystem_dirty('staticroutes');
    }
}
