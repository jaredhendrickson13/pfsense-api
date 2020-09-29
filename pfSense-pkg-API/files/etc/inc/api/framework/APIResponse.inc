<?php
//   Copyright 2020 Jared Hendrickson
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

namespace APIResponse;

# Pulls a assoc array API response from our response library. Optionally formats descriptive data into messages.
function get($id, $data=[], $all=false) {
    $responses = [
        // 0-999 reserved for system API level responses
        0 => [
            "status" => "ok",
            "code" => 200,
            "return" => $id,
            "message" => "Success",
        ],
        1 => [
            "status" => "server error",
            "code" => 500,
            "return" => $id,
            "message" => "Process encountered unexpected error",
        ],
        2 => [
            "status" => "method not allowed",
            "code" => 405,
            "return" => $id,
            "message" => "Invalid HTTP method",
        ],
        3 => [
            "status" => "unauthorized",
            "code" => 401,
            "return" => $id,
            "message" => "Authentication failed",
        ],
        4 => [
            "status" => "forbidden",
            "code" => 403,
            "return" => $id,
            "message" => "Authorization failed",
        ],
        5 => [
            "status" => "not implemented",
            "code" => 501,
            "return" => $id,
            "message" => "Incompatible pfSense version",
        ],
        6 => [
            "status" => "forbidden",
            "code" => 403,
            "return" => $id,
            "message" => "Requested action is not allowed",
        ],
        7 => [
            "status" => "not found",
            "code" => 404,
            "return" => $id,
            "message" => "Search attribute not found",
        ],
        8 => [
            "status" => "not found",
            "code" => 404,
            "return" => $id,
            "message" => "Could not identify pfSense version",
        ],
        9 => [
            "status" => "forbidden",
            "code" => 403,
            "return" => $id,
            "message" => "Authentication mode must be set to JWT to enable access token authentication",
        ],
        10 => [
            "status" => "server error",
            "code" => "500",
            "return" => $id,
            "message" => "Your API request was valid but no actions were specified for this endpoint",
        ],
        // 1000-1999 reserved for /system API calls
        1000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid system hostname"
        ],
        1001 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid system hostname domain"
        ],
        1002 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System certificate descriptive name required"
        ],
        1003 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid system certificate"
        ],
        1004 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid system certificate key"
        ],
        1005 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System certificate in use"
        ],
        1006 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "ARP IP required"
        ],
        1007 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid system DNS server IP address"
        ],
        1008 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System DNS server IP address required"
        ],
        1009 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Could not locate system certificate"
        ],
        1010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Tunable name is required"
        ],
        1011 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Tunable name must be string type"
        ],
        1012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Tunable name already exists"
        ],
        1013 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Tunable value is required"
        ],
        1014 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Tunable value may only contain alphanumeric characters, -, _, %, and /"
        ],
        1015 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Tunable description must be string type"
        ],
        1016 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Tunable ID is required"
        ],
        1017 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Could not locate tunable with ID"
        ],
        1018 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Table name is required"
        ],
        1019 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Table name does not exist"
        ],

        // 2000-2999 reserved for /services API calls
        2000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid sshd enable value"
        ],
        2001 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid sshd key only mode"
        ],
        2002 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid sshd agent forwarding value"
        ],
        2003 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid sshd port"
        ],
        2004 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override hostname required"
        ],
        2005 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override domain required"
        ],
        2006 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override IP address required"
        ],
        2007 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override alias hostname required"
        ],
        2008 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override alias domain required"
        ],
        2009 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override alias already exists"
        ],
        2010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override already exists"
        ],
        2011 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid unbound host override IP address"
        ],
        2012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override new IP required"
        ],
        2013 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override does not exist"
        ],
        2014 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Host override deletion criteria not met"
        ],
        2015 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override ID required"
        ],
        2016 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override ID does not exist"
        ],
        2017 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd interface is required"
        ],
        2018 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd interface does not exist"
        ],
        2019 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd interface must be enabled and host a static IPv4 subnet"
        ],
        2020 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd start range is required"
        ],
        2021 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd start range must be valid IPv4 address"
        ],
        2022 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd start range is outside of available IP range"
        ],
        2023 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd end range is required"
        ],
        2024 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd end range must be valid IPv4 address"
        ],
        2025 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd end range is outside of available IP range"
        ],
        2026 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Only 4 DHCPd DNS servers are allowed"
        ],
        2027 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd DNS servers must be valid IPv4 addresses"
        ],
        2028 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd gateway must be valid IPv4 address"
        ],
        2029 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd gateway is outside of available IP range"
        ],
        2030 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd domain must be valid hostname"
        ],
        2031 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd domain search list entries must be valid hostnames"
        ],
        2032 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd allowed MAC entries must be partial or full MAC addresses"
        ],
        2033 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd denied MAC entries must be partial or full MAC addresses"
        ],
        2034 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping MAC address is required"
        ],
        2035 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping MAC address is invalid"
        ],
        2036 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping MAC address already exists on this interface"
        ],
        2037 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping IPv4 address is required"
        ],
        2038 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping IPv4 address is invalid"
        ],
        2039 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping IPv4 address already in use"
        ],
        2040 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping hostname is invalid"
        ],
        2041 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping IPv4 cannot match interface address"
        ],
        2042 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping IPv4 must be within interface subnet"
        ],
        2043 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping IPv4 cannot be within existing DHCP pool"
        ],
        2044 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping ID is required"
        ],
        2045 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd static mapping ID does not exist"
        ],

        // 3000-3999 reserved for /interfaces API calls
        3000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown interface ID"
        ],
        3001 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface ID in use"
        ],
        3002 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface ID required"
        ],
        3003 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface MAC address"
        ],
        3004 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface MTU value"
        ],
        3005 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface MSS value"
        ],
        3006 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface MSS value greater than parent interface"
        ],
        3007 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface speed and duplex"
        ],
        3008 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface descriptive name already in use"
        ],
        3009 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface IPv4 address already in use"
        ],
        3010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv4 address"
        ],
        3011 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface type staticv4 requires IPv4 address"
        ],
        3012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv4 subnet"
        ],
        3013 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface type staticv4 requires subnet bitmask"
        ],
        3014 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv4 gateway"
        ],
        3015 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP alias address"
        ],
        3016 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP reject from address"
        ],
        3017 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP timeout value"
        ],
        3018 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP retry value"
        ],
        3019 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP timeout value"
        ],
        3020 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP reboot value"
        ],
        3021 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP backoff cutoff value"
        ],
        3022 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP initial interval value"
        ],
        3023 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP configuration file"
        ],
        3024 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP VLAN priority value"
        ],
        3025 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv4 configuration type"
        ],
        3026 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv6 address"
        ],
        3027 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface IPv6 address already in use"
        ],
        3028 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface staticv6 requires IPv6 address"
        ],
        3029 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv6 subnet"
        ],
        3030 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface type staticv6 requires subnet bitmask"
        ],
        3031 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv6 gateway"
        ],
        3032 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv6 prefix delegation size"
        ],
        3033 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP6 VLAN priority value"
        ],
        3034 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface DHCP6 configuration file"
        ],
        3035 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface 6RD gateway address"
        ],
        3036 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface 6RD gateway address required"
        ],
        3037 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface 6RD prefix length"
        ],
        3038 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv6 track interface"
        ],
        3039 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface IPv6 track interface required"
        ],
        3040 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv6 track prefix hex value"
        ],
        3041 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface IPv6 configuration type"
        ],
        3042 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface WAN cannot be deleted"
        ],
        3043 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface cannot be deleted while member of interface group"
        ],
        3044 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface cannot be deleted while member of bridge interface"
        ],
        3045 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface cannot be deleted while member of GRE tunnel"
        ],
        3046 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface cannot be deleted while member of GIF tunnel"
        ],
        3047 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface cannot be deleted with existing traffic shaper configuration"
        ],
        3048 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface VLAN tag required"
        ],
        3049 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface VLAN tag in use"
        ],
        3050 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface VLAN does not exist"
        ],
        3051 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface VLAN parent does not exist"
        ],
        3052 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface VLAN tag"
        ],
        3053 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid interface VLAN priority value"
        ],
        3054 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface VLAN already exists on parent"
        ],
        3055 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface VLAN parent required"
        ],

        // 4000-4999 reserved for /firewall API calls
        4000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward interface required"
        ],
        4001=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward rule protocol required"
        ],
        4002 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward redirect IP required"
        ],
        4003 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward redirect port required"
        ],
        4004 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward source required"
        ],
        4005 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward destination required"
        ],
        4006=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown port forward interface"
        ],
        4007 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown port forward protocol"
        ],
        4008=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown port forward NAT reflection value"
        ],
        4009 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward redirect IP"
        ],
        4010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward redirect port"
        ],
        4011 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward source address"
        ],
        4012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward destination address"
        ],
        4013 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward source port"
        ],
        4014 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid port forward destination port"
        ],
        4015 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward rule ID required"
        ],
        4016 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Port forward rule ID does not exist"
        ],
        4017 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP configuration ID required"
        ],
        4018 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP configuration ID does not exist"
        ],
        4019 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP mode required"
        ],
        4020 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP interface required"
        ],
        4021 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP subnet required"
        ],
        4022 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP CARP password required"
        ],
        4023 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown virtual IP mode"
        ],
        4024 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown virtual IP interface"
        ],
        4025 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP subnet"
        ],
        4026 =>[
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP subnet already in use"
        ],
        4027 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Virtual IP VHID already in use"
        ],
        4028 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP VHID"
        ],
        4029 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP CARP advertisement base"
        ],
        4030 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid virtual IP CARP advertisement skew"
        ],
        4031 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule tracker ID required"
        ],
        4032 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule tracker ID does not exist"
        ],
        4033 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule type required"
        ],
        4034 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule interface required"
        ],
        4035 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule IP version required"
        ],
        4036 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule protocol required"
        ],
        4037 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule source required"
        ],
        4038 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule destination required"
        ],
        4039 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule type"
        ],
        4040 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule interface"
        ],
        4041 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule IP version"
        ],
        4042 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule protocol"
        ],
        4043 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule gateway"
        ],
        4044 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall rule source"
        ],
        4045 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall rule destination"
        ],
        4046 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule ICMP subtype"
        ],
        4047 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule source and destination port required"
        ],
        4048 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall rule source port"
        ],
        4049 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall rule destination port"
        ],
        4050 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias name required"
        ],
        4051 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias in use"
        ],
        4052 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias address value required"
        ],
        4053 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias name must be type string"
        ],
        4054 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias address must be type array"
        ],
        4055 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias does not exist"
        ],
        4056 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias already exists"
        ],
        4057 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall alias type"
        ],
        4058 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall alias IP address or hostname"
        ],
        4059 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall alias subnet"
        ],
        4060 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall alias port"
        ],
        4061 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias type required"
        ],
        4062 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias type must be type string"
        ],
        4063 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias description must be type string"
        ],
        4064 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias detail must be type array"
        ],
        4065 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall alias port value"
        ],
        4066 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias port values must be numeric"
        ],
        4067 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall alias IP address"
        ],
        4068 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall alias subnet"
        ],
        4069 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias address values must be valid CIDR"
        ],
        4070 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Alias address contents must be type string"
        ],
        4071 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Alias detail contents must be type string"
        ],
        4072 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall state maximum size required"
        ],
        4073 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid firewall state table size"
        ],
        4074 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias id required"
        ],
        //5000-5999 reserved for /users API calls
        5000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Username required"
        ],
        5001 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User does not exist"
        ],
        5002 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User already exists"
        ],
        5003 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User password required"
        ],
        5004 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User privilege required"
        ],
        5005 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User privilege must be type array or string"
        ],
        5006 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown user privilege"
        ],
        5007 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User group name required"
        ],
        5008 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown group name"
        ],
        5009 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Group must be type array or string"
        ],
        5010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User authentication server name required"
        ],
        5011 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "LDAP authentication server hostname or IP required"
        ],
        5012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid LDAP authentication server hostname or IP"
        ],
        5013 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP port required"
        ],
        5014 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid authentication server LDAP port"
        ],
        5015 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP URL type required"
        ],
        5016 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid authentication server LDAP URL type"
        ],
        5017 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid authentication server LDAP protocol version"
        ],
        5018 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP search scope required"
        ],
        5019 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid authentication server LDAP search scope"
        ],
        5020 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP bind DN required"
        ],
        5021 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP bind password required"
        ],
        5022 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP user naming attribute required"
        ],
        5023 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP group naming attribute required"
        ],
        5024 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server LDAP group member attribute required"
        ],
        5025 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Could not locate authentication server"
        ],
        5026 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server name already in use"
        ],
        //6000-6999 reserved for /routing API calls
        6000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Static route network value is required"
        ],
        6001 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Static route network value must be valid IPv4, IPv6 or alias"
        ],
        6002 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Static route network subnet out of range"
        ],
        6003 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Static route gateway value is required"
        ],
        6004 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Static route gateway does not exist"
        ],
        6005 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Static route ID is required"
        ],
        6006 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Static route ID does not exist"
        ],

    ];
    $response = $responses[(!in_array($id, $responses)) ? $id : 1];
    $response["data"] = $data;
    if ($all === true) {
        $response = $responses;
    }
    return $response;
}
