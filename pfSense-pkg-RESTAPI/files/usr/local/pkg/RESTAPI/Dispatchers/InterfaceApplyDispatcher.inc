<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to interface configurations.
 */
class InterfaceApplyDispatcher extends Dispatcher {
    # Do not limit the number concurrent filter reloads that can run
    public int $max_concurrency = 1;

    /**
     * Applies pending interface changes.
     */
    protected function _process(mixed ...$arguments): void {
        # Local variables
        global $g;
        unlink_if_exists("{$g['tmp_path']}/config.cache");
        $vlan_redo = [];

        # Only apply interfaces changes if the cache of interface changes is present
        if (file_exists("{$g['tmp_path']}/.interfaces.apply")) {
            # Unserialize the file to determine which interfaces need to be applied
            $interfaces = unserialize(file_get_contents("{$g['tmp_path']}/.interfaces.apply"));

            # Loop through each interface to configure and restart services
            foreach ($interfaces as $id => $old_interface) {
                # Determine the real interface name and it's current MTU
                $new_interface = config_get_path("interfaces/{$id}");
                $real_interface = get_real_interface($id);
                $real_interface_mtu = get_interface_mtu($real_interface);

                # Configure the interface if it's enabled
                if (config_path_enabled("interfaces/{$id}")) {
                    interface_bring_down($id, false, $old_interface);
                    interface_configure($id, true);

                    # Reconfigure track6 if this is a track6 enabled interface
                    if ($new_interface['ipaddrv6'] == 'track6') {
                        interface_track6_configure($id, $new_interface, true);
                    }
                }
                # Otherwise, tear down this interface
                else {
                    interface_bring_down($id, true, $old_interface);
                }

                restart_interface_services($id, $new_interface['ipaddrv6']);
                $new_interface['mtu'] = config_get_path("interfaces/{$id}/mtu");

                # Check if VLANs need to be redone
                if (
                    (interface_has_clones($real_interface) &&
                        ($new_interface['mtu'] && $new_interface['mtu'] != $real_interface_mtu)) ||
                    (!$new_interface['mtu'] && get_interface_default_mtu() != $real_interface_mtu)
                ) {
                    $vlan_redo[] = $real_interface;
                }
            }
        }

        # Reconfigure VLANs if any VLANs needed to be redone
        foreach ($vlan_redo as $vlredo) {
            interfaces_vlan_configure_mtu($vlredo);
        }

        # Reload monitoring, filter and graphing services
        setup_gateways_monitor();
        filter_configure();
        enable_rrd_graphing();

        # Reload routing services
        if (is_subsystem_dirty('staticroutes') && system_routing_configure() == 0) {
            clear_subsystem_dirty('staticroutes');
        }

        # Reload packages
        send_event('service reload packages');

        # Clear the pending changes now that they're applied
        clear_subsystem_dirty('interfaces');
        @unlink("{$g['tmp_path']}/.interfaces.apply");
    }
}
