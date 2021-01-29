<?php
//   Copyright 2021 Jared Hendrickson
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


class APIServicesDHCPdStaticMappingDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dhcpserver-editstaticmapping"];
        $this->change_note = "Deleted DHCP static mapping via API";
    }

    public function action() {
        $del_data = $this->config["dhcpd"][$this->initial_data["interface"]]["staticmap"][$this->id];
        unset($this->config["dhcpd"][$this->initial_data["interface"]]["staticmap"][$this->id]);
        $this->write_config();
        $this->reload_dhcpd();
        return APIResponse\get(0, $del_data);
    }

    public function validate_payload() {
        # Check for our required 'interface' payload value
        if (isset($this->initial_data["interface"])) {
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            # Check that interface exists
            if (is_string($this->initial_data["interface"])) {
                $if_conf = $this->config["interfaces"][$this->initial_data["interface"]];
                # Check that interface hosts a static network with multiple addresses
                if (isset($if_conf["enable"]) and is_ipaddrv4($if_conf["ipaddr"]) and $if_conf["subnet"] <= 31) {

                } else {
                    $this->errors[] = APIResponse\get(2019);
                }
            } else {
                $this->errors[] = APIResponse\get(2018);
            }
        } else {
            $this->errors[] = APIResponse\get(2017);
        }

        # Check for our required 'id' payload value
        if (isset($this->initial_data["id"])) {
            # Ensure this ID exists
            $dhcp_maps = $this->config["dhcpd"][$this->initial_data["interface"]]["staticmap"];
            if (array_key_exists($this->initial_data["id"], $dhcp_maps)) {
                $this->id = $this->initial_data["id"];
            } else {
                $this->errors[] = APIResponse\get(2045);
            }
        } else {
            $this->errors[] = APIResponse\get(2044);
        }
    }

    # Executes functions to reload DHCP and corresponding/dependent services
    protected function reload_dhcpd() {
        $retvaldhcp = 0;
        $retvaldns = 0;
        /* dnsmasq_configure calls dhcpd_configure */
        /* no need to restart dhcpd twice */
        if (isset($this->config['dnsmasq']['enable']) && isset($this->config['dnsmasq']['regdhcpstatic']))	{
            $retvaldns |= services_dnsmasq_configure();
            if ($retvaldns == 0) {
                clear_subsystem_dirty('hosts');
                clear_subsystem_dirty('staticmaps');
            }
        } else if (isset($this->config['unbound']['enable']) && isset($this->config['unbound']['regdhcpstatic'])) {
            $retvaldns |= services_unbound_configure();
            if ($retvaldns == 0) {
                clear_subsystem_dirty('unbound');
                clear_subsystem_dirty('hosts');
                clear_subsystem_dirty('staticmaps');
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
        if (is_package_installed('pfSense-pkg-bind') && isset($this->config['installedpackages']['bind']['config'][0]['enable_bind'])) {
            $reloadbind = false;
            if (is_array($this->config['installedpackages']['bindzone'])) {
                $bindzone = $this->config['installedpackages']['bindzone']['config'];
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