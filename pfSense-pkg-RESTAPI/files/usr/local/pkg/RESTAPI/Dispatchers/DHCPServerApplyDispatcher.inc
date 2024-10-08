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
        $retvaldhcp = 0;
        $retvaldns = 0;

        if (Model::is_config_enabled('dnsmasq') && Model::is_config_enabled('dnsmasq', 'regdhcpstatic')) {
            $retvaldns |= services_dnsmasq_configure();
            if ($retvaldns == 0) {
                clear_subsystem_dirty('hosts');
                clear_subsystem_dirty('dhcpd');
            }
            if (Model::is_config_enabled('unbound') && Model::is_config_enabled('unbound', 'regdhcpstatic')) {
                $retvaldns |= services_unbound_configure();
                if ($retvaldns == 0) {
                    clear_subsystem_dirty('unbound');
                    clear_subsystem_dirty('hosts');
                    clear_subsystem_dirty('dhcpd');
                }
            }
        } else {
            $retvaldhcp |= services_dhcpd_configure();
            if ($retvaldhcp == 0) {
                clear_subsystem_dirty('dhcpd');
            }
        }
        /* BIND package - Bug #3710 */
        if (!function_exists('is_package_installed')) {
            require_once 'pkg-utils.inc';
        }
        if (
            is_package_installed('pfSense-pkg-bind') &&
            Model::is_config_enabled('installedpackages/bind/config/0', 'enable_bind')
        ) {
            $reloadbind = false;
            if (is_array(Model::get_config('installedpackages/bindzone'))) {
                $bindzone = Model::get_config('installedpackages/bindzone/config');
            } else {
                $bindzone = [];
            }
            for ($x = 0; $x < sizeof($bindzone); $x++) {
                $zone = $bindzone[$x];
                if ($zone['regdhcpstatic'] == 'on') {
                    $reloadbind = true;
                    break;
                }
            }
            if ($reloadbind === true) {
                if (file_exists('/usr/local/pkg/bind.inc')) {
                    require_once '/usr/local/pkg/bind.inc';
                    bind_sync();
                }
            }
        }
        filter_configure();
    }
}
