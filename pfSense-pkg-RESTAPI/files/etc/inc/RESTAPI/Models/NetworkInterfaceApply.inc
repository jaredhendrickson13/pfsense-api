<?php

namespace RESTAPI\Models;

require_once("RESTAPI/autoloader.inc");

use RESTAPI;
use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\InterfaceField;

/**
 * A Model object that gathers details on pending interface changes as well as tools to apply pending interface changes.
 */
class NetworkInterfaceApply extends Model
{
    public BooleanField $applied;
    public InterfaceField $pending_interfaces;

    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null)
    {
        # Set model attributes
        $this->internal_callable = "get_applied_status";
        $this->always_apply = true;

        # Set model Fields
        $this->applied = new BooleanField(
            read_only: true,
            indicates_true: true,
            indicates_false: false,
            help_text: "Displays `true` when all interfaces are applied and there are no pending changes left.".
                "Displays `false` when there are pending interface changes that have not been applied."
        );
        $this->pending_interfaces = new InterfaceField(
            read_only: true,
            many: true,
            help_text: "Displays a list of interfaces that have pending changes waiting to be applied."
        );


        parent::__construct(id: $id, data: $data, client: $client);
    }

    /**
     * The internal callable used to populate the internal values for this Model's Fields. Gathers info on pending
     * interface changes.
     * @return array An array of internal values for Field objects in this Model
     */
    public function get_applied_status() : array {
        global $g;

        # Check if there are pending interface changes
        $applied = !is_file($g['tmp_path']."/.interfaces.apply");
        $pending_interfaces = [];

        # Populate pending interfaces if there are pending changes
        if (!$applied) {
            # Unserialize the file to determine which interfaces need to be applied
            $interfaces = unserialize(file_get_contents("{$g['tmp_path']}/.interfaces.apply"));
            $pending_interfaces = array_keys($interfaces);
        }

        return [
            "applied" => $applied,
            "pending_interfaces" => implode(",", $pending_interfaces)
        ];
    }

    public function _create() {
        # Override this method but don't do anything. We only want to apply the changes.
    }

    public function apply() {
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
                    if ($new_interface["ipaddrv6"] == "track6") {
                        interface_track6_configure($id, $new_interface, true);
                    }
                }
                # Otherwise, tear down this interface
                else {
                    interface_bring_down($id, true, $old_interface);
                }

                restart_interface_services($id, $new_interface['ipaddrv6']);
                $new_interface["mtu"] = config_get_path("interfaces/{$id}/mtu");
                
                # Check if VLANs need to be redone
                if (interface_has_clones($real_interface) &&
                    ($new_interface["mtu"] && ($new_interface["mtu"] != $real_interface_mtu)) ||
                    (!$new_interface["mtu"] && (get_interface_default_mtu() != $real_interface_mtu))) {
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
        if (is_subsystem_dirty('staticroutes') && (system_routing_configure() == 0)) {
            clear_subsystem_dirty('staticroutes');
        }

        # Reload packages
        send_event("service reload packages");

        # Clear the pending changes now that they're applied
        clear_subsystem_dirty('interfaces');
        @unlink("{$g['tmp_path']}/.interfaces.apply");
    }
}
