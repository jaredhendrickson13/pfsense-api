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


class APIServicesDHCPdUpdate extends APIModel {
    private $__if_conf;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dhcpserver"];
        $this->change_note = "Modified DHCP server via API";
        $this->__if_conf = [];
    }

    public function action() {
        $this->set_config("dhcpd/{$this->id}", $this->validated_data);
        $this->write_config();
        $this->reload_dhcpd();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_interface();
        $this->__validate_enable();
        $this->__validate_range_from();
        $this->__validate_range_to();
        $this->__validate_domain();
        $this->__validate_mac_allow();
        $this->__validate_mac_deny();
        $this->__validate_domainsearchlist();
        $this->__validate_defaultleasetime();
        $this->__validate_maxleasetime();
        $this->__validate_gateway();
        $this->__validate_dnsserver();
        $this->__validate_staticarp();
        $this->__validate_ignorebootp();
        $this->__validate_denyunknown();
        $this->__validate_numberoptions();
    }

    private function __validate_interface() {
        # Check for our required 'interface' payload value
        if (isset($this->initial_data["interface"])) {
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            # Check that interface exists
            if (is_string($this->initial_data["interface"])) {
                $this->__if_conf = $this->get_config("interfaces/{$this->initial_data["interface"]}", []);
                # Check that interface hosts a static network with multiple addresses
                if (isset($this->__if_conf["enable"]) and is_ipaddrv4($this->__if_conf["ipaddr"]) and $this->__if_conf["subnet"] <= 31) {
                    $this->id = $this->initial_data["interface"];
                    $this->__init_config($this->id);
                    $this->validated_data = $this->get_config("dhcpd/{$this->id}", []);
                } else {
                    $this->errors[] = APIResponse\get(2019);
                }
            } else {
                $this->errors[] = APIResponse\get(2018);
            }
        } else {
            $this->errors[] = APIResponse\get(2017);
        }
    }

    private function __validate_enable() {
        # Check for our optional 'enabled' payload value
        if ($this->initial_data["enable"] === true) {
            $this->validated_data["enable"] = true;
        } elseif ($this->initial_data["enable"] === false) {
            $this->validated_data["enable"] = false;
        }
    }

    private function __validate_range_from() {
        # Check for our optional 'range_from' payload value
        if (isset($this->initial_data["range_from"])) {
            # Ensure our range_from is an IPv4 address
            if (is_ipaddrv4($this->initial_data["range_from"])) {
                # Format this interface's CIDR
                $if_cidr = $this->__if_conf["ipaddr"]."/".$this->__if_conf["subnet"];

                # Ensure our range_from is within our available range
                if (APITools\is_ipv4_in_cidr($this->initial_data["range_from"], $if_cidr)) {
                    $this->validated_data["range"]["from"] = $this->initial_data["range_from"];
                } else {
                    $this->errors[] = APIResponse\get(2022);
                }
            } else {
                $this->errors[] = APIResponse\get(2021);
            }
        } elseif (!isset($this->validated_data["range"]["from"])) {
            $this->errors[] = APIResponse\get(2020);
        }
    }

    private function __validate_range_to() {
        # Check for our optional 'range_to' payload value
        if (isset($this->initial_data["range_to"])) {
            # Ensure our range_to is an IPv4 address
            if (is_ipaddrv4($this->initial_data["range_to"])) {
                # Format this interface's CIDR
                $if_cidr = $this->__if_conf["ipaddr"]."/".$this->__if_conf["subnet"];

                # Ensure our range_to is within our available range
                if (APITools\is_ipv4_in_cidr($this->initial_data["range_to"], $if_cidr)) {
                    $this->validated_data["range"]["to"] = $this->initial_data["range_to"];
                } else {
                    $this->errors[] = APIResponse\get(2025);
                }
            } else {
                $this->errors[] = APIResponse\get(2024);
            }
        } elseif (!isset($this->validated_data["range"]["to"])) {
            $this->errors[] = APIResponse\get(2023);
        }
    }

    private function __validate_domain() {
        # Check for our optional 'domain' value
        if (isset($this->initial_data["domain"])) {
            # Ensure domain is a hostname or default
            if (is_hostname($this->initial_data["domain"]) or $this->initial_data["domain"] === "") {
                $this->validated_data["domain"] = $this->initial_data["domain"];
            } else {
                $this->errors[] = APIResponse\get(2030);
            }
        }
    }

    private function __validate_mac_allow() {
        # Check for our optional 'mac_allow' value
        if (isset($this->initial_data["mac_allow"])) {
            # Ensure value is an array
            if (!is_array($this->initial_data["mac_allow"])) {
                $this->initial_data["mac_allow"] = [$this->initial_data["mac_allow"]];
            }
            # Loop through each mac_allow value
            $macs = [];
            foreach ($this->initial_data["mac_allow"] as $mac) {
                # Ensure entry is partial mac or default
                if (is_macaddr($mac, true) or $mac === "") {
                    $macs[] = $mac;
                } else {
                    $this->errors[] = APIResponse\get(2032);
                }
            }

            # If we have validated values, format them to the XML supported format. Otherwise assume default.
            if (count($macs) > 0) {
                $this->validated_data["mac_allow"] = implode(",", $macs);
            } else {
                $this->validated_data["mac_allow"] = "";
            }
        }
    }

    private function __validate_mac_deny() {
        # Check for our optional 'mac_deny' value
        if (isset($this->initial_data["mac_deny"])) {
            # Ensure value is an array
            if (!is_array($this->initial_data["mac_deny"])) {
                $this->initial_data["mac_deny"] = [$this->initial_data["mac_deny"]];
            }
            # Loop through each mac_deny value
            $macs = [];
            foreach ($this->initial_data["mac_deny"] as $mac) {
                # Ensure entry is partial mac or default
                if (is_macaddr($mac, true) or $mac === "") {
                    $macs[] = $mac;
                } else {
                    $this->errors[] = APIResponse\get(2033);
                }
            }

            # If we have validated values, format them to the XML supported format. Otherwise assume default.
            if (count($macs) > 0) {
                $this->validated_data["mac_deny"] = implode(",", $macs);
            } else {
                $this->validated_data["mac_deny"] = "";
            }
        }
    }

    private function __validate_domainsearchlist() {
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
    }

    private function __validate_defaultleasetime() {
        # Variables
        $max_time = $this->validated_data["maxleasetime"];

        # Check for our optional defaultleasetime field
        if (isset($this->initial_data["defaultleasetime"])) {
            # Allow clients to unset this field with an empty string
            if ($this->initial_data["defaultleasetime"] === "") {
                unset($this->validated_data["defaultleasetime"]);
                return;
            }

            # Return an error if the default lease time is less than 60
            if (intval($this->initial_data["defaultleasetime"]) < 60) {
                $this->errors[] = APIResponse\get(2080);
            }
            # Return error if the default lease time is greater than the maximum lease time
            elseif (!empty($max_time) and intval($this->initial_data["defaultleasetime"]) >= intval($max_time)) {
                $this->errors[] = APIResponse\get(2081);
            }
            # When no error criteria was met above, the data is valid
            else {
                $this->validated_data["defaultleasetime"] = intval($this->initial_data["defaultleasetime"]);
            }
        }
    }

    private function __validate_maxleasetime() {
        # Variables
        $def_time = $this->validated_data["defaultleasetime"];

        # Check for our optional maxleasetime field
        if (isset($this->initial_data["maxleasetime"])) {
            # Allow clients to unset this field with an empty string
            if ($this->initial_data["maxleasetime"] === "") {
                unset($this->validated_data["maxleasetime"]);
                return;
            }

            # Return an error if the max lease time is less than 60
            if (intval($this->initial_data["maxleasetime"]) < 60) {
                $this->errors[] = APIResponse\get(2082);
            }
            # Return error if the default lease time is greater than the maximum lease time
            elseif (!empty($def_time) and intval($this->initial_data["maxleasetime"]) <= intval($def_time)) {
                $this->errors[] = APIResponse\get(2081);
            }
            # When no error criteria was met above, the data is valid
            else {
                $this->validated_data["maxleasetime"] = intval($this->initial_data["maxleasetime"]);
            }
        }
    }

    private function __validate_gateway() {
        # Check for our optional 'gateway' payload value
        if (isset($this->initial_data["gateway"])) {
            # Ensure our gateway is an IPv4 address
            if (is_ipaddrv4($this->initial_data["gateway"])) {
                # Format this interface's CIDR
                $if_cidr = $this->__if_conf["ipaddr"]."/".$this->__if_conf["subnet"];

                # Ensure our gateway is within our available range
                if (APITools\is_ipv4_in_cidr($this->initial_data["gateway"], $if_cidr)) {
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
    }

    private function __validate_dnsserver() {
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
    }

    private function __validate_staticarp() {
        # TODO: issues with DHCP reloads and this field have been reported that may be fixed by calling
        # TODO: reload_interfaces() which does not seem to be present within pfSense itself (#129). Current workaround
        # TODO: is to call rc.reload_interfaces after interacting with this field until a root cause can be verified.
        # Check for our optional 'staticarp' payload value
        if ($this->initial_data["staticarp"] === true) {
            $this->validated_data["staticarp"] = true;
        } elseif ($this->initial_data["staticarp"] === false) {
            $this->validated_data["staticarp"] = false;
        }
    }

    private function __validate_ignorebootp() {
        # Check for our optional 'ignorebootp' payload value
        if ($this->initial_data["ignorebootp"] === true) {
            $this->validated_data["ignorebootp"] = true;
        } elseif ($this->initial_data["ignorebootp"] === false) {
            $this->validated_data["ignorebootp"] = false;
        }
    }

    private function __validate_denyunknown() {
        # Check for our optional 'denyunknown' payload value
        if ($this->initial_data["denyunknown"] === true) {
            $this->validated_data["denyunknown"] = true;
        } elseif ($this->initial_data["denyunknown"] === false) {
            $this->validated_data["denyunknown"] = false;
        }
    }

    # This method invokes a nested API call to the APIServicesDHCPdOptionsCreate model
    private function __validate_numberoptions() {
        # Check for our optional numberoptions field
        if (isset($this->initial_data["numberoptions"])) {
            # Discard the nested 'item' array if present
            if (array_key_exists("item", $this->initial_data["numberoptions"])) {
                $this->initial_data["numberoptions"] = $this->initial_data["numberoptions"]["item"];
            }

            # Reset the saved number options
            $this->validated_data["numberoptions"] = [];

            # Loop through each requested number option and ensure it is valid
            foreach ($this->initial_data["numberoptions"] as $numberoption) {
                $options_model = new APIServicesDHCPdOptionsCreate();
                $options_model->initial_data = $numberoption;
                $options_model->__validate_number();
                $options_model->__validate_type();
                $options_model->__validate_value();

                # Add any errors found in our nested model to the current model
                $this->errors = $this->errors + $options_model->errors;

                # Only save our data as validated if no errors occurred
                if (!$options_model->errors) {
                    $this->validated_data["numberoptions"]["item"][] = $options_model->validated_data;
                }
            }
        }
    }

    # Executes functions to reload DHCP and corresponding/dependent services
    public function reload_dhcpd() {
        $retvaldhcp = 0;
        $retvaldns = 0;
        /* dnsmasq_configure calls dhcpd_configure */
        /* no need to restart dhcpd twice */
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

    private function __init_config($interface) {
        # Initialize the default interface DHCP configuration if it does not exist
        if (!$this->is_config_enabled("dhcpd", $interface)) {
            $this->set_config("dhcpd/{$interface}", [
                "range" => ["from" => "", "to" => ""],
                "failover_peerip" => "",
                "defaultleasetime" => "",
                "maxleasetime" => "",
                "netmask" => "",
                "gateway" => "",
                "domain" => "",
                "domainsearchlist" => "",
                "ddnsdomain" => "",
                "ddnsdomainprimary" => "",
                "ddnsdomainsecondary" => "",
                "ddnsdomainkeyname" => "",
                "ddnsdomainkeyalgorithm" => "hmac-md5",
                "ddnsdomainkey" => "",
                "mac_allow" => "",
                "mac_deny" => "",
                "ddnsclientupdates" => "allow",
                "tftp" => "",
                "ldap" => "",
                "nextserver" => "",
                "filename" => "",
                "filename32" => "",
                "filename64" => "",
                "filename32arm" => "",
                "filename64arm" => "",
                "rootpath" => "",
                "numberoptions" => ""
            ]);
        }
    }
}
