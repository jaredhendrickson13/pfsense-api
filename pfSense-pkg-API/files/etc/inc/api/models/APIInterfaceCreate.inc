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


class APIInterfaceCreate extends APIModel {
    private $if_list;
    private $next_if;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-assignnetworkports"];
        $this->change_note = "Added interface via API";
        $this->if_list = APITools\get_all_avail_interfaces();
        $this->next_if = APITools\get_next_pfsense_if_id();
        $this->init_config('interfaces');
    }

    public function action() {
        # Add the new interface to our configuration and write the pending changes for application
        $this->config["interfaces"][$this->next_if] = $this->validated_data;
        $this->write_config();
        APIInterfaceApplyCreate::serialize_changes($this->next_if, $this->validated_data);

        # Only apply the interface configuration if client requested
        if ($this->initial_data["apply"] === true) {
            APIInterfaceApplyCreate::apply();
        }
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_if() {
        if (isset($this->initial_data["if"])) {
            $this->validated_data["if"] = trim($this->initial_data["if"]);
            // Check that our interface exists and is not in use
            if (!array_key_exists($this->initial_data["if"], $this->if_list)) {
                $this->errors[] = APIResponse\get(3000);
            } elseif (isset($this->if_list[$this->initial_data["if"]]["in_use"])) {
                $this->errors[] = APIResponse\get(3001);
            }
            $this->validated_data["if"] = $this->initial_data["if"];
        } else {
            $this->errors[] = APIResponse\get(3002);
        }
    }

    private function __validate_enable() {
        // Check for our enable value
        if ($this->initial_data["enable"] === true) {
            $this->validated_data["enable"] = "";
        }
    }

    private function __validate_spoofmac() {
        # Check for our MAC address value
        if (isset($this->initial_data["spoofmac"])) {
            # Check if MAC address is valid
            if (is_macaddr($this->initial_data["spoofmac"])) {
                $this->validated_data["spoofmac"] = $this->initial_data["spoofmac"];
            }
            # Check if value is empty string (default value)
            elseif ($this->initial_data["spoofmac"] === "") {
                $this->validated_data["spoofmac"] = "";
            }
            # Otherwise return an invalid error
            else {
                $this->errors[] = APIResponse\get(3003);
            }
        }
        # Otherwise assume default
        else {
            $this->validated_data["spoofmac"] = "";
        }
    }

    private function __validate_mtu() {
        # Check for our MTU value
        if (isset($this->initial_data["mtu"])) {
            # Ensure value is an integer
            $this->initial_data["mtu"] = intval($this->initial_data["mtu"]);

            # Check if MTU is within range
            if (1280 > $this->initial_data["mtu"] or $this->initial_data["mtu"] > 8192) {
                $this->errors[] = APIResponse\get(3004);
                return;
            }
            # Validate VLAN specific constraints
            if ($this->if_list[$this->validated_data["if"]]["is_vlan"]) {
                # Fetch the parent interface configuration
                $parent_if_id = APITools\get_pfsense_if_id($this->if_list[$this->validated_data["if"]]["if"]);
                $parent_if_conf = $this->config["interfaces"][$parent_if_id];

                # Ensure MTU is not lower than the parent interface
                if ($this->initial_data["mtu"] > $parent_if_conf["mtu"]) {
                    $this->errors[] = APIResponse\get(3006);
                    return;
                }
            }

            $this->validated_data["mtu"] = $this->initial_data["mtu"];
        }
    }

    private function __validate_mss() {
        // Check for our MSS value
        if (isset($this->initial_data["mss"])) {
            $this->initial_data["mss"] = intval($this->initial_data["mss"]);
            // Check if MSS is within range
            if (576 > $this->initial_data["mss"] or $this->initial_data["mss"] > 65535) {
                $this->errors[] = APIResponse\get(3005);

            } else {
                $this->validated_data["mss"] = $this->initial_data["mss"];
            }
        }
    }
    
    private function __validate_media() {
        // Check for our SPEED/DUPLEX value
        if (isset($this->initial_data["media"])) {
            $avail_media = APITools\get_if_media_options($this->validated_data["if"], true);
            $media_found = false;
            // Loop each of our media options and see if our input matches
            foreach ($avail_media as $mopt) {
                if ($this->initial_data["media"] === $mopt) {
                    $media_found = true;
                    $mopt_list = explode(" ", $mopt);
                    $this->validated_data["media"] = $mopt_list[0];
                    $this->validated_data["mediaopt"] = $mopt_list[1];
                    break;
                }
            }
            // If we did not find a match return error
            if (!$media_found) {
                $this->errors[] = APIResponse\get(3007);
            }
        }
    }

    private function __validate_descr() {
        # Check for our description value
        if (isset($this->initial_data["descr"])) {
            $this->initial_data["descr"] = APITools\sanitize_str($this->initial_data["descr"]);
            # Ensure this interface description does not contain reserved keywords
            if (substr($this->initial_data['descr'], 0, 4) === "pkg_") {
                $this->errors[] = APIResponse\get(3059);
            }
            # Ensure this interface is not named the same as an alias
            elseif (is_alias($this->initial_data["descr"])) {
                $this->errors[] = APIResponse\get(3060);
            }
            # Ensure this interface is name named entirely numerical
            elseif (is_numeric($this->initial_data["descr"])) {
                $this->errors[] = APIResponse\get(3061);
            }
            # Ensure this interface is not named the same as an interface group
            elseif (APITools\is_ifgroup($this->initial_data["descr"])) {
                $this->errors[] = APIResponse\get(3062);
            }
            # Ensure this interface description is not already used
            elseif (APITools\get_pfsense_if_id($this->initial_data["descr"])) {
                $this->errors[] = APIResponse\get(3008);
            }
            # Otherwise, this description looks to be valid. Update the value
            else {
                $this->validated_data["descr"] = $this->initial_data["descr"];
            }
        } else {
            $this->validated_data["descr"] = strtoupper($this->next_if);
        }
    }

    private function __validate_blockpriv() {
        // Check for our block private IP value
        if ($this->initial_data["blockpriv"] === true) {
            $this->validated_data["blockpriv"] = "";
        }
    }

    private function __validate_blockbogons() {
        // Check for our block bogon value
        if ($this->initial_data["blockbogons"] === true) {
            $this->validated_data["blockbogons"] = "";
        }
    }

    private function __validate_type_staticv4_ipaddr() {
        // Check if our IP is set
        if (isset($this->initial_data["ipaddr"])) {
            // Check if IP address is valid
            if (!is_ipaddrv4($this->initial_data["ipaddr"])) {
                $this->errors[] = APIResponse\get(3010);
            } elseif (APITools\is_ip_in_use($this->initial_data["ipaddr"])) {
                $this->errors[] = APIResponse\get(3009);
            } else {
                $this->validated_data["ipaddr"] = $this->initial_data["ipaddr"];
            }
        } else {
            $this->errors[] = APIResponse\get(3011);
        }
    }

    private function __validate_type_staticv4_subnet() {
        // Check if our subnet is valid
        if (isset($this->initial_data["subnet"])) {
            $subnet = strval($this->initial_data["subnet"]);
            // Check if our subnet is within range
            if (!is_subnet($this->validated_data["ipaddr"]."/".$subnet)) {
                $this->errors[] = APIResponse\get(3012);
            } else {
                $this->validated_data["subnet"] = $subnet;
            }
        } else {
            // Update our message if we did not already encounter an error
            $this->errors[] = APIResponse\get(3013);
        }
    }
    
    private function __validate_type_staticv4_gateway() {
        // Check if user specified a network gateway, if so check if it's valid
        if (isset($this->initial_data["gateway"])) {
            // Check if this gateway exists
            if (!APITools\is_gateway($this->initial_data["gateway"])) {
                $this->errors[] = APIResponse\get(3014);
            } else {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            }
        }
    }
    
    private function __validate_type_staticv4() {
        $this->__validate_type_staticv4_ipaddr();
        $this->__validate_type_staticv4_subnet();
        $this->__validate_type_staticv4_gateway();
    }
    
    private function __validate_type_dhcp_dhcphostname() {
        // Check if we have a dhcphostname value
        if (isset($this->initial_data["dhcphostname"])) {
            $this->validated_data["dhcphostname"] = strval($this->initial_data["dhcphostname"]);
        }
    }

    private function __validate_type_dhcp_alias_address() {
        // Check if we have a alias-address value
        if (isset($this->initial_data["alias-address"])) {
            if (is_ipaddrv4($this->initial_data["alias-address"])) {
                $this->validated_data["alias-address"] = strval($this->initial_data["alias-address"]);
                if (isset($this->initial_data["alias-subnet"])) {
                    $dhcpaliasnet = strval($this->initial_data["alias-subnet"]);
                    if (is_subnet($this->validated_data["alias-address"]."/".$dhcpaliasnet)) {
                        $this->validated_data["alias-subnet"] = $dhcpaliasnet;
                    } else {
                        $this->errors[] = APIResponse\get(3015);
                    }
                } else {
                    $this->validated_data["alias-subnet"] = 32;
                }
            } else {
                $this->errors[] = APIResponse\get(3015);
            }
        }
    }
    
    private function __validate_type_dhcp_dhcprejectfrom() {
        // Check if we have a dhcprejectfrom value
        if (isset($this->initial_data["dhcprejectfrom"])) {
            // Ensure dhcpreject from is formatted as an array
            if (is_string($this->initial_data["dhcprejectfrom"])) {
                $this->initial_data["dhcprejectfrom"] = explode(",", $this->initial_data["dhcprejectfrom"]);
            }

            // Loop through our reject list and ensure values are valid
            foreach ($this->initial_data["dhcprejectfrom"] as $ra) {
                if (!is_ipaddrv4($ra)) {
                    $bad_reject = true;
                    break;
                }
            }

            // Convert our list to comma separated string
            $this->initial_data["dhcprejectfrom"] = implode(",", $this->initial_data["dhcprejectfrom"]);

            // Check for bad IPs
            if ($bad_reject) {
                $this->errors[] = APIResponse\get(3016);
            } else {
                $this->validated_data["dhcprejectfrom"] = $this->initial_data["dhcprejectfrom"];
            }
        }
    }

    private function __validate_type_dhcp_protocol_timing() {
        // Check for our DHCP protocol timing
        $timing_protocols = array(
            "adv_dhcp_pt_timeout" => ["keyword" => "timeout", "return" => 3017, "min" => 1],
            "adv_dhcp_pt_retry" => ["keyword" => "retry", "return" => 3018, "min" => 1],
            "adv_dhcp_pt_select_timeout" => ["keyword" => "select timeout", "return" => 3019, "min" => 0],
            "adv_dhcp_pt_reboot" => ["keyword" => "reboot", "return" => 3020, "min" => 1],
            "adv_dhcp_pt_backoff_cutoff" => ["keyword" => "backoff cutoff", "return" => 3021, "min" => 1],
            "adv_dhcp_pt_initial_interval" => ["keyword" => "initial interval", "return" => 3022, "min" => 1],
        );
        // Loop through each timing attribute and see if it's valid
        foreach ($timing_protocols as $tp => $data) {
            if (isset($this->initial_data[$tp])) {
                // Check that value is in range
                $dhcp_attr = intval($this->initial_data[$tp]);
                if ($dhcp_attr >= $data["min"]) {
                    $this->validated_data[$tp] = $dhcp_attr;
                    $this->validated_data["adv_dhcp_pt_values"] = "SavedCfg";
                } else {
                    $this->errors[] = APIResponse\get($data["return"]);
                }
            }
        }
    }

    private function __validate_type_dhcp_adv_dhcp_config_advanced() {
        // Check for advance DHCP config
        if (isset($this->initial_data["adv_dhcp_config_advanced"])) {
            $this->validated_data["adv_dhcp_config_advanced"] = "yes";
            // Check for our DHCP options
            $dhcp_opts = array(
                "adv_dhcp_send_options",
                "adv_dhcp_request_options",
                "adv_dhcp_required_options",
                "adv_dhcp_option_modifiers"
            );
            foreach ($dhcp_opts as $do) {
                // Check if option exists
                if (isset($this->initial_data[$do])) {
                    $this->validated_data[$do] = strval($this->initial_data[$do]);
                }
            }
        }
    }

    private function __validate_type_dhcp_adv_dhcp_config_file_override() {
        // Check for DHCP configuration file override option
        if (isset($this->initial_data["adv_dhcp_config_file_override"])) {
            $this->validated_data["adv_dhcp_config_file_override"] = "";
            // Check if a file path was given
            if (isset($this->initial_data["adv_dhcp_config_file_override_path"])) {
                $dhcp_conf_file = $this->initial_data["adv_dhcp_config_file_override_path"];
                // Check that our conf file exists
                if (is_file($dhcp_conf_file)) {
                    $this->validated_data["adv_dhcp_config_file_override"] = $dhcp_conf_file;
                } else {
                    $this->errors[] = APIResponse\get(3023);
                }
            }
        }
    }

    private function __validate_type_dhcp_dhcpvlanenable() {
        // Check for DHCP VLAN priority
        $dhcp_vlan_prios = array(
            0 => "bk",
            1 => "be",
            2 => "ee",
            3 => "ca",
            4 => "vi",
            5 => "vo",
            6 => "ic",
            7 => "nc"
        );
        if (isset($this->initial_data["dhcpvlanenable"])) {
            $this->validated_data["dhcpvlanenable"] = "";
            if (isset($this->initial_data["dhcpcvpt"])) {
                $vlan_prio = strtolower($this->initial_data["dhcpcvpt"]);
                // Check if VLAN priority was provided as number
                if (is_numeric($vlan_prio) and array_key_exists(intval($vlan_prio), $dhcp_vlan_prios)) {
                    $this->validated_data["dhcpcvpt"] = $dhcp_vlan_prios[intval($vlan_prio)];
                } else {
                    // Loop through our priorities and see if value matches
                    foreach ($dhcp_vlan_prios as $dvp => $dvpval) {
                        if ($vlan_prio === $dvpval) {
                            $vlan_prio_found = true;
                            $this->validated_data["dhcpcvpt"] = $dvpval;
                            break;
                        }
                    }
                    // Check that we found a value in our loop
                    if (!$vlan_prio_found) {
                        $this->errors[] = APIResponse\get(3024);
                    }
                }
            }
        }
    }
    
    private function __validate_type_dhcp() {
        $this->validated_data["ipaddr"] = $this->initial_data["type"];
        $this->__validate_type_dhcp_dhcphostname();
        $this->__validate_type_dhcp_alias_address();
        $this->__validate_type_dhcp_dhcprejectfrom();
        $this->__validate_type_dhcp_protocol_timing();
        $this->__validate_type_dhcp_adv_dhcp_config_advanced();
        $this->__validate_type_dhcp_adv_dhcp_config_file_override();
        $this->__validate_type_dhcp_dhcpvlanenable();
    }

    private function __validate_type6_staticv6_ipaddrv6() {
        // Check if our IP is set
        if (isset($this->initial_data["ipaddrv6"])) {
            // Check if IP address is valid
            if (!is_ipaddrv6($this->initial_data["ipaddrv6"])) {
                $this->errors[] = APIResponse\get(3026);
            } elseif (APITools\is_ip_in_use($this->initial_data["ipaddrv6"])) {
                $this->errors[] = APIResponse\get(3027);
            } else {
                $this->validated_data["ipaddrv6"] = $this->initial_data["ipaddrv6"];
            }
        } else {
            $this->errors[] = APIResponse\get(3028);
        }
    }

    private function __validate_type6_staticv6_subnetv6() {
        // Check if our subnet is valid
        if (isset($this->initial_data["subnetv6"])) {
            $subnetv6 = strval($this->initial_data["subnetv6"]);
            // Check if our subnet is within range
            if (!is_subnet($this->validated_data["ipaddrv6"] . "/" . $subnetv6)) {
                $this->errors[] = APIResponse\get(3029);
            } else {
                $this->validated_data["subnetv6"] = $subnetv6;
            }
        } else {
            $this->errors[] = APIResponse\get(3030);
        }
    }

    private function __validate_type6_staticv6_gatewayv6() {
        // Check if user specified a network gateway, if so check if it's valid
        if (isset($this->initial_data["gatewayv6"])) {
            $gatewayv6 = $this->initial_data["gatewayv6"];
            // Check if this gateway exists
            if (!APITools\is_gateway($gatewayv6)) {
                $this->errors[] = APIResponse\get(3031);
            } else {
                $this->validated_data["gatewayv6"] = $gatewayv6;
            }
        }
    }

    private function __validate_type6_staticv6_ipv6usev4iface() {
        // Check if user set ipv6usev4iface value
        if ($this->initial_data["ipv6usev4iface"] === true) {
            $this->validated_data["ipv6usev4iface"] = "";
        }
    }
    
    private function __validate_type6_staticv6() {
        $this->__validate_type6_staticv6_ipaddrv6();
        $this->__validate_type6_staticv6_subnetv6();
        $this->__validate_type6_staticv6_gatewayv6();
        $this->__validate_type6_staticv6_ipv6usev4iface();
    }

    private function __validate_type6_dhcp6_ipv6usev4iface() {
        // Check if user set ipv6usev4iface value
        if ($this->initial_data["ipv6usev4iface"] === true) {
            $this->validated_data["ipv6usev4iface"] = "";
        }
    }

    private function __validate_type6_dhcp6_dhcp6prefixonly() {
        // Check if user set dhcp6prefixonly value
        if ($this->initial_data["dhcp6prefixonly"] === true) {
            $this->validated_data["dhcp6prefixonly"] = "";
        }
    }

    private function __validate_type6_dhcp6_dhcp6_ia_pd_send_hint() {
        // Check if user set dhcp6-ia-pd-send-hint value
        if ($this->initial_data["dhcp6-ia-pd-send-hint"] === true) {
            $this->validated_data["dhcp6-ia-pd-send-hint"] = "";
        }
    }

    private function __validate_type6_dhcp6_dhcp6debug() {
        // Check if user set dhcp6debug value
        if ($this->initial_data["dhcp6debug"] === true) {
            $this->validated_data["dhcp6debug"] = "";
        }
    }

    private function __validate_type6_dhcp6_dhcp6withoutra() {
        // Check if user set dhcp6withoutra value
        if ($this->initial_data["dhcp6withoutra"] === true) {
            $this->validated_data["dhcp6withoutra"] = "";
        }
    }

    private function __validate_type6_dhcp6_dhcp6norelease() {
        // Check if user set dhcp6norelease value
        if ($this->initial_data["dhcp6norelease"] === true) {
            $this->validated_data["dhcp6norelease"] = "";
        }
    }

    private function __validate_type6_dhcp6_dhcp6vlanenable() {
        // Check if user set dhcp6vlanenable value
        if ($this->initial_data["dhcp6vlanenable"] === true) {
            $this->validated_data["dhcp6vlanenable"] = "";
        }
    }

    private function __validate_type6_dhcp6_dhcp6_ia_pd_len() {
        // Check if user set dhcp6-ia-pd-len value
        if (isset($this->initial_data["dhcp6-ia-pd-len"])) {
            // Set array of allowed prefix delegation sizes and their config translation
            $dhcp6_del_size = intval($this->initial_data["dhcp6-ia-pd-len"]);
            $allowed_size = array(
                64 => 0,
                63 => 1,
                62 => 2,
                61 => 3,
                60 => 4,
                59 => 5,
                56 => 8,
                52 => 12,
                48 => 16
            );
            if (array_key_exists($dhcp6_del_size, $allowed_size)) {
                $this->validated_data["dhcp6-ia-pd-len"] = $allowed_size[$dhcp6_del_size];
            } else {
                $this->errors[] = APIResponse\get(3032);
            }
        }
    }

    private function __validate_type6_dhcp6_dhcp6cvpt() {
        // Check for DHCP VLAN priority
        $dhcp_vlan_prios = array(
            0 => "bk",
            1 => "be",
            2 => "ee",
            3 => "ca",
            4 => "vi",
            5 => "vo",
            6 => "ic",
            7 => "nc"
        );
        if (isset($this->validated_data["dhcp6vlanenable"])) {
            if (isset($this->initial_data["dhcp6cvpt"])) {
                $vlan_prio = strtolower($this->initial_data["dhcp6cvpt"]);
                // Check if VLAN priority was provided as number
                if (is_numeric($vlan_prio) and array_key_exists(intval($vlan_prio), $dhcp_vlan_prios)) {
                    $this->validated_data["dhcp6cvpt"] = $dhcp_vlan_prios[intval($vlan_prio)];
                } else {
                    // Loop through our priorities and see if value matches
                    foreach ($dhcp_vlan_prios as $dvp => $dvpval) {
                        if ($vlan_prio === $dvpval) {
                            $vlan_prio_found = true;
                            $this->validated_data["dhcp6cvpt"] = $dvpval;
                            break;
                        }
                    }
                    // Check that we found a value in our loop
                    if (!$vlan_prio_found) {
                        $this->errors[] = APIResponse\get(3033);
                    }
                }
            }
        }
    }

    private function __validate_type6_dhcp6_adv_dhcp6_config_file_override() {
        // Check for DHCP configuration file override option
        if (isset($this->initial_data["adv_dhcp6_config_file_override"])) {
            $this->validated_data["adv_dhcp6_config_file_override"] = "";
            // Check if a file path was given
            if (isset($this->initial_data["adv_dhcp6_config_file_override_path"])) {
                $dhcp_conf_file = $this->initial_data["adv_dhcp6_config_file_override_path"];
                // Check that our conf file exists
                if (is_file($dhcp_conf_file)) {
                    $this->validated_data["adv_dhcp6_config_file_override_path"] = $dhcp_conf_file;
                } else {
                    $this->errors[] = APIResponse\get(3034);
                }
            }
        }
    }

    private function __validate_type6_dhcp6() {
        $this->validated_data["ipaddrv6"] = "dhcp6";
        $this->__validate_type6_dhcp6_ipv6usev4iface();
        $this->__validate_type6_dhcp6_dhcp6prefixonly();
        $this->__validate_type6_dhcp6_dhcp6_ia_pd_send_hint();
        $this->__validate_type6_dhcp6_dhcp6debug();
        $this->__validate_type6_dhcp6_dhcp6withoutra();
        $this->__validate_type6_dhcp6_dhcp6norelease();
        $this->__validate_type6_dhcp6_dhcp6vlanenable();
        $this->__validate_type6_dhcp6_dhcp6_ia_pd_len();
        $this->__validate_type6_dhcp6_dhcp6cvpt();
        $this->__validate_type6_dhcp6_adv_dhcp6_config_file_override();
    }

    private function __validate_type6_slaac() {
        $this->validated_data["ipaddrv6"] = "slaac";
    }

    private function __validate_type6_6rd_gateway_6rd() {
        // Check for a 6RD border relay
        if (isset($this->initial_data["gateway-6rd"])) {
            $gw6rd = $this->initial_data["gateway-6rd"];
            // Check that our gateway is a valid IPv4 address
            if (is_ipaddrv4($gw6rd)) {
                $this->validated_data["gateway-6rd"] = $this->initial_data["gateway-6rd"];
            } else {
                $this->errors[] = APIResponse\get(3035);
            }
        } else {
            $this->errors[] = APIResponse\get(3036);
        }
    }

    private function __validate_type6_6rd_prefix_6rd() {
        // Check for a 6RD prefix
        if (isset($this->initial_data["prefix-6rd"])) {
            $this->validated_data["prefix-6rd"] = $this->initial_data["prefix-6rd"];
        }
    }

    private function __validate_type6_6rd_prefix_6rd_v4plen() {
        // Check for a 6RD prefix length
        if (isset($this->initial_data["prefix-6rd-v4plen"])) {
            $prefix_len = $this->initial_data["prefix-6rd-v4plen"];
            // Check if our prefix length is within range
            if (is_numeric($prefix_len) and (0 <= intval($prefix_len) and intval($prefix_len) <= 32)) {
                $this->validated_data["prefix-6rd-v4plen"] = $this->initial_data["prefix-6rd-v4plen"];
            } else {
                $this->errors[] = APIResponse\get(3037);
            }
        } else {
            $this->validated_data["prefix-6rd-v4plen"] = 0;
        }
    }

    private function __validate_type6_6rd() {
        $this->validated_data["ipaddrv6"] = "6rd";
        $this->__validate_type6_6rd_gateway_6rd();
        $this->__validate_type6_6rd_prefix_6rd();
        $this->__validate_type6_6rd_prefix_6rd_v4plen();
    }

    private function __validate_type6_track6_track6_interface() {
        // Check for track 6 interface
        if (isset($this->initial_data["track6-interface"])) {
            $track_if = $this->initial_data["track6-interface"];
            $track_if = APITools\get_pfsense_if_id($track_if);
            // Check that our gateway is a valid IPv4 address
            if (array_key_exists($track_if, APITools\get_ipv6_if_list())) {
                $this->validated_data["track6-interface"] = $this->initial_data["track6-interface"];
            } else {
                $this->errors[] = APIResponse\get(3038);
            }
        } else {
            $this->errors[] = APIResponse\get(3039);
        }
    }

    private function __validate_type6_track6_track6_prefix_id_hex() {
        // Check for track 6 prefix ID
        if (isset($this->initial_data["track6-prefix-id-hex"])) {
            $track_prefix = $this->initial_data["track6-prefix-id-hex"];
            // Check that our gateway is a valid IPv4 address
            if (is_numeric($track_prefix) and ctype_xdigit(strval($track_prefix))) {
                $this->validated_data["track6-prefix-id"] = intval($track_prefix);
            } else {
                $this->errors[] = APIResponse\get(3040);
            }
        } else {
            $this->validated_data["track6-prefix-id"] = 0;
        }
    }

    private function __validate_type6_track6() {
        $this->validated_data["ipaddrv6"] = "track6";
        $this->__validate_type6_track6_track6_interface();
        $this->__validate_type6_track6_track6_prefix_id_hex();
    }

    private function __validate_type6_6to4() {
        $this->validated_data["ipaddrv6"] = "6to4";
    }


    public function validate_payload() {
        $this->__validate_if();
        $this->__validate_enable();
        $this->__validate_spoofmac();
        $this->__validate_mtu();
        $this->__validate_mss();
        $this->__validate_media();
        $this->__validate_descr();
        $this->__validate_blockpriv();
        $this->__validate_blockbogons();

        # Check if we have an IPv4 configuration
        if (isset($this->initial_data["type"])) {
            # Validate remaining data according to type specification, return error if unmatched
            switch ($this->initial_data["type"]) {
                case "staticv4":
                    $this->__validate_type_staticv4();
                    break;
                case "dhcp":
                    $this->__validate_type_dhcp();
                    break;
                default:
                    $this->errors[] = APIResponse\get(3025);
                    break;
            }
        }

        # Check if we have an IPv6 configuration
        if (isset($this->initial_data["type6"])) {
            # Validate remaining data according to type6 specification, return error if unmatched
            switch ($this->initial_data["type6"]) {
                case "staticv6":
                    $this->__validate_type6_staticv6();
                    break;
                case "dhcp6":
                    $this->__validate_type6_dhcp6();
                    break;
                case "slaac":
                    $this->__validate_type6_slaac();
                    break;
                case "6rd":
                    $this->__validate_type6_6rd();
                    break;
                case "track6":
                    $this->__validate_type6_track6();
                    break;
                case "6to4":
                    $this->__validate_type6_6to4();
                    break;
                default:
                    $this->errors[] = APIResponse\get(3041);
                    break;
            }
        }
    }
}
