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

class APIServicesOpenVPNServerUpdate extends APIModel {
    public $original_server;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-openvpn-server"];
        $this->change_note = "Modified OpenVPN server via API";
        $this->init_config("openvpn", "openvpn-server");
    }

    public function action() {
        # Initialize expected configuration arrays
        $this->__init_config();

        # Update server to the new configuration and resync OpenVPN to use new server
        $this->config['openvpn']['openvpn-server'][$this->id] = $this->validated_data;
        $this->write_config();

        openvpn_delete('server', $this->original_server);
        openvpn_resync('server', $this->validated_data);        
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_vpnid() {
        if (isset($this->initial_data['vpnid'])) {
            foreach ($this->config["openvpn"]["openvpn-server"] as $i => $server) {
                if ((string) $this->initial_data['vpnid'] === (string) $server['vpnid']) {
                    $this->id = $i;
                    $this->validated_data = $this->config['openvpn']['openvpn-server'][$this->id];
                    $this->original_server = $this->config['openvpn']['openvpn-server'][$this->id];
                    break;
                }                 
            }
            if (empty($this->validated_data["vpnid"])) {
                $this->errors[] = APIResponse\get(2139);
            }
        } else {
            $this->errors[] = APIResponse\get(2140);
        }
    }

    private function __validate_mode() {
        # Local variables
        $key_types = ["p2p_tls", "p2p_shared_key", "server_tls", "server_user", "server_tls_user"];
        
        # Validate Server mode
        if (isset($this->initial_data["mode"])) {
            if (in_array($this->initial_data["mode"], $key_types)) {
                $this->validated_data["mode"] = $this->initial_data["mode"];
            } else {
                $this->errors[] = APIResponse\get(2104);
            }
        }
    }

    private function __validate_authmode() {
        # Validate Backend for authentication
        if (isset($this->initial_data["authmode"])) {
            $this->validated_data["authmode"] = $this->initial_data["authmode"];
        }
    }

    private function __validate_protocol() {
        # Local variables
        $key_types = ["UDP4", "UDP6", "TCP4", "TCP6", "UDP", "TCP"];

        # Validate Protocol
        if (isset($this->initial_data["protocol"])) {
            if (in_array(strtoupper($this->initial_data["protocol"]), $key_types)) {
                $this->validated_data["protocol"] = strtoupper($this->initial_data["protocol"]);  
            } else {
                $this->errors[] = APIResponse\get(2105);
            }
        }
    }

    private function __validate_dev_mode() {
        # Local variables
        $key_types = ["tun", "tap"];

        # Validate Device mode
        if (isset($this->initial_data["dev_mode"])) {
            if (in_array($this->initial_data["dev_mode"], $key_types)) {
                $this->validated_data["dev_mode"] = $this->initial_data["dev_mode"];  
            } else {
                $this->errors[] = APIResponse\get(2106);
            }
        }
    }

    private function __validate_interface() {
        if (($this->validated_data["protocol"] === "UDP") or ($this->validated_data["protocol"] === "TCP")) {
            $this->validated_data["interface"] = "wan";
            $this->validated_data["ipaddr"] = "";
        } elseif (isset($this->initial_data["interface"])) {
            # Local variables
            $match = false;
            $interface = $this->initial_data["interface"];

            // Validate Interface
            if (!empty(APITools\get_pfsense_if_id($interface))) {
                if (isset($this->config["interfaces"][APITools\get_pfsense_if_id($interface)]["enable"])) {
                    $this->validated_data["interface"] = APITools\get_pfsense_if_id($interface);
                    $this->validated_data["ipaddr"] = "";
                    $match = true;
                }
            }

            // Validate Virtual Interface
            if (!$match) {
                foreach ($this->config["virtualip"]["vip"] as $vip) {
                    if (($vip["mode"] === "ipalias") or ($vip["mode"] === "carp") ) {
                        if (strtolower($this->initial_data["interface"]) === strtolower($vip["descr"])) {
                            $this->validated_data["interface"] = "_vip" . $vip["uniqid"];
                            $this->validated_data["ipaddr"] = $vip["subnet"];
                            $match = true;
                            break;
                        } elseif ((filter_var($vip["subnet"], FILTER_VALIDATE_IP)) and ($this->initial_data["interface"] === $vip["subnet"])) {
                            $this->validated_data["interface"] = "_vip" . $vip["uniqid"];
                            $this->validated_data["ipaddr"] = $vip["subnet"];
                            $match = true;
                            break;
                        } elseif (strtolower($this->initial_data["interface"]) === $vip["uniqid"]) {
                            $this->validated_data["interface"] = "_vip" . $vip["uniqid"];
                            $this->validated_data["ipaddr"] = $vip["subnet"];
                            $match = true;
                            break;
                        }
                    }
                }
            }

            // Validate Gateway Group Interface
            if (!$match) {
                foreach ($this->config["gateways"]["gateway_group"] as $gateway_group) {
                    if (strtolower($this->initial_data["interface"]) === strtolower($gateway_group['name'])) {
                        $this->validated_data["interface"] = $gateway_group['name'];
                        $this->validated_data["ipaddr"] = "";
                        $match = true;
                        break;
                    }
                }
            }

            if (!$match) {
                $this->errors[] = APIResponse\get(2107);
            }
        }
    }

    private function __validate_local_port() {
        # Only create local_port if no errors were found
        if (isset($this->initial_data["local_port"])) {
            if (!openvpn_validate_port($this->initial_data["local_port"], "local_port", 1)) {
                if (openvpn_port_used($this->validated_data["protocol"], $this->validated_data["interface"], $this->initial_data["local_port"], 0) == 0) {
                    $this->validated_data["local_port"] = $this->initial_data["local_port"];
                } else {
                    $this->errors[] = APIResponse\get(2108);
                }
            } else {
                $this->errors[] = APIResponse\get(2108);
            }
        }
    }

    private function __validate_description() {
        # Check for our optional `descr` payload value
        if (isset($this->initial_data['description'])) {
            $this->validated_data["description"] = $this->initial_data['description'];
        }
    }

    private function __validate_custom_options() {
        # Check for our optional `Custom options` payload value 
        if (isset($this->initial_data['custom_options'])) {
            $this->validated_data["custom_options"] = $this->initial_data['custom_options'];
        }
    }

    private function __validate_tls() {
        # Check for our optional `TLS` payload value
        if (isset($this->initial_data["tls"])) {
            # Auto generate TLS key if empty
            if ($this->initial_data["tls"] == "" or $this->initial_data["tls"] == "true") {
                $this->validated_data["tls"] = base64_encode(openvpn_create_key());
            } else {
                $this->validated_data["tls"] = base64_encode($this->initial_data["tls"]);
            }

            # Disable TLS if fasle
            if ($this->initial_data["tls"] == "false") {
                unset($this->validated_data["tls"]);
            }
        }
    }

    private function __validate_tls_type() {
        # Local variables
        $key_types = ["auth", "crypt"];

        # Validate TLS type if TLS set
        if (isset($this->validated_data["tls"])) {
            if (isset($this->initial_data["tls_type"])) {
                if (in_array($this->initial_data["tls_type"], $key_types)) {
                    $this->validated_data["tls_type"] = $this->initial_data["tls_type"];  
                } else {
                    $this->errors[] = APIResponse\get(2109);
                }
            }
        } else {
            unset($this->validated_data["tls_type"]);
        }
    }

    private function __validate_tls_keydir() {
        # Local variables
        $key_types = ["default", "direction-0", "direction-1", "both-directions"];

        # Validate keydir direction if TLS set
        if (isset($this->validated_data["tls"])) {
            if (isset($this->initial_data["tlsauth_keydir"])) {
                if (in_array($this->initial_data["tlsauth_keydir"], $key_types)) {
                    switch ($this->initial_data["tlsauth_keydir"]) {
                        case "default":
                            $this->validated_data["tlsauth_keydir"] = "default";
                            break;
                        case "direction-0":
                            $this->validated_data["tlsauth_keydir"] = "0";
                            break;
                        case "direction-1":
                            $this->validated_data["tlsauth_keydir"] = "1";
                            break;
                        case "both-directions":
                            $this->validated_data["tlsauth_keydir"] = "2";
                            break;
                    } 
                } else {
                    $this->errors[] = APIResponse\get(2110);
                }
            }
        } else {
            unset($this->validated_data["tlsauth_keydir"]);
        }
    }

    private function __validate_shared_key() {
        # Check for our optional `Shared key` payload value
        if (isset($this->initial_data["shared_key"])) {
            # Auto generate Shared key if empty
            if ($this->initial_data["shared_key"] === "") {
                $this->validated_data["shared_key"] = base64_encode(openvpn_create_key());
            # Assume default if none was specified
            } else {
                $this->validated_data["shared_key"] = base64_encode($this->initial_data["shared_key"]);
            }
        }
    }

    private function __validate_caref() {
        if (isset($this->initial_data["caref"])) {
            foreach ($this->config["ca"] as $ca) {
                if ($this->initial_data["caref"] === $ca["refid"]) {
                    $this->validated_data["caref"] = $ca["refid"];
                    break;
                }
            }

            if ($this->validated_data["caref"] !== $this->initial_data["caref"]) {
                $this->errors[] = APIResponse\get(2142);
            }
        }
    }

    private function __validate_crlref() {
        if (isset($this->initial_data["crlref"])) {
            if ($this->initial_data["crlref"] === "none") {
                $this->validated_data["crlref"] = "";
            } else {
                foreach ($this->config["crl"] as $crl) {
                    if (($this->validated_data["caref"] === $crl["caref"]) and ($this->initial_data["crlref"] === $crl["refid"])) {
                        $this->validated_data["crlref"] = $this->initial_data["refid"];
                        break;
                    }

                    if ((empty($this->validated_data["crlref"])) or ($this->validated_data["crlref"] !== $this->initial_data["crlref"])) {
                        $this->errors[] = APIResponse\get(2137);
                    }
                }
            }
        }      
    }

    private function __validate_ocspcheck() {
        # Check for our optional `OCSP` payload value
        if (($this->initial_data["ocspcheck"] === true) and (empty($this->validated_data["ocspcheck"]))) {
            $this->validated_data["ocspcheck"] = "yes";
        } elseif (($this->initial_data["ocspcheck"] === false) and (!empty($this->validated_data["ocspcheck"]))) {
            unset($this->validated_data["ocspcheck"]);
        }
    }

    private function __validate_ocspurl() {
        # Check for our optional `OCSP URL` payload value
        if ($this->validated_data["ocspcheck"] === "yes") {
            if (isset($this->initial_data["ocspurl"])) {
                $this->validated_data["ocspurl"] = $this->initial_data["ocspurl"];
            }
        } elseif (empty($this->validated_data["ocspcheck"])) {
            unset($this->validated_data["ocspurl"]);
        }
    }

    private function __validate_certref() {
        if (isset($this->initial_data["certref"])) {
            foreach ($this->config["cert"] as $cert) {
                if ($this->initial_data["certref"] === $cert["refid"]) {
                    $this->validated_data["certref"] = $cert["refid"];
                    break;
                }
            }

            if ($this->validated_data["certref"] !== $this->initial_data["certref"]) {
                $this->errors[] = APIResponse\get(2143);
            }
        } 
    }

    private function __validate_dh_length() {
        # Local variables
        $key_types = ["1024", "2048", "3072", "4096", "6144", "7680", "8192", "15360", "16384", "ECDH Only"];

        # Validate DH Parameter Length
        if (isset($this->initial_data["dh_length"])) {
            if (in_array($this->initial_data["dh_length"], $key_types)) {
                if ($this->initial_data["dh_length"] === "ECDH Only") {
                    $this->validated_data["dh_length"] = "none";
                } else {
                    $this->validated_data["dh_length"] = $this->initial_data["dh_length"];
                }
            } else {
                $this->errors[] = APIResponse\get(2111);
            }
        }
    }

    private function __validate_ecdh_curve() {
        # Local variables
        $key_types = ["none", "prime256v1", "secp384r1", "secp521r1"];

        # Validate ECDH Curve
        if (isset($this->initial_data["ecdh_curve"])) {
            if (in_array($this->initial_data["ecdh_curve"], $key_types)) {
                $this->validated_data["ecdh_curve"] = $this->initial_data["ecdh_curve"];
            } else {
                $this->errors[] = APIResponse\get(2112);
            }
        }
    }

    private function __validate_cert_depth() {
        # Local variables
        $key_types = ["none", "one", "two", "three", "four", "five"];

        # Validate Certificate Depth
        if (isset($this->initial_data["cert_depth"])) {
            if (in_array($this->initial_data["cert_depth"], $key_types)) {
                switch ($this->initial_data["cert_depth"]) {
                    case "none":
                        $this->validated_data["cert_depth"] = "";
                        break;
                    case "one":
                        $this->validated_data["cert_depth"] = "1";
                        break;
                    case "two":
                        $this->validated_data["cert_depth"] = "2";
                        break;
                    case "three":
                        $this->validated_data["cert_depth"] = "3";
                        break;
                    case "four":
                        $this->validated_data["cert_depth"] = "4";
                        break;
                    case "five":
                        $this->validated_data["cert_depth"] = "5";
                        break;
                } 
            } else {
                $this->errors[] = APIResponse\get(2113);
            }
        }
    }

    private function __validate_strick_user_cn() {
        if (($this->initial_data['strictusercn'] === true) and ($this->validated_data["strictusercn"] === "")) {
            $this->validated_data["strictusercn"] = "yes";
        } elseif (($this->initial_data['strictusercn'] === false) and ($this->validated_data["strictusercn"] === "yes")) {
            $this->validated_data["strictusercn"] = "";
        }
    }

    private function __validate_ncp_enable() {
        if (($this->initial_data["data_encryption_negotiation"] === false) and ($this->validated_data["ncp_enable"] === "enabled")) {
            $this->validated_data["ncp_enable"] = "disabled";
        } elseif (($this->initial_data["data_encryption_negotiation"] === true) and ($this->validated_data["ncp_enable"] === "disabled")) {
            $this->validated_data["ncp_enable"] = "enabled";
        }
    }

    private function __validate_data_ciphers() {
        if (isset($this->initial_data["data_ciphers"])) {
            $data_ciphers = explode(',', $this->initial_data['data_ciphers']);
            $counter = 1;
            foreach ($data_ciphers as $cipher) {
                if (!array_key_exists($cipher, openvpn_get_cipherlist())) {
                    $this->errors[] = APIResponse\get(2138);
                } elseif ($counter > 19) {
                    break;
                } else {
                    $counter += 1;
                }
            }
            $this->validated_data["data_ciphers"] = $this->initial_data['data_ciphers'];
        }
    }

    private function __validate_data_ciphers_fallback() {
        # Validate Fallback Data Encryption Algorithm
        if (isset($this->initial_data["data_ciphers_fallback"])) {
            if (array_key_exists($this->initial_data["data_ciphers_fallback"], openvpn_get_cipherlist())) {
                $this->validated_data["data_ciphers_fallback"] = $this->initial_data["data_ciphers_fallback"];
            } else {
                $this->errors[] = APIResponse\get(2114);
            }
        }
    }

    private function __validate_digest() {
        # Local variables
        $key_types = ["SHA256", "BLAKE2b512", "BLAKE2b256", "MD4", "MD5", "MD5-SHA1", "MDC2", "RIPEMD160", "SHA1", "SHA224", "SHA256", "SHA3-224", "SHA3-256", "SHA3-384", "SHA3-512", "SHA384", "SHA512", "SHA512-224", "SHA512-256", "SHAKE128", "SHAKE256", "SM3", "whirlpool", "none"];

        # Validate Auth digest algorithm
        if (isset($this->initial_data["digest"])) {
            if (in_array($this->initial_data["digest"], $key_types)) {
                $this->validated_data["digest"] = $this->initial_data["digest"];
            } else {
                $this->errors[] = APIResponse\get(2115);
            }
        }
    }

    private function __validate_engine() {
        # Validate Hardware Crypto
        if (isset($this->initial_data["engine"])) {
            if (array_key_exists($this->initial_data["engine"], openvpn_get_engines())) {
                $this->validated_data["engine"] = $this->initial_data["engine"];
            } else {
                $this->errors[] = APIResponse\get(2116);
            }
        }
    }

    private function __validate_tunnel_network() {
        # Validate Tunnel Network
        if (isset($this->initial_data['tunnel_network'])) {
            if (!openvpn_validate_cidr($this->initial_data['tunnel_network'], "", false, "ipv4")) {
                if (!openvpn_is_tunnel_network_in_use($this->initial_data['tunnel_network'])) {
                    $this->validated_data["tunnel_network"] = $this->initial_data['tunnel_network'];
                } else {
                    $this->errors[] = APIResponse\get(2117);
                }
            } else {
                $this->errors[] = APIResponse\get(2117);
            }
        }
    }

    private function __validate_tunnel_networkv6() {
        # Validate Tunnel Network v6
        if (isset($this->initial_data['tunnel_networkv6'])) {
            if (!openvpn_validate_cidr($this->initial_data['tunnel_networkv6'], "", false, "ipv6")) {
                if (!openvpn_is_tunnel_network_in_use($this->initial_data['tunnel_networkv6'])) {
                    $this->validated_data["tunnel_networkv6"] = $this->initial_data['tunnel_networkv6'];
                } else {
                    $this->errors[] = APIResponse\get(2117);
                }
            } else {
                $this->errors[] = APIResponse\get(2117);
            }
        }
    }

    private function __validate_serverbridge_dhcp() {
        if (($this->validated_data['dev_mode'] === 'tap') and (isset($this->initial_data["serverbridge_dhcp"]))) {
            if ($this->initial_data['serverbridge_dhcp'] === true) {
                $this->validated_data['serverbridge_dhcp'] = 'yes';
            } else {
                $this->validated_data['serverbridge_dhcp'] = "";
            }
        }
    }

    private function __validate_serverbridge_interface() {
        $key_types = ["wan", "lan", "none"];
        $match = false;

        if (($this->validated_data['serverbridge_dhcp'] === 'yes') and (isset($this->initial_data['serverbridge_interface']))) {
            // Validate Virtual Interface
            foreach ($this->config["virtualip"]["vip"] as $vip) {
                if (strtolower($this->initial_data["serverbridge_interface"]) === strtolower($vip["descr"])) {
                    $this->validated_data["interface"] = "_vip" . $vip["uniqid"] . "|" . $vip["subnet"];
                    $match = true;
                    break;
                } elseif ((filter_var($vip["subnet"], FILTER_VALIDATE_IP)) and ($this->initial_data["serverbridge_interface"] === $vip["subnet"])) {
                    $this->validated_data["interface"] = "_vip" . $vip["uniqid"] . "|" . $vip["subnet"];
                    $match = true;
                    break;
                } elseif (strtolower($this->initial_data["serverbridge_interface"]) === strtolower($vip["uniqid"])) {
                    $this->validated_data["interface"] = "_vip" . $vip["uniqid"] . "|" . $vip["subnet"];
                    $match = true;
                    break;
                }
            }

            if (in_array(strtolower($this->initial_data['serverbridge_interface']), $key_types)) {
                $this->validated_data["serverbridge_interface"] = strtolower($this->initial_data['serverbridge_interface']);
                $match = true;
            } 
            
            if (!$match) {
                $this->errors[] = APIResponse\get(2146);
            }
        }
    }

    private function __validate_serverbridge_routegateway() {
        if (($this->validated_data['serverbridge_dhcp'] === 'yes') and (isset($this->initial_data['serverbridge_routegateway']))) {
            if ($this->initial_data['serverbridge_routegateway'] === true) {
                if ($this->validated_data["serverbridge_interface"] !== "none") {
                    $this->validated_data["serverbridge_routegateway"] = "yes";
                } else {
                    $this->errors[] = APIResponse\get(2142);
                }
            } else {
                $this->validated_data["serverbridge_routegateway"] = "";
            }
        }
    }

    private function __validate_serverbridge_dhcp_start() {
        if (($this->validated_data['serverbridge_dhcp'] === 'yes') and (isset($this->initial_data['serverbridge_dhcp_start']))) {
            if ((isset($this->initial_data['serverbridge_dhcp_start'])) and (is_ipaddrv4($this->initial_data['serverbridge_dhcp_start'])) and (isset($this->initial_data['serverbridge_dhcp_end']))) {
                $this->validated_data["serverbridge_dhcp_start"] = $this->initial_data['serverbridge_dhcp_start'];
            } else {
                $this->errors[] = APIResponse\get(2148);
            }
        }
    }

    private function __validate_serverbridge_dhcp_end() {
        if (($this->validated_data['serverbridge_dhcp'] === 'yes') and (isset($this->initial_data['serverbridge_dhcp_end']))) {
            if ((isset($this->initial_data['serverbridge_dhcp_end'])) and (is_ipaddrv4($this->initial_data['serverbridge_dhcp_end'])) and (isset($this->initial_data['serverbridge_dhcp_start']))) {
                $this->validated_data["serverbridge_dhcp_end"] = $this->initial_data['serverbridge_dhcp_end'];
            } else {
                $this->errors[] = APIResponse\get(2148);
            }
        }
    }

    private function __validate_remote_network() {
        # Check for our optional `Remote Network` payload value
        if (isset($this->initial_data['remote_network'])) {
            if (!openvpn_validate_cidr($this->initial_data['remote_network'], "", true, "ipv4")) {
                $this->validated_data["remote_network"] = $this->initial_data['remote_network'];
            } else {
                $this->errors[] = APIResponse\get(2118);
            }
        }
    }

    private function __validate_remote_networkv6() {
        # Check for our optional `Remote Network v6` payload value
        if (isset($this->initial_data['remote_networkv6'])) {
            if (!openvpn_validate_cidr($this->initial_data['remote_networkv6'], "", true, "ipv6") === false) {
                $this->validated_data["remote_networkv6"] = $this->initial_data['remote_networkv6'];
            } else {
                $this->errors[] = APIResponse\get(2118);
            }
        }
    }

    private function __validate_redirect_gateway() {
        if (isset($this->initial_data['redirect_gateway'])) {
            if (($this->initial_data['redirect_gateway'] === true) and ($this->validated_data["gwredir"] === "")) {
                $this->validated_data["gwredir"] = "yes";
            } elseif (($this->initial_data['redirect_gateway'] === false) and ($this->validated_data["gwredir"] === "yes")) {
                $this->validated_data["gwredir"] = "";
            }
        } 
    }

    private function __validate_redirect_gateway6() {
        if (isset($this->initial_data['redirect_gateway6'])) {
            if (($this->initial_data['redirect_gateway6'] === true) and ($this->validated_data["gwredir6"] === "")) {
                $this->validated_data["gwredir6"] = "yes";
            } elseif (($this->initial_data['redirect_gateway6'] === false) and ($this->validated_data["gwredir6"] === "yes")) {
                $this->validated_data["gwredir6"] = "";
            }
        }
    }

    private function __validate_local_network() {
        if ($this->validated_data['gwredir'] === "") {
            if (isset($this->initial_data['local_network'])) {
                if (!openvpn_validate_cidr($this->initial_data['local_network'], "", true, "ipv4")) {
                    $this->validated_data["local_network"] = $this->initial_data['local_network'];
                } else {
                    $this->errors[] = APIResponse\get(2119);
                }
            }
        } else {
            $this->validated_data["local_network"] = "";
        }
    }

    private function __validate_local_networkv6() {
        if ($this->validated_data['gwredir6'] === "") {
            if (isset($this->initial_data['local_networkv6'])) {
                if (!openvpn_validate_cidr($this->initial_data['local_networkv6'], "", true, "ipv6")) {
                    $this->validated_data["local_networkv6"] = $this->initial_data['local_networkv6'];
                } else {
                    $this->errors[] = APIResponse\get(2119);
                }
            }
        } else {
            $this->validated_data["local_networkv6"] = "";
        }
    }

    private function __validate_concurrent_connections() {
        if (isset($this->initial_data['concurrent_connections'])) {
            if (is_numericint($this->initial_data['concurrent_connections']) and (int) $this->initial_data['concurrent_connections'] > 0) {
                $this->validated_data["maxclients"] = $this->initial_data['concurrent_connections'];
            } else {
                $this->errors[] = APIResponse\get(2133);
            }
        }
    }

    private function __validate_allow_compression() {
        # Local variables
        $key_types = ["asym", "no", "yes"];

        # Validate Allow Compression
        if (isset($this->initial_data["allow_compression"])) {
            if (in_array($this->initial_data["allow_compression"], $key_types)) {
                $this->validated_data["allow_compression"] = $this->initial_data["allow_compression"];
            } else {
                $this->errors[] = APIResponse\get(2120);
            }
        }
    }

    private function __validate_compression() {
        # Local variables
        $key_types = ["none", "stub", "stub-v2", "lz4", "lz4-v2", "lzo", "noadapt", "adaptive", "yes", "no"];

        if (($this->validated_data["allow_compression"] === "yes") or ($this->validated_data["allow_compression"] === "asym")) {
            # Validate Compression
            if (isset($this->initial_data["compression"])) {
                if (in_array($this->initial_data["compression"], $key_types)) {
                    $this->validated_data["compression"] = $this->initial_data["compression"];
                } else {
                    $this->errors[] = APIResponse\get(2121);
                }
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["compression"] = "";
        } 
    }

    private function __validate_compression_push() {
        if (($this->initial_data['compression_push'] === true) and ($this->validated_data["compression_push"] === "")) {
            $this->validated_data["compression_push"] = "yes";
        # Assume default if none was specified
        } elseif (($this->initial_data['compression_push'] === false) and ($this->validated_data["compression_push"] === "yes")) {
            $this->validated_data["compression_push"] = "";
        }
    }

    private function __validate_type_of_service() {
        if (($this->initial_data['passtos'] === true) and ($this->validated_data["passtos"] === "")) {
            $this->validated_data["passtos"] = "yes";
        } elseif (($this->initial_data['passtos'] === false) and ($this->validated_data["passtos"] === "yes")) {
            $this->validated_data["passtos"] = "";
        }
    }

    private function __validate_inter_client_communication() {
        if (($this->initial_data['client2client'] === true) and ($this->validated_data["client2client"] === "")) {
            $this->validated_data["client2client"] = "yes";
        # Assume default if none was specified
        } elseif (($this->initial_data['client2client'] === false) and ($this->validated_data["client2client"] === "yes")) {
            $this->validated_data["client2client"] = "";
        }
    }

    private function __validate_duplicate_connection() {
        if (($this->initial_data['duplicate_cn'] === true) and (empty($this->validated_data["duplicate_cn"]))) {
            $this->validated_data["duplicate_cn"] = "";
        } elseif (($this->initial_data['duplicate_cn'] === false) and (!empty($this->validated_data["duplicate_cn"]))) {
            unset($this->validated_data["duplicate_cn"]);
        }
    }

    private function __validate_dynamic_ip() {
        if (($this->initial_data['dynamic_ip'] === true) and ($this->validated_data["dynamic_ip"] === "")) {
            $this->validated_data["dynamic_ip"] = "yes";
        } elseif (($this->initial_data['dynamic_ip'] === false) and ($this->validated_data["dynamic_ip"] === "yes")) {
            $this->validated_data["dynamic_ip"] = "";
        }
    }

    // TODO: Add validation for "topology"
    private function __validate_topology() {
        # Local variables
        $key_types = ["subnet", "net30"];

        # Validate Topology
        if ($this->validated_data["dev_mode"] === "tap") {
            $this->validated_data["topology"] = "subnet";
        } elseif (isset($this->initial_data["topology"])) {
            if (in_array($this->initial_data["topology"], $key_types)) {
                $this->validated_data["topology"] = $this->initial_data["topology"];
            } else {
                $this->errors[] = APIResponse\get(2122);
            }
        }
    }

    private function __validate_dns_domain() {
        if (isset($this->initial_data['dns_domain'])) {
            $this->validated_data["dns_domain"] = $this->initial_data['dns_domain'];
        }
    }
    
    private function __validate_dns_servers() {
        if (isset($this->initial_data['dns_servers'])) {
            $dns_servers = explode(',', $this->initial_data['dns_servers']);
            $counter = 1;
            foreach ($dns_servers as $server) {
                if ($counter > 4) {
                    break;
                } elseif (is_ipaddrv4(str_replace(" ", "", $server))) {
                    $this->validated_data["dns_server" . $counter] = str_replace(" ", "", $server);
                    $counter += 1;
                }
            }
        }
    }

    private function __validate_push_blockout_side_dns() {
        if (($this->initial_data['push_blockoutsidedns'] === true) and (empty($this->validated_data["push_blockoutsidedns"]))) {
            $this->validated_data["push_blockoutsidedns"] = "yes";
        } elseif (($this->initial_data['push_blockoutsidedns'] === false) and (!empty($this->validated_data["push_blockoutsidedns"]))) {
            unset($this->validated_data["push_blockoutsidedns"]);
        }
    }

    private function __validate_inactive_seconds() {
        if (isset($this->initial_data['inactive_seconds'])) {
            if (is_numericint($this->initial_data['inactive_seconds']) and ((int) $this->initial_data['inactive_seconds']) >= 0) {
                $this->validated_data["inactive_seconds"] = $this->initial_data['inactive_seconds'];
            # Assume default if none was specified
            } else {
                $this->errors[] = APIResponse\get(2134);
            }
        }
    }

    private function __validate_ping_method() {
        # Local variables
        $key_types = ["keepalive", "ping"];

        # Validate Ping method
        if (isset($this->initial_data["ping_method"])) {
            if (in_array($this->initial_data["ping_method"], $key_types)) {
                $this->validated_data["ping_method"] = $this->initial_data["ping_method"];
            } else {
                $this->errors[] = APIResponse\get(2123);
            }
        }
    }

    private function __validate_keepalive_interval() {
        if ($this->validated_data["ping_method"] === "keepalive") {
            if (isset($this->initial_data['keepalive_interval'])) {
                if (is_numericint($this->initial_data['keepalive_interval']) and ((int) $this->initial_data['keepalive_interval'] >= 0)) {
                    $this->validated_data["keepalive_interval"] = $this->initial_data['keepalive_interval'];
                } else {
                    $this->errors[] = APIResponse\get(2124);
                }
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["keepalive_interval"] = "10";
        }
    }

    private function __validate_keepalive_timeout() {
        if ($this->validated_data["ping_method"] === "keepalive") {
            if (isset($this->initial_data['keepalive_timeout'])) {
                if (is_numericint($this->initial_data['keepalive_timeout']) and ((int) $this->initial_data['keepalive_timeout'] >= 0)) {
                    $this->validated_data["keepalive_timeout"] = $this->initial_data['keepalive_timeout'];
                } else {
                    $this->errors[] = APIResponse\get(2125);
                }
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["keepalive_timeout"] = "60";
        }
    }

    private function __validate_ping_seconds() {
        if ($this->validated_data["ping_method"] === "ping") {
            if (isset($this->initial_data['ping_seconds'])) {
                if (is_numericint($this->initial_data['ping_seconds']) and ((int) $this->initial_data['ping_seconds'] >= 0)) {
                    $this->validated_data["ping_seconds"] = $this->initial_data['ping_seconds'];
                } else {
                    $this->errors[] = APIResponse\get(2126);
                }
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["ping_seconds"] = "10";
        }
    }

    private function __validate_ping_action_seconds() {
        if ($this->validated_data["ping_method"] === "ping") {
            if (isset($this->initial_data['ping_action_seconds'])) {
                if (is_numericint($this->initial_data['ping_action_seconds']) and ((int) $this->initial_data['ping_action_seconds'] >= 0)) {
                    $this->validated_data["ping_action_seconds"] = $this->initial_data['ping_action_seconds'];
                } else {
                    $this->errors[] = APIResponse\get(2127);
                }
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["ping_action_seconds"] = "60";
        }
    }

    private function __validate_ping_push() {
        if ($this->validated_data["ping_method"] === "ping") {
            if (($this->initial_data['ping_push'] === true) and ($this->validated_data["ping_push"] === "")) {
                $this->validated_data["ping_push"] = "yes";
            } elseif (($this->initial_data['ping_push'] === false) and ($this->validated_data["ping_push"] === "yes")) {
                $this->validated_data["ping_push"] = "";
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["ping_push"] = "";
        }
    }

    private function __validate_ping_action_push() {
        if ($this->validated_data["ping_method"] === "ping") {
            if (($this->initial_data['ping_action_push'] === true) and ($this->validated_data["ping_action_push"] === "")) {
                $this->validated_data["ping_action_push"] = "yes";
            } elseif (($this->initial_data['ping_action_push'] === false) and ($this->validated_data["ping_action_push"] === "yes")) {
                $this->validated_data["ping_action_push"] = "";
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["ping_action_push"] = "";
        }
    }

    private function __validate_ping_action() {
        # Local variables
        $key_types = ["ping_exit", "ping_restart"];
        
        # Validate Ping restart or exit
        if ($this->validated_data["ping_method"] === "ping") {
            if (isset($this->initial_data["ping_action"])) {
                if (in_array($this->initial_data["ping_action"], $key_types)) {
                    $this->validated_data["ping_action"] = $this->initial_data["ping_action"];
                }
                else {
                    $this->errors[] = APIResponse\get(2128);
                }
            }
        # Assume default if none was specified
        } else {
            $this->validated_data["ping_action"] = "ping_restart";
        }
    }

    private function __validate_username_as_cn() {
        if (($this->initial_data['username_as_common_name'] === true) and ($this->validated_data["username_as_common_name"] === "disabled")) {
            $this->validated_data["username_as_common_name"] = "enabled";
        } elseif (($this->initial_data['username_as_common_name'] === false) and ($this->validated_data["username_as_common_name"] === "enabled")) {
            $this->validated_data["username_as_common_name"] = "disabled";
        }
    }

    private function __validate_udp_fast_io() {
        if (($this->initial_data['udp_fast_io'] === true) and (empty($this->validated_data["udp_fast_io"]))) {
            $this->validated_data["udp_fast_io"] = "yes";
        } elseif (($this->initial_data['udp_fast_io'] === false) and (!empty($this->validated_data["udp_fast_io"]))) {
            unset($this->validated_data["udp_fast_io"]);
        }
    }

    private function __validate_exit_notify() {
        # Local variables
        $key_types = ["once", "twice", "none", ""];

        # Validate Exit Notify
        if (isset($this->initial_data["exit_notify"])) {
            if (in_array($this->initial_data["exit_notify"], $key_types)) {
                switch ($this->initial_data["exit_notify"]) {
                    case "none":
                        $this->validated_data["exit_notify"] = "none";
                        break;
                    case "":
                        $this->validated_data["exit_notify"] = "none";
                        break;
                    case "once":
                        $this->validated_data["exit_notify"] = "1";
                        break;
                    case "twice":
                        $this->validated_data["exit_notify"] = "2";
                        break;
                } 
            } else {
                $this->errors[] = APIResponse\get(2129);
            }
        }
    }

    private function __validate_snd_rcv_buf() {
        # Local variables
        $key_types = ["64KiB", "128KiB", "256KiB", "512KiB", "1MiB", "2MiB", "default"];

        # Validate Send/Receive Buffer
        if (isset($this->initial_data["sndrcvbuf"])) {
            if (in_array($this->initial_data["sndrcvbuf"], $key_types)) {
                switch ($this->initial_data["sndrcvbuf"]) {
                    case "64KiB":
                        $this->validated_data["sndrcvbuf"] = "65536";
                        break;
                    case "128KiB":
                        $this->validated_data["sndrcvbuf"] = "131072";
                        break;
                    case "256KiB":
                        $this->validated_data["sndrcvbuf"] = "262144";
                        break;
                    case "512KiB":
                        $this->validated_data["sndrcvbuf"] = "524288";
                        break;
                    case "1MiB":
                        $this->validated_data["sndrcvbuf"] = "1048576";
                        break;
                    case "2MiB":
                        $this->validated_data["sndrcvbuf"] = "2097152";
                        break;
                    case "default":
                        $this->validated_data["sndrcvbuf"] = "";
                        break;
                }
            } else {
                $this->errors[] = APIResponse\get(2130);
            }
        }
    }

    private function __validate_push_register_dns() {
        if (($this->initial_data['push_register_dns'] === true) and (empty($this->validated_data["push_register_dns"]))) {
            $this->validated_data["push_register_dns"] = "yes";
        } elseif (($this->initial_data['push_register_dns'] === false) and (!empty($this->validated_data["push_register_dns"]))) {
            unset($this->validated_data["push_register_dns"]);
        }
    }

    private function __validate_ntp_servers() {
        if (isset($this->initial_data['ntp_servers'])) {
            $ntp_servers = explode(',', $this->initial_data['ntp_servers']);
            $counter = 1;
            foreach ($ntp_servers as $server) {
                if ($counter > 2) {
                    break;
                } elseif (is_ipaddrv4(str_replace(" ", "", $server))) {
                    $this->validated_data["ntp_server" . $counter] = str_replace(" ", "", $server);
                    $counter += 1;
                } else {
                    $this->errors[] = APIResponse\get(2149);
                } 
            }
        }
    }

    private function __validate_netbios_enable() {
        if (($this->initial_data['netbios_enable'] === true) and ($this->validated_data["netbios_enable"] === "")) {
            $this->validated_data["netbios_enable"] = "yes";
        } elseif (($this->initial_data['netbios_enable'] === false) and ($this->validated_data["netbios_enable"] === "yes")) {
            $this->validated_data["netbios_enable"] = "";
        }
    }

    private function __validate_netbios_ntype() {
        if (isset($this->initial_data['netbios_node_type'])) {
            if ($this->initial_data['netbios_node_type'] == 'b') {
                $this->validated_data["netbios_ntype"] = "1";
            } else if ($this->initial_data['netbios_node_type'] == 'p') {
                $this->validated_data["netbios_ntype"] = "2";
            } else if ($this->initial_data['netbios_node_type'] == 'm') {
                $this->validated_data["netbios_ntype"] = "3";
            } else if ($this->initial_data['netbios_node_type'] == 'h') {
                $this->validated_data["netbios_ntype"] = "4";
            } else {
                $this->errors[] = APIResponse\get(2076);
            }
        }
    }

    private function __validate_netbios_scope() {
        if (isset($this->initial_data['netbios_scope'])) {
            $this->validated_data["netbios_scope"] = $this->initial_data['netbios_scope'];
        }
    }

    private function __validate_create_gw() {
        # Local variables
        $key_types = ["both", "v4only", "v6only"];

        # Validate Gateway creation
        if (isset($this->initial_data["create_gw"])) {
            if (in_array(strtolower($this->initial_data["create_gw"]), $key_types)) {
                switch ($this->initial_data["create_gw"]) {
                    case "both":
                        $this->validated_data["create_gw"] = strtolower($this->initial_data["create_gw"]);
                        break;
                    case "v4only":
                        $this->validated_data["create_gw"] = strtolower($this->initial_data["create_gw"]);
                        break;
                    case "v6only":
                        $this->validated_data["create_gw"] = strtolower($this->initial_data["create_gw"]);
                        break;
                } 
            } else {
                $this->errors[] = APIResponse\get(2131);
            }
        }
    }

    private function __validate_verbosity_level() {
        if (isset($this->initial_data['verbosity_level'])) {
            if (is_numericint($this->initial_data['verbosity_level']) and ((int) $this->initial_data['verbosity_level'] >= 2) and ((int) $this->initial_data['verbosity_level'] < 13)) {
                $this->validated_data["verbosity_level"] = $this->initial_data["verbosity_level"];
            } elseif ($this->initial_data['verbosity_level'] == "none") {
                $this->validated_data["verbosity_level"] = "0";
            } elseif ($this->initial_data['verbosity_level'] == "default") {
                $this->validated_data["verbosity_level"] = "1";
            } else {
                $this->errors[] = APIResponse\get(2132);
            }
        }
    }

    private function __validate_wins_servers() {
        if (isset($this->initial_data['wins_servers'])) {
            $win_servers = explode(',', $this->initial_data['wins_servers']);
            $counter = 1;
            foreach ($win_servers as $server) {
                if ($counter > 2) {
                    break;
                } elseif (is_ipaddrv4(str_replace(" ", "", $server))) {
                    $this->validated_data["wins_server" . $counter] = str_replace(" ", "", $server);
                    $counter += 1;
                }  else {
                    $this->errors[] = APIResponse\get(2151);
                }
            }
        }
    }

    // TODO: Validate nbdd servers is not yet available
    private function __validate_nbdd_server() {
        if (isset($this->validated_data['wins_servers' . '1']) and ($this->validated_data["netbios_enable"] = "yes")) {
            $this->validated_data["nbdd_server1"] = "";
        } else {
            unset($this->validated_data["nbdd_server1"]);
        }
    }

    private function __validate_disabled() {
        # Check for our optional `disabled` payload value
        if (($this->initial_data["disable"] === true) and (empty($this->validated_data["disable"]))) {
            $this->validated_data["disable"] = "";
        } elseif (($this->initial_data["disable"] === false) and (!empty($this->validated_data["disable"]))) {
            unset($this->validated_data["disable"]);
        }
    }

    public function validate_payload() {
        # Run each validation method
        $this->__validate_description();
        $this->__validate_disabled();
        $this->__validate_vpnid();
        $this->__validate_mode();
        $this->__validate_dev_mode();
        $this->__validate_protocol();
        $this->__validate_interface();
        $this->__validate_local_port();
        $this->__validate_ncp_enable();
        $this->__validate_data_ciphers();
        $this->__validate_data_ciphers_fallback();
        $this->__validate_engine();
        $this->__validate_digest();
        $this->__validate_tunnel_network();
        $this->__validate_tunnel_networkv6();
        $this->__validate_concurrent_connections();
        $this->__validate_allow_compression();
        $this->__validate_compression();
        $this->__validate_type_of_service();
        $this->__validate_ping_method();
        $this->__validate_keepalive_interval();
        $this->__validate_keepalive_timeout();
        $this->__validate_ping_seconds();
        $this->__validate_ping_action_seconds();
        $this->__validate_ping_push();
        $this->__validate_ping_action_push();
        $this->__validate_ping_action();
        $this->__validate_custom_options();
        $this->__validate_udp_fast_io();
        $this->__validate_snd_rcv_buf();
        $this->__validate_create_gw();
        $this->__validate_verbosity_level();

        # Run each validation method for ["p2p_tls", "server_tls", "server_user", "server_tls_user"]
        if ($this->validated_data["mode"] !== "p2p_shared_key") {
            $this->__validate_tls();
            $this->__validate_tls_type();
            $this->__validate_tls_keydir();
            $this->__validate_caref();
            $this->__validate_crlref();
            $this->__validate_ocspcheck();
            $this->__validate_ocspurl();
            $this->__validate_certref();
            $this->__validate_dh_length();
            $this->__validate_ecdh_curve();
            $this->__validate_cert_depth();
            $this->__validate_redirect_gateway();
            $this->__validate_redirect_gateway6();
            $this->__validate_local_network();
            $this->__validate_local_networkv6();
            $this->__validate_compression_push();
            $this->__validate_inter_client_communication();
            $this->__validate_duplicate_connection();
            $this->__validate_dynamic_ip();
            $this->__validate_topology();
            $this->__validate_inactive_seconds();
            $this->__validate_exit_notify();

            if (isset($this->validated_data["shared_key"])) {
                unset($this->validated_data["shared_key"]);
            }
        }

        # Run each validation method for ["p2p_tls", "p2p_shared_key"]
        if (($this->validated_data["mode"] === "p2p_tls") or ($this->validated_data["mode"] === "p2p_shared_key")) {
            $this->__validate_remote_network();
            $this->__validate_remote_networkv6();

            $this->validated_data["serverbridge_dhcp"] = "";
            $this->validated_data["serverbridge_interface"] = "none";
            $this->validated_data["serverbridge_routegateway"] = "";
            $this->validated_data["serverbridge_dhcp_start"] = "";
            $this->validated_data["serverbridge_dhcp_end"] = "";
            $this->validated_data["netbios_enable"] = "";
            $this->validated_data["netbios_ntype"] = "0";
            $this->validated_data["netbios_scope"] = "";

            if (isset($this->validated_data["dns_domain"])) {
                unset($this->validated_data["dns_domain"]);
            }

            for ($i=1; $i < 5; $i++) { 
                if (isset($this->validated_data["dns_server" . $i])) {
                    unset($this->validated_data["dns_server" . $i]);
                }
            }

            for ($i=1; $i < 3; $i++) { 
                if (isset($this->validated_data["ntp_server" . $i])) {
                    unset($this->validated_data["ntp_server" . $i]);
                }
            }

            for ($i=1; $i < 3; $i++) { 
                if (isset($this->validated_data["wins_server" . $i])) {
                    unset($this->validated_data["wins_server" . $i]);
                }
            }

            if (isset($this->validated_data["push_blockoutsidedns"])) {
                unset($this->validated_data["push_blockoutsidedns"]);
            }

            if (isset($this->validated_data["push_register_dns"])) {
                unset($this->validated_data["push_register_dns"]);
            }
        }

        # Run each validation method for ["server_tls", "server_user", "server_tls_user"]
        if (($this->validated_data["mode"] !== "p2p_tls") and ($this->validated_data["mode"] !== "p2p_shared_key")) {
            $this->__validate_dns_domain();
            $this->__validate_dns_servers();
            $this->__validate_push_blockout_side_dns();
            $this->__validate_push_register_dns();
            $this->__validate_ntp_servers();
            $this->__validate_netbios_enable();
            $this->__validate_netbios_ntype();
            $this->__validate_netbios_scope();
            $this->__validate_wins_servers();
            $this->__validate_nbdd_server();
            $this->__validate_serverbridge_dhcp();
            $this->__validate_serverbridge_interface();
            $this->__validate_serverbridge_routegateway();
            $this->__validate_serverbridge_dhcp_start();
            $this->__validate_serverbridge_dhcp_end();

            $this->validated_data["remote_network"] = "";
            $this->validated_data["remote_networkv6"] = "";
        }
        
        # Run each validation method for ["p2p_tls", "p2p_shared_key", "server_tls"]
        if (($this->validate_data["mode"] !== "server_user") and ($this->validated_data["mode"] !== "server_tls_user")) {
            $this->validated_data["username_as_common_name"] = "disabled";

            if (isset($this->validated_data["authmode"])) {
                unset($this->validated_data["authmode"]);
            }
        } 

        # Run each validation method for ["server_user", "server_tls_user"]
        if (($this->validate_data["mode"] === "server_user") or ($this->validated_data["mode"] === "server_tls_user")) {
            $this->__validate_authmode();
            $this->__validate_username_as_cn();
        }

        if ($this->validated_data['mode'] !== 'server_tls_user') {
            unset($this->validated_data["strictusercn"]);
        }

        # Run each validation method for ["p2p_tls"]
        if ($this->validated_data["mode"] === "p2p_tls") {  
        }

        # Run each validation method for ["p2p_shared_key"]
        if ($this->validated_data["mode"] === "p2p_shared_key") {
            $this->__validate_shared_key();

            $this->validated_data["gwredir"] = "";
            $this->validated_data["gwredir6"] = "";
            $this->validated_data["local_network"] = "";
            $this->validated_data["local_networkv6"] = "";
            $this->validated_data["compression_push"] = "";
            $this->validated_data["client2client"] = "";
            $this->validated_data["topology"] = "subnet";
            $this->validated_data["dynamic_ip"] = "";
            $this->validated_data["exit_notify"] = "1";
            $this->validated_data["inactive_seconds"] = "300";

            if (isset($this->validated_data["tls"])) {
                unset($this->validated_data["tls"]);
            }

            if (isset($this->validated_data["tls_type"])) {
                unset($this->validated_data["tls_type"]);
            }

            if (isset($this->validated_data["tlsauth_keydir"])) {
                unset($this->validated_data["tlsauth_keydir"]);
            }

            if (isset($this->validated_data["caref"])) {
                unset($this->validated_data["caref"]);
            }

            if (isset($this->validated_data["certref"])) {
                unset($this->validated_data["certref"]);
            }

            if (isset($this->validated_data["crlref"])) {
                unset($this->validated_data["crlref"]);
            }

            if (isset($this->validated_data["cert_depth"])) {
                unset($this->validated_data["cert_depth"]);
            }

            if (isset($this->validated_data["ocspcheck"])) {
                unset($this->validated_data["ocspcheck"]);
            }

            if (isset($this->validated_data["ocspurl"])) {
                unset($this->validated_data["ocspurl"]);
            }

            if (isset($this->validated_data["dh_length"])) {
                unset($this->validated_data["dh_length"]);
            }

            if (isset($this->validated_data["ecdh_curve"])) {
                unset($this->validated_data["ecdh_curve"]);
            }

            if (isset($this->validated_data["duplicate_cn"])) {
                unset($this->validated_data["duplicate_cn"]);
            }
        }

        # Run each validation method for ["server_tls"]
        if ($this->validated_data["mode"] === "server_tls") {  
        }

        # Run each validation method for ["server_user"]
        if ($this->validate_data["mode"] === "server_user") {
        }

        # Run each validation method for ["server_tls_user"]
        if ($this->validated_data["mode"] === "server_tls_user") {
            $this->__validate_strick_user_cn();
        }
    }

    private function __init_config() {
        # Ensure the OpenVPN configuration array exists
        if (empty($this->config["openvpn"])) {
            $this->config["openvpn"] = [];
        }

        # Ensure the OpenVPN server configuration array exists
        if (empty($this->config['openvpn']['openvpn-server'])) {
            $this->config['openvpn']['openvpn-server'] = [];
        }
    }
}
