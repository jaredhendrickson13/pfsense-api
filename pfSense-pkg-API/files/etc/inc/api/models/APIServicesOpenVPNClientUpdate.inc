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

class APIServicesOpenVPNClientUpdate extends APIModel {
    public $original_client;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-openvpn-client"];
        $this->change_note = "Modified OpenVPN client via API";
    }

    public function action() {
        # Add our Client to the configuration and resync OpenVPN to use new client
        $this->set_config("openvpn/openvpn-client/{$this->id}", $this->validated_data);
        $this->write_config();

        openvpn_delete('client', $this->original_client);
        openvpn_resync('client', $this->validated_data);
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_vpnid() {
        if (isset($this->initial_data['vpnid'])) {
            foreach ($this->get_config("openvpn/openvpn-client", []) as $i => $client) {
                if ((string) $this->initial_data['vpnid'] === (string) $client['vpnid']) {
                    $this->id = $i;
                    $this->validated_data = $this->get_config("openvpn/openvpn-client/{$this->id}");
                    $this->original_client = $this->get_config("openvpn/openvpn-client/{$this->id}");
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
        $key_types = ["p2p_tls", "p2p_shared_key"];
        
        # Validate Server mode
        if (isset($this->initial_data["mode"])) {
            if (in_array($this->initial_data["mode"], $key_types)) {
                $this->validated_data["mode"] = $this->initial_data["mode"];
            } else {
                $this->errors[] = APIResponse\get(2104);
            }
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
                $interface = APITools\get_pfsense_if_id($interface);

                if ($this->is_config_enabled("interfaces/{$interface}")) {
                    $this->validated_data["interface"] = $interface;
                    $this->validated_data["ipaddr"] = "";
                    $match = true;
                }
            }

            // Validate Virtual Interface
            if (!$match) {
                foreach ($this->get_config("virtualip/vip", []) as $vip) {
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
                foreach ($this->get_config("gateways/gateway_group", []) as $gateway_group) {
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
            if (ctype_digit($this->initial_data["local_port"]) or is_int($this->initial_data["local_port"])) {
                if (((int) $this->initial_data["local_port"] >= 0) and ((int) $this->initial_data["local_port"] < 65536)) {
                    $this->validated_data["local_port"] = $this->initial_data["local_port"];
                } else {
                    $this->errors[] = APIResponse\get(2108);
                }
            } else {
                $this->errors[] = APIResponse\get(2108);
            }
        }
    }

    private function __validate_server_addr() {
        # Only create server_addr if no errors were found
        if (isset($this->initial_data["server_addr"])) {
            $this->validated_data["server_addr"] = $this->initial_data["server_addr"];
        }
    }

    private function __validate_server_port() {
        # Only create server_port if no errors were found
        if (isset($this->initial_data["server_port"])) {
            if (ctype_digit($this->initial_data["server_port"]) or is_int($this->initial_data["server_port"])) {
                if (((int) $this->initial_data["server_port"] >= 0) and ((int) $this->initial_data["server_port"] < 65536)) {
                    $this->validated_data["server_port"] = $this->initial_data["server_port"];
                } else {
                    $this->errors[] = APIResponse\get(2154);
                }
            } else {
                $this->errors[] = APIResponse\get(2154);
            }
        }
    }

    private function __validate_proxy_addr() {
        # Only create proxy_addr if no errors were found
        if (isset($this->initial_data["proxy_addr"])) {
            $this->validated_data["proxy_addr"] = $this->initial_data["proxy_addr"];
        }
    }

    private function __validate_proxy_port() {
        # Only create proxy_port if no errors were found
        if (isset($this->initial_data["proxy_port"])) {
            if (ctype_digit($this->initial_data["proxy_port"]) or is_int($this->initial_data["proxy_port"])) {
                if (((int) $this->initial_data["proxy_port"] >= 0) and ((int) $this->initial_data["proxy_port"] < 65536)) {
                    $this->validated_data["proxy_port"] = $this->initial_data["proxy_port"];
                } else {
                    $this->errors[] = APIResponse\get(2155);
                }
            } else {
                $this->errors[] = APIResponse\get(2155);
            }
        }
    }

    private function __validate_proxy_authtype() {
        # Local variables
        $key_types = ["none", "basic", "ntlm"];

        # Validate proxy authtype if auth set
        if (isset($this->initial_data['proxy_authtype'])) {
            if (in_array($this->initial_data["proxy_authtype"], $key_types)) {
                $this->validated_data["proxy_authtype"] = $this->initial_data["proxy_authtype"];  
            } else {
                $this->errors[] = APIResponse\get(2156);
            }
        }
    }

    private function __validate_proxy_user() {
        if (isset($this->validated_data['proxy_authtype'])) {
            if (isset($this->initial_data['proxy_user'])) {
                $this->validated_data['proxy_user'] = $this->initial_data['proxy_user'];
            } else {
                $this->validated_data['proxy_user'] = "";
            }
        }
    }

    private function __validate_proxy_passwd() {
        if (isset($this->validated_data['proxy_authtype'])) {
            if (isset($this->initial_data['proxy_passwd'])) {
                $this->validated_data['proxy_passwd'] = $this->initial_data['proxy_passwd'];
            } else {
                $this->validated_data['proxy_passwd'] = "";
            }
        }
    }

    private function __validate_auth_user() {
        if (isset($this->validated_data['auth_user'])) {
            $this->validated_data['auth_user'] = $this->initial_data['auth_user'];
        }
    }

    private function __validate_auth_passwd() {
        if (isset($this->validated_data['auth_passwd'])) {
            $this->validated_data['auth_passwd'] = $this->initial_data['auth_passwd'];
        }
    }

    private function __validate_auth_retry_none() {
        if (($this->initial_data['auth-retry-none'] === true) and ($this->validated_data["auth-retry-none"] === "")) {
            $this->validated_data["auth-retry-none"] = "yes";
        } elseif (($this->initial_data['auth-retry-none'] === false) and ($this->validated_data["auth-retry-none"] === "yes")) {
            $this->validated_data["auth-retry-none"] = "";
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

    private function __validate_remote_cert_tls() {
        if (($this->initial_data['remote_cert_tls'] === true) and ($this->validated_data["remote_cert_tls"] === "")) {
            $this->validated_data["remote_cert_tls"] = "yes";
        } elseif (($this->initial_data['remote_cert_tls'] === false) and ($this->validated_data["remote_cert_tls"] === "yes")) {
            $this->validated_data["remote_cert_tls"] = "";
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
            foreach ($this->get_config("ca", []) as $ca) {
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
                foreach ($this->get_config("crl", []) as $crl) {
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

    private function __validate_certref() {
        if (isset($this->initial_data["certref"])) {
            foreach ($this->get_config("cert", []) as $cert) {
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
            $this->validated_data['data_ciphers'] = $this->initial_data['data_ciphers']; 
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
            if (!openvpn_validate_cidr($this->initial_data['remote_networkv6'], "", true, "ipv6")) {
                $this->validated_data["remote_networkv6"] = $this->initial_data['remote_networkv6'];
            } else {
                $this->errors[] = APIResponse\get(2118);
            }
        }
    }

    private function __validate_use_shaper() {
        # Validate Use Shaper
        if (isset($this->initial_data["use_shaper"])) {
            if (ctype_digit($this->initial_data["use_shaper"]) or is_int($this->initial_data["use_shaper"])) {
                if (((int) $this->initial_data["use_shaper"] >= 100) and ((int) $this->initial_data["use_shaper"] < 100000000)) {
                    $this->validated_data["use_shaper"] = $this->initial_data["use_shaper"];
                } else {
                    $this->errors[] = APIResponse\get(2157);
                }
            } else {
                $this->errors[] = APIResponse\get(2157);
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
    
    private function __validate_topology() {
        # Local variables
        $key_types = ["subnet", "net30"];

        # Validate Topology
        if ($this->validated_data['dev_mode'] === "tap") {
            $this->validated_data["topology"] = "subnet";
        } elseif (isset($this->initial_data["topology"])) {
            if (in_array($this->initial_data["topology"], $key_types)) {
                $this->validated_data["topology"] = $this->initial_data["topology"];
            } else {
                $this->errors[] = APIResponse\get(2122);
            }
        }
    }

    private function __validate_type_of_service() {
        if (($this->initial_data['passtos'] === true) and ($this->validated_data["passtos"] === "")) {
            $this->validated_data["passtos"] = "yes";
        } elseif (($this->initial_data['passtos'] === false) and ($this->validated_data["passtos"] === "yes")) {
            $this->validated_data["passtos"] = "";
        }
    }

    private function __validate_route_no_pull() {
        if (($this->initial_data['route_no_pull'] === true) and ($this->validated_data["route_no_pull"] === "")) {
            $this->validated_data["route_no_pull"] = "yes";
        } elseif (($this->initial_data['route_no_pull'] === false) and ($this->validated_data["route_no_pull"] === "yes")) {
            $this->validated_data["route_no_pull"] = "";
        }
    }

    private function __validate_route_no_exec() {
        if (($this->initial_data['route_no_exec'] === true) and ($this->validated_data["route_no_exec"] === "")) {
            $this->validated_data["route_no_exec"] = "yes";
        } elseif (($this->initial_data['route_no_exec'] === false) and ($this->validated_data["route_no_exec"] === "yes")) {
            $this->validated_data["route_no_exec"] = "";
        }
    }

    private function __validate_dns_add() {
        if (($this->initial_data['dns_add'] === true) and ($this->validated_data["dns_add"] === "")) {
            $this->validated_data["dns_add"] = "yes";
        } elseif (($this->initial_data['dns_add'] === false) and ($this->validated_data["dns_add"] === "yes")) {
            $this->validated_data["dns_add"] = "";
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

    private function __validate_udp_fast_io() {
        if (($this->initial_data['udp_fast_io'] === true) and (empty($this->validated_data["udp_fast_io"]))) {
            $this->validated_data["udp_fast_io"] = "yes";
        } elseif (($this->initial_data['udp_fast_io'] === false) and (!empty($this->validated_data["udp_fast_io"]))) {
            unset($this->validated_data["udp_fast_io"]);
        }
    }

    private function __validate_exit_notify() {
        # Local variables
        $key_types = ["x1", "x2", "x3", "x4", "x5", "disable"];

        # Validate Exit Notify
        if (isset($this->initial_data["exit_notify"])) {
            if (in_array($this->initial_data["exit_notify"], $key_types)) {
                switch ($this->initial_data["exit_notify"]) {
                    case "disable":
                        $this->validated_data["exit_notify"] = "disable";
                        break;
                    case "x1":
                        $this->validated_data["exit_notify"] = "1";
                        break;
                    case "x2":
                        $this->validated_data["exit_notify"] = "2";
                        break;
                    case "x3":
                        $this->validated_data["exit_notify"] = "3";
                        break;
                    case "x4":
                        $this->validated_data["exit_notify"] = "4";
                        break;
                    case "x5":
                        $this->validated_data["exit_notify"] = "5";
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
        $this->__validate_vpnid();
        $this->__validate_mode();
        $this->__validate_description();
        $this->__validate_disabled();
        $this->__validate_dev_mode();
        $this->__validate_protocol();
        $this->__validate_interface();
        $this->__validate_local_port();
        $this->__validate_server_addr();
        $this->__validate_server_port();
        $this->__validate_proxy_addr();
        $this->__validate_proxy_port();
        $this->__validate_proxy_user();
        $this->__validate_proxy_passwd();
        $this->__validate_proxy_authtype();
        $this->__validate_ncp_enable();
        $this->__validate_data_ciphers();
        $this->__validate_data_ciphers_fallback();
        $this->__validate_digest();
        $this->__validate_engine();
        $this->__validate_tunnel_network();
        $this->__validate_tunnel_networkv6();
        $this->__validate_remote_network();
        $this->__validate_remote_networkv6();
        $this->__validate_use_shaper();
        $this->__validate_allow_compression();
        $this->__validate_compression();
        $this->__validate_type_of_service();
        $this->__validate_route_no_exec();
        $this->__validate_dns_add();
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

        # Run each validation method for ["p2p_tls"]
        if ($this->validated_data["mode"] === "p2p_tls") {
            $this->__validate_auth_user();
            $this->__validate_auth_passwd();
            $this->__validate_auth_retry_none();
            $this->__validate_tls();
            $this->__validate_tls_keydir();
            $this->__validate_tls_type();
            $this->__validate_remote_cert_tls();
            $this->__validate_caref();
            $this->__validate_crlref();
            $this->__validate_certref();
            $this->__validate_topology();
            $this->__validate_route_no_pull();
            $this->__validate_inactive_seconds();
            $this->__validate_exit_notify();

            if (isset($this->validated_data["shared_key"])) {
                unset($this->validated_data["shared_key"]);
            }
        }

        # Run each validation method for ["p2p_shared_key"]
        if ($this->validated_data["mode"] === "p2p_shared_key") {
            $this->__validate_shared_key();

            $this->validated_data["auth_user"] = "";
            $this->validated_data["auth_pass"] = "";
            $this->validated_data["auth_retry_none"] = "";
            $this->validated_data["topology"] = "subnet";
            $this->validated_data["route_no_pull"] = "";
            $this->validated_data["inactive_seconds"] = "0";
            $this->validated_data["exit_notify"] = "1";


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

            if (isset($this->validated_data["remote_cert_tls"])) {
                unset($this->validated_data["remote_cert_tls"]);
            }
        }
    }
}
