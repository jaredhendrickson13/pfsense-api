<?php
//   Copyright 2022 Jared Hendrickson
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


class APIServicesDHCPdStaticMappingCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dhcpserver-editstaticmapping"];
        $this->change_note = "Added DHCP static mapping via API";
    }

    public function action() {
        # Init the staticmap config
        if (!is_array($this->config["dhcpd"][$this->id]["staticmap"])) {
            $this->config["dhcpd"][$this->id]["staticmap"] = [];
        }
        $this->config["dhcpd"][$this->id]["staticmap"][] = $this->validated_data;
        $this->write_config();
        $this->reload_dhcpd();
        return APIResponse\get(0, $this->validated_data);
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
                    $this->id = $this->initial_data["interface"];
                } else {
                    $this->errors[] = APIResponse\get(2019);
                }
            } else {
                $this->errors[] = APIResponse\get(2018);
            }
        } else {
            $this->errors[] = APIResponse\get(2017);
        }

        # Check for our required 'mac' payload value
        if (isset($this->initial_data["mac"])) {
            # Ensure MAC address is valid
            if (is_macaddr($this->initial_data["mac"])) {
                # Loop through existing mappings and ensure this MAC doesn't exist already
                foreach ($this->config["dhcpd"][$this->id]["staticmap"] as $map) {
                    if ($map["mac"] === $this->initial_data["mac"]) {
                        $this->errors[] = APIResponse\get(2036);
                    }
                }
                $this->validated_data["mac"] = $this->initial_data["mac"];
            } else {
                $this->errors[] = APIResponse\get(2035);
            }
        } else {
            $this->errors[] = APIResponse\get(2034);
        }

        # Check for our required 'ipaddr' payload value
        if (isset($this->initial_data["ipaddr"])) {
            # Ensure MAC address is valid
            if (is_ipaddrv4($this->initial_data["ipaddr"])) {
                $ip = ip2long($this->initial_data["ipaddr"]);
                $gw_ip = ip2long($if_conf["ipaddr"]);
                $net_ip = ip2long(gen_subnetv4($if_conf["ipaddr"], $if_conf["subnet"]));
                $bc_ip = ip2long(gen_subnetv4_max($if_conf["ipaddr"], $if_conf["subnet"]));
                $dhcp_start_ip = ip2long($this->config["dhcpd"][$this->id]["range"]["from"]);
                $dhcp_end_ip = ip2long($this->config["dhcpd"][$this->id]["range"]["to"]);

                # Check if this IP is the gateway address, if so set error
                if ($ip === $gw_ip) {
                    $this->errors[] = APIResponse\get(2041);
                }

                # Check if this IP is the DHCP pool, if so set error
                if ($ip >= $dhcp_start_ip and $ip <= $dhcp_end_ip) {
                    $this->errors[] = APIResponse\get(2043);
                }

                # Check if this IP is outside or equal to the network and broadcast addresses
                if ($ip <= $net_ip or $ip >= $bc_ip) {
                    $this->errors[] = APIResponse\get(2042);
                }

                # Loop through existing mappings and ensure this MAC doesn't exist already
                foreach ($this->config["dhcpd"][$this->id]["staticmap"] as $map) {
                    if ($map["ipaddr"] === $this->initial_data["ipaddr"]) {
                        $this->errors[] = APIResponse\get(2039);
                    }
                }
                $this->validated_data["ipaddr"] = $this->initial_data["ipaddr"];
            } else {
                $this->errors[] = APIResponse\get(2038);
            }
        } else {
            $this->errors[] = APIResponse\get(2037);
        }

        # Check for our optional 'cid' payload value
        if (isset($this->initial_data["cid"])) {
            $this->validated_data["cid"] = strval($this->initial_data["cid"]);
        }

        # Check for our optional 'descr' payload value
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = strval($this->initial_data["descr"]);
        }

        # Check for our optional 'hostname' value
        if (isset($this->initial_data["hostname"])) {
            # Ensure domain is a hostname or default
            if (is_hostname($this->initial_data["hostname"]) or $this->initial_data["hostname"] === "") {
                $this->validated_data["hostname"] = $this->initial_data["hostname"];
            } else {
                $this->errors[] = APIResponse\get(2040);
            }
        }

        # Check for our optional 'domain' value
        if (isset($this->initial_data["domain"])) {
            # Ensure domain is a hostname or default
            if (is_hostname($this->initial_data["domain"]) or $this->initial_data["domain"] === "") {
                $this->validated_data["domain"] = $this->initial_data["domain"];
            } else {
                $this->errors[] = APIResponse\get(2030);
            }
        }

        # Check for our optional 'domainsearchlist' value
        if (isset($this->initial_data["domainsearchlist"])) {
            # Ensure value is an array
            if (!is_array($this->initial_data["domainsearchlist"])) {
                $this->initial_data["domainsearchlist"] = [$this->initial_data["domainsearchlist"]];
            }
            # Loop through each domainsearrchlist value
            $domains = [];
            foreach ($this->initial_data["domainsearchlist"] as $domain) {
                # Ensure domain is a hostname or default
                if (is_hostname($domain) or $domain === "") {
                    $domains[] = $domain;
                } else {
                    $this->errors[] = APIResponse\get(2031);
                }
            }

            # If we have validated values, format them to the XML supported format. Otherwise assume default.
            if (count($domains) > 0) {
                $this->validated_data["domainsearchlist"] = implode(";", $domains);
            } else {
                $this->validated_data["domainsearchlist"] = "";
            }
        }

        # Check for our optional 'gateway' payload value
        if (isset($this->initial_data["gateway"])) {
            # Ensure our gateway is an IPv4 address
            if (is_ipaddrv4($this->initial_data["gateway"])) {
                $ip = ip2long($this->initial_data["gateway"]);
                $start_ip = ip2long(gen_subnetv4($if_conf["ipaddr"], $if_conf["subnet"]));
                $end_ip = ip2long(gen_subnetv4_max($if_conf["ipaddr"], $if_conf["subnet"]));
                # Ensure our gateway is within our available range
                if ($ip > $start_ip and $ip < $end_ip) {
                    $this->validated_data["gateway"] = $this->initial_data["gateway"];
                } else {
                    $this->errors[] = APIResponse\get(2029);
                }
            # Allow empty string to revert gateway to default
            } elseif ($this->initial_data["gateway"] === "") {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            } else {
                $this->errors[] = APIResponse\get(2028);
            }
        }

        # Check for our optional 'dnsserver' payload value
        if (isset($this->initial_data["dnsserver"])) {
            # Ensure this value is an array
            if (!is_array($this->initial_data["dnsserver"])) {
                $this->initial_data["dnsserver"] = [$this->initial_data["dnsserver"]];
            }

            # Ensure we have 4 or less DNS servers
            if (count($this->initial_data["dnsserver"]) <= 4) {
                $this->validated_data["dnsserver"] = [];
                # Loop through each requested DNS server and ensure it is valid
                foreach ($this->initial_data["dnsserver"] as $server) {
                    # Ensure value is IPv4 address
                    if (is_ipaddrv4($server)) {
                        $this->validated_data["dnsserver"][] = $server;
                    } else {
                        $this->errors[] = APIResponse\get(2027);
                    }
                }
            } else {
                $this->errors[] = APIResponse\get(2026);
            }
        }

        # Check for our optional 'arp_table_static_entry' payload value
        if ($this->initial_data["arp_table_static_entry"] === true) {
            $this->validated_data["arp_table_static_entry"] = true;
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
