<?php

namespace API\Models;

use API\Core\Auth;
use API\Core\Model;
use API\Fields\BooleanField;

class DHCPServerApply extends Model
{
    public BooleanField $applied;

    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null)
    {
        # Set model attributes
        $this->internal_callable = "get_applied_status";
        $this->always_apply = true;

        # Set model fields
        $this->applied = new BooleanField(
            read_only: true,
            indicates_true: true,
            indicates_false: false,
            help_text: "Displays `true` if all DHCP server changes are applied and there are no pending changes left.".
            "Displays `false` when there are pending DHCP server changes that have not been applied."
        );
        parent::__construct(id: $id, data: $data, client: $client);
    }

    /**
     * Obtains the current applied status.
     * @return array The internal value array to be loaded by this model as the internal callable
     */
    public function get_applied_status() : array {
        return ["applied" => !is_subsystem_dirty("dhcpd")];
    }

    /**
     * Applies changes to the DHCP server configuration.
     */
    public function apply() {
        $retvaldhcp = 0;
        $retvaldns = 0;

        if ($this->is_config_enabled('dnsmasq') && $this->is_config_enabled('dnsmasq', 'regdhcpstatic')) {
            $retvaldns |= services_dnsmasq_configure();
            if ($retvaldns == 0) {
                clear_subsystem_dirty('hosts');
                clear_subsystem_dirty('staticmaps');
            }
            if ($this->is_config_enabled('unbound') && $this->is_config_enabled('unbound', 'regdhcpstatic')) {
                $retvaldns |= services_unbound_configure();
                if ($retvaldns == 0) {
                    clear_subsystem_dirty('unbound');
                    clear_subsystem_dirty('hosts');
                    clear_subsystem_dirty('staticmaps');
                }
            }
        } else {
            $retvaldhcp |= services_dhcpd_configure();
            if ($retvaldhcp == 0) {
                clear_subsystem_dirty('staticmaps');
            }
        }
        /* BIND package - Bug #3710 */
        if (!function_exists('is_package_installed')) {
            require_once('pkg-utils.inc');
        }
        if (is_package_installed('pfSense-pkg-bind') && $this->is_config_enabled('installedpackages/bind/config/0', 'enable_bind')) {
            $reloadbind = false;
            if (is_array($this->get_config('installedpackages/bindzone'))) {
                $bindzone = $this->get_config('installedpackages/bindzone/config');
            } else {
                $bindzone = array();
            }
            for ($x = 0; $x < sizeof($bindzone); $x++) {
                $zone = $bindzone[$x];
                if ($zone['regdhcpstatic'] == 'on') {
                    $reloadbind = true;
                    break;
                }
            }
            if ($reloadbind === true) {
                if (file_exists("/usr/local/pkg/bind.inc")) {
                    require_once("/usr/local/pkg/bind.inc");
                    bind_sync();
                }
            }
        }
        filter_configure();
    }
}
