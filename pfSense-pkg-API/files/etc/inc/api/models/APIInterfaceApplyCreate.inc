<?php
//   Copyright 2023 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIInterfaceApplyCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-assignnetworkports"];
        $this->change_note = "Applied interface via API";
    }

    public function action() {
        # Check if the client requested asynchronous application
        if ($this->initial_data["async"] === true) {
            $this->apply_async();
            return APIResponse\get(14);
        }
        # Otherwise, apply interfaces in sequence
        else {
            $this->apply();
            return APIResponse\get(0);
        }

    }

    # Write the pending changes to a serialized file to apply later
    public static function serialize_changes($id, $interface) {
        # Local variables
        global $g;
        $pending_ifs = [];

        # Check for any existing pending interface changes
        if (file_exists($g['tmp_path']."/.interfaces.apply")) {
            $pending_ifs = unserialize(file_get_contents($g['tmp_path']."/.interfaces.apply"));
        }

        # Set the current validated interface as a pending interface change
        $pending_ifs[$id] = [];
        $pending_ifs[$id]['ifcfg'] = $interface;
        file_put_contents($g['tmp_path']."/.interfaces.apply", serialize($pending_ifs));
        mark_subsystem_dirty('interfaces');
    }

    public static function apply() {
        # Local variables
        global $g;
        $mtu_redo = array();

        # Remove configuration cache if it exists
        unlink_if_exists($g['tmp_path']."/config.cache");

        # Check that there are even pending interface changes to apply
        if (file_exists("{$g['tmp_path']}/.interfaces.apply")) {
            # Load our pending interface file
            $pending_ifs = unserialize(file_get_contents($g['tmp_path']."/.interfaces.apply"));

            # Loop through each pending interface configuration and apply it accordingly
            foreach ($pending_ifs as $if_to_apply => $if_conf) {
                # Check this interface's MTU value
                $if_mtu = get_interface_mtu(get_real_interface($if_to_apply));

                # Only apply changes to interfaces that are enabled
                if (APIModel::is_config_enabled("interfaces/{$if_to_apply}")) {
                    # Bring down existing interface and configure the new one
                    interface_bring_down($if_to_apply, false, $if_conf);
                    interface_configure($if_to_apply, true);

                    # Call track6 specific configuration steps
                    if (APIModel::get_config("interfaces/{$if_to_apply}/ipaddrv6") == "track6") {
                        $wancfg = APIModel::get_config("interfaces/{$if_to_apply}");
                        interface_track6_configure($if_to_apply, $wancfg, true);
                    }
                }
                # If interface was not enabled (or was disabled) remove any existing configurations utilizing interface
                else {
                    # Bring down the existing interface on the system if it exists
                    interface_bring_down($if_to_apply, true, $if_conf);

                    # Restart DHCP services to remove DHCP server(s) on this interface
                    if (APIModel::is_config_enabled("dhcpd/{$if_to_apply}") or APIModel::is_config_enabled("dhcpdv6/{$if_to_apply}")) {
                        services_dhcpd_configure();
                    }
                }

                # Interfaces with sub-interfaces (e.g. VLANs) need their MTU reconfigured if changed
                if (interface_has_clones(get_real_interface($if_to_apply))) {
                    # When static MTU is set
                    if (APIModel::is_config_enabled("interfaces/{$if_to_apply}", "mtu")) {
                        # Interface must be redone if MTU has changed
                        if (APIModel::get_config("interfaces/{$if_to_apply}/mtu") != $if_mtu) {
                            $mtu_redo[] = get_real_interface($if_to_apply);
                        }
                    }
                    # When default (unspecified) MTU is set
                    else {
                        # Interface must be redone if MTU has changed
                        if (get_interface_default_mtu() != $if_mtu) {
                            $mtu_redo[] = get_real_interface($if_to_apply);
                        }
                    }
                }
            }
        }

        # Reconfigure interface MTU if MTU changes were present
        if (!empty($mtu_redo)) {
            foreach ($mtu_redo as $vlredo) {
                interfaces_vlan_configure_mtu($vlredo);
            }
        }

        # Restart associated/dependent services
        services_snmpd_configure();
        setup_gateways_monitor();
        clear_subsystem_dirty('interfaces');
        filter_configure();
        enable_rrd_graphing();

        # Restart routing services to accommodate interface changes. Clear routing subsystem if it was successful.
        if (is_subsystem_dirty('staticroutes') and system_routing_configure() === 0) {
            clear_subsystem_dirty('staticroutes');
        }

        # Initialize syslog configuration and restart the syslog service
        if (APIModel::is_config_enabled("syslog") and $if_to_apply == APIModel::get_config("syslog/sourceip")) {
            system_syslogd_start();
        }

        # After all is said and done, clear the interface subsystem status and remove the pending interface change file
        clear_subsystem_dirty('interfaces');
        @unlink($g['tmp_path']."/.interfaces.apply");
    }

    # Apply interfaces asynchronously
    public static function apply_async() {
        # Execute the helper script asynchronously.
        shell_exec("nohup php -f /usr/local/share/pfSense-pkg-API/scripts/apply_interfaces.php > /dev/null &");
    }
}
