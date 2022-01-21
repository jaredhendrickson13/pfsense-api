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
            "code" => 500,
            "return" => $id,
            "message" => "Your API request was valid but no actions were specified for this endpoint",
        ],
        11 => [
            "status" => "unsupported media type",
            "code" => 415,
            "return" => $id,
            "message" => "Specified content type is not supported"
        ],
        12 => [
            "status" => "server error",
            "code" => 500,
            "return" => $id,
            "message" => "API endpoint requires one or more add-on packages to be installed before accepting requests",
        ],
        13 => [
            "status" => "bad gateway",
            "code" => 502,
            "return" => $id,
            "message" => "Failed to connect to upstream package repositories",
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
        1020 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown API interface specified in allowed interfaces"
        ],
        1021 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown API authentication mode"
        ],
        1022 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "API JWT expiration threshold must be between 300 and 86400"
        ],
        1023 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown API token hash algorithm"
        ],
        1024 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported API token bytes count"
        ],
        1025 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "API custom headers must be an array containing key-value pairs"
        ],
        1026 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "API custom header key-value pairs must be string types"
        ],
        1027 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "At least one HA sync host must be specified when HA sync is enabled"
        ],
        1028 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "HA sync hosts must be valid IP address or hostname"
        ],
        1029 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "An HA sync username is required when HA sync is enabled"
        ],
        1030 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "An HA sync password is required when HA sync is enabled"
        ],
        1031 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate management method is required"
        ],
        1032 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported certificate management method specified"
        ],
        1033 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate serial number must be 1 or greater"
        ],
        1034 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate cannot be deleted while in use"
        ],
        1035 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate does not appear to be a CA certificate"
        ],
        1036 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Encrypted private keys are not supported"
        ],
        1037 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate description contains invalid characters"
        ],
        1038 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate key type is required"
        ],
        1039 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported certificate key type specified"
        ],
        1040 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate key length is required"
        ],
        1041 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported certificate key length specified"
        ],
        1042 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate key elliptic curve name is required"
        ],
        1043 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported certificate elliptic curve name specified"
        ],
        1044 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate digest algorithm is required"
        ],
        1045 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported certificate digest algorithm specified"
        ],
        1046 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certifiate lifetime is greater than allowed maximum"
        ],
        1047 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Signing CA reference ID is required"
        ],
        1048 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Signing CA reference ID does not exist"
        ],
        1049 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate and private key are not matching keypair"
        ],
        1050 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "An error occurred attempting to generate certificate. Please check your DN values."
        ],
        1051 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown certificate country"
        ],
        1052 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate common name is required"
        ],
        1053 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate type is required"
        ],
        1054 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate type is invalid"
        ],
        1055 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate alternative names must be contained within an array"
        ],
        1056 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate alternative name type is invalid"
        ],
        1057 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate DNS alternative names must be valid hostname or FQDN"
        ],
        1058 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate IP alternative names must be valid IP address"
        ],
        1059 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate URI alternative names must be valid URI"
        ],
        1060 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Certificate email alternative names must be valid email address"
        ],
        1061 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification IP address is required"
        ],
        1062 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification IP address must be valid IP address or hostname"
        ],
        1063 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification port is required"
        ],
        1064 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification SMTP port must be between 1 and 65535"
        ],
        1065 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification timeout must be a number greater than 0"
        ],
        1066 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification From Email Address is required"
        ],
        1067 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification From Email Address must be valid email address"
        ],
        1068 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification Notify Email Address is required"
        ],
        1069 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification Notify Email Address must be valid email address"
        ],
        1070 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification authentication mechanism is required"
        ],
        1071 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System email notification authentication mechanism must be PLAIN or LOGIN"
        ],
        1072 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System API access list must be array of IPv4 or IPv6 subnets in CIDR notation"
        ],
        1073 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System package name is required"
        ],
        1074 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Only pfSense packages are deletable. Ensure your package is prefixed with .pfSense-pkg-'."
        ],
        1075 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System package with this name could not be found"
        ],
        1076 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System package with this name is already installed"
        ],
        1077 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Remote URL does not contain a pfSense package"
        ],
        1078 => [
            "status" => "bad request",
            "code" => 504,
            "return" => $id,
            "message" => "System package installation exceeded timeout"
        ],
        1079 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "System package installation timeout cannot be greater than 120 seconds"
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
            "message" => "Unbound host override alias already exists with this IP address type"
        ],
        2010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound host override already exists with this IP address type"
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
        2046 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid unbound host value"
        ],
        2047 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid unbound domain value"
        ],
        2048 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "NTPd orphan value must be between 1 and 15"
        ],
        2049 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown NTPd interface specified"
        ],
        2050 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "NTPd timeserver value is required"
        ],
        2051 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "NTPd timeserver must be valid IP address or hostname"
        ],
        2052 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Maximum limit of 10 NTPd timeservers exceeded"
        ],
        2053 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "NTPd timeserver does not exist"
        ],
        2054 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override already exists with this IP address type"
        ],
        2055 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override host value required"
        ],
        2056 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid dnsmasq host value"
        ],
        2057 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override domain value required"
        ],
        2058 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid dnsmasq domain value"
        ],
        2059 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override IP value required"
        ],
        2060 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid dnsmasq host override IP value"
        ],
        2061 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override ID required"
        ],
        2062 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override ID does not exist"
        ],
        2063 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override alias hostname required"
        ],
        2064 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override alias domain required"
        ],
        2065 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Dnsmasq host override alias already exists with this IP address type"
        ],
        2066 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound access list action required"
        ],
        2067 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown unbound access list action"
        ],
        2068 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound access list network required"
        ],
        2069 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound access list network must be valid IPv4 or IPv6 address"
        ],
        2070 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound access list network mask must be between 0 and 32 for IPv4 networks or 0 and 128 for IPv6 networks"
        ],
        2071 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound access list network mask is required"
        ],
        2072 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound access list ID is required"
        ],
        2073 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Minimum of 1 unbound access list row is required"
        ],
        2074 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unbound access list does not exist"
        ],
        2075 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "OpenVPN client specific override parameter 'common_name' is missing"
        ],
        2076 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown OpenVPN client specific override Netbios Node Type. Valid options are: b, p, m, h"
        ],
        2077 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Missing OpenVPN client specific override parameter 'refid'. This parameter is needed to identify the csc to delete."
        ],
        2078 => [
            "status" => "Not found",
            "code" => 404,
            "return" => $id,
            "message" => "Unknown OpenVPN client specific override refid."
        ],
        2079 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Missing OpenVPN client specific override parameter 'refid'. This parameter is needed to identify the csc to modify."
        ],
        2080 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd default lease time must be a value greater than 60"
        ],
        2081 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd default lease time cannot be greater than the maximum lease time"
        ],
        2082 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd max lease time must be a value greater than 60"
        ],
        2083 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option number is required"
        ],
        2084 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option number must be a number between 1 and 254"
        ],
        2085 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option type is required"
        ],
        2086 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported DHCPd option type specified"
        ],
        2087 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option value is required"
        ],
        2088 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type text cannot contain quotation marks"
        ],
        2089 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type string must be enclosed in quotations or a hexidecimal value"
        ],
        2090 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type boolean must be true, false, on, or off"
        ],
        2091 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type unsigned integer 8 must be between 0 and 255"
        ],
        2092 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type unsigned integer 16 must be between 0 and 65535"
        ],
        2093 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type unsigned integer 32 must be between 0 and 4294967295"
        ],
        2094 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type signed integer 8 must be between -128 and 127"
        ],
        2095 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type signed integer 16 must be between -32768 and 32767"
        ],
        2096 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type signed integer 32 must be between -2147483648 and 2147483647"
        ],
        2097 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "DHCPd option with type ip-address must be valid IPv4 address or hostname"
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
        3056 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface VLAN ID required"
        ],
        3057 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface ID is required"
        ],
        3058 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface ID does not exist"
        ],
        3059 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface descriptive name cannot start with 'pkg_'"
        ],
        3060 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface descriptive name cannot match alias name"
        ],
        3061 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface descriptive name cannot be entirely numeric"
        ],
        3062 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface descriptive name cannot match existing interface name"
        ],
        3063 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface type cannot be changed while DHCP server is enabled"
        ],
        3064 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface subnet must be less than 31 while DHCP server is enabled"
        ],
        3065 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface type6 cannot be changed while DHCPv6 server is enabled"
        ],
        3066 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge must contain at least 1 member interface"
        ],
        3067 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge member does not exist"
        ],
        3068 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge member is already a member of another bridge"
        ],
        3069 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge member must be in hostap mode if wireless"
        ],
        3070 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge member cannot use captive portal"
        ],
        3071 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge ID is required"
        ],
        3072 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge does not exist"
        ],
        3073 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Interface bridge cannot be deleted while it's in use"
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
            "message" => "Invalid firewall alias name"
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
            "message" => "Invalid firewall alias port or port range"
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
        4075 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "1:1 NAT interface is required"
        ],
        4076=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown 1:1 NAT interface"
        ],
        4077 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "1:1 NAT source required"
        ],
        4078 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "1:1 NAT destination required"
        ],
        4079 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid 1:1 NAT source address"
        ],
        4080 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid 1:1 NAT destination address"
        ],
        4081 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "1:1 NAT external address required"
        ],
        4082 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid 1:1 NAT external address"
        ],
        4083 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "1:1 NAT rule ID required"
        ],
        4084 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "1:1 NAT rule ID does not exist"
        ],
        4085 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid outbound NAT mode"
        ],
        4086 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping interface is required"
        ],
        4087 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown outbound NAT mapping interface"
        ],
        4088=> [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping protocol required"
        ],
        4089 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown outbound NAT mapping protocol"
        ],
        4090 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown outbound NAT mapping pool option"
        ],
        4091 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping source hash key must start with 0x"
        ],
        4092 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping source hash key must contain a hexadecimal value after 0x"
        ],
        4093 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping source hash key must be 34 characters in length including 0x"
        ],
        4094 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping target is required"
        ],
        4095 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid outbound NAT mapping target"
        ],
        4096 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping pool option must be default or round robin when using an alias target"
        ],
        4097 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid outbound NAT port"
        ],
        4098 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping source required"
        ],
        4099 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid outbound NAT source address"
        ],
        4100 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping destination required"
        ],
        4101 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid outbound NAT destination address"
        ],
        4102 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid outbound NAT source port"
        ],
        4103 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid outbound NAT destination port"
        ],
        4104 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping ID required"
        ],
        4105 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Outbound NAT mapping ID does not exist"
        ],
        4106 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias details cannot contain more items than alias addresses"
        ],
        4107 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias type cannot be changed while in use"
        ],
        4108 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall alias cannot be deleted while in use"
        ],
        4109 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule gateway must match IP protocol"
        ],
        4110 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper interface is required"
        ],
        4111 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall traffic shaper interface"
        ],
        4112 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper already exists for this interface"
        ],
        4113 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper scheduler is required"
        ],
        4114 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall traffic shaper scheduler"
        ],
        4115 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper bandwidth type is required"
        ],
        4116 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall traffic shaper bandwidth type"
        ],
        4117 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper bandwidth is required"
        ],
        4118 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper bandwidth must be 1 or greater"
        ],
        4119 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper bandwidth must be 100 or less with % bandwidth type"
        ],
        4120 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue limit must be 1 or greater"
        ],
        4121 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper TBR size must be 1 or greater"
        ],
        4122 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper does not exist for this interface"
        ],
        4123 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue name is required"
        ],
        4124 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue name must only contain alphanumerics, underscore, and hyphens"
        ],
        4125 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue name must be between 1 and 15 characters"
        ],
        4126 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue name already exists for this interface"
        ],
        4127 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue priority must be between 0 and 7 for CBQ and FAIRQ schedulers"
        ],
        4128 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue priority must be between 0 and 15 for PRIQ schedulers"
        ],
        4129 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Sum of firewall traffic shaper queues bandwidth exceeds parent's bandwidth limits"
        ],
        4130 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue upperlimit1 value must be a bandwidth formatted string"
        ],
        4131 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue upperlimit2 value is required"
        ],
        4132 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue upperlimit2 must be 1 or greater"
        ],
        4133 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue upperlimit3 value is required"
        ],
        4134 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue upperlimit3 value must be a bandwidth formatted string"
        ],
        4135 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue linkshare1 value must be a bandwidth formatted string"
        ],
        4136 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue linkshare2 value is required"
        ],
        4137 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue linkshare2 must be 1 or greater"
        ],
        4138 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue linkshare3 value is required"
        ],
        4139 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue linkshare3 value must be a bandwidth formatted string"
        ],
        4140 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue realtime1 value must be a bandwidth formatted string"
        ],
        4141 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue realtime2 value is required"
        ],
        4142 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue realtime2 must be 1 or greater"
        ],
        4143 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue realtime3 value is required"
        ],
        4144 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue realtime3 value must be a bandwidth formatted string"
        ],
        4145 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper queue name does not exist"
        ],
        4146 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule name is required"
        ],
        4147 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule name can only contain alphanumerics, underscores and hyphens"
        ],
        4148 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule name must 32 characters or less"
        ],
        4149 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule name already in use"
        ],
        4150 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule name does not exist"
        ],
        4151 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range month is required"
        ],
        4152 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range months must be between 1 and 12"
        ],
        4153 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range day is required"
        ],
        4154 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range number of days must match number of months"
        ],
        4155 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range days out of range for month"
        ],
        4156 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range hour is required"
        ],
        4157 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range starting hour is invalid"
        ],
        4158 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range ending hour is invalid"
        ],
        4159 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range starting hour cannot be greater than the ending hour"
        ],
        4160 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range positions must be between 1 and 7"
        ],
        4161 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range is required"
        ],
        4162 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "At least one firewall schedule time range is required"
        ],
        4163 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range ID is required"
        ],
        4164 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall schedule time range ID does not exist"
        ],
        4165 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Cannot delete the last remaining firewall schedule time range"
        ],
        4166 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Cannot delete firewall schedule while it is in use"
        ],
        4167 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter name is required"
        ],
        4168 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter name can only contain alphanumerics, hyphens and underscores"
        ],
        4169 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter name must be 32 characters or less"
        ],
        4170 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter name is already in use"
        ],
        4171 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall traffic shaper limiter mask"
        ],
        4172 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter mask bits must be numeric value between 1 and 32"
        ],
        4173 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter IPv6 mask bits must be numeric value between 1 and 128"
        ],
        4174 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter queue management algorithm (AQM) is required"
        ],
        4175 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall traffic shaper limiter queue management algorithm (AQM)"
        ],
        4176 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter ECN requested but neither the AQM nor scheduler support it"
        ],
        4177 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter delay must be a numeric value between 0 and 10000"
        ],
        4178 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter packet loss rate must be a numeric value between 0 and 1"
        ],
        4179 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter buckets must be a numeric value between 16 and 65535"
        ],
        4180 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter codel target parameter must be 0 or greater"
        ],
        4181 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter codel interval parameter must be 0 or greater"
        ],
        4182 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter pie target parameter must be 0 or greater"
        ],
        4183 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter pie tupdate parameter must be 0 or greater"
        ],
        4184 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter pie alpha parameter must be 0 or greater"
        ],
        4185 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter pie beta parameter must be 0 or greater"
        ],
        4186 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter pie max burst parameter must be 0 or greater"
        ],
        4187 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter pie max ecnth parameter must be 0 or greater"
        ],
        4188 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter red w_q parameter must be 1 or greater"
        ],
        4189 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter red min th parameter must be 0 or greater"
        ],
        4190 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter red max th parameter must be 1 or greater"
        ],
        4191 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter red max p parameter must be 1 or greater"
        ],
        4192 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter gred w_q parameter must be 1 or greater"
        ],
        4193 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter gred min th parameter must be 0 or greater"
        ],
        4194 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter gred max th parameter must be 1 or greater"
        ],
        4195 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter gred max p parameter must be 1 or greater"
        ],
        4196 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_codel target parameter must be 0 or greater"
        ],
        4197 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_codel interval parameter must be 0 or greater"
        ],
        4198 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_codel quantum parameter must be 0 or greater"
        ],
        4199 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_codel limit parameter must be 0 or greater"
        ],
        4200 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_codel flows parameter must be 0 or greater"
        ],
        4201 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_pie target parameter must be 0 or greater"
        ],
        4202 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_pie tupdate parameter must be 0 or greater"
        ],
        4203 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_pie alpha parameter must be 0 or greater"
        ],
        4204 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_pie beta parameter must be 0 or greater"
        ],
        4205 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_pie max burst parameter must be 0 or greater"
        ],
        4206 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter fq_pie max ecnth parameter must be 0 or greater"
        ],
        4207 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter bandwidth is required"
        ],
        4208 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "At least 1 bandwidth schedule object is required"
        ],
        4209 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter with this name does not exist"
        ],
        4210 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter bandwidth is required"
        ],
        4211 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter bandwidth must be 1 or greater"
        ],
        4212 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter bandwidth scale is required"
        ],
        4213 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall traffic shaper limiter bandwidth scale"
        ],
        4214 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter and child queues cannot be deleted while in use"
        ],
        4215 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter bandwidth ID required"
        ],
        4216 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter bandwidth ID does not exist"
        ],
        4217 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter queue weight must be between 1 and 100"
        ],
        4218 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper parent limiter required"
        ],
        4219 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper parent limiter does not exist"
        ],
        4220 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter queue name required"
        ],
        4221 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper limiter queue name does not exist"
        ],
        4222 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule dnpipe or pdnpipe"
        ],
        4223 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule dnpipe is required for pdnpipe"
        ],
        4224 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule dnpipe and pdnpipe queues cannot be the same"
        ],
        4225 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule dnpipe and pdnpipe queue and a virtual interface cannot be selected"
        ],
        4226 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule default queue"
        ],
        4227 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule default queue required for ackqueue"
        ],
        4228 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule ackqueue"
        ],
        4229 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule default queue and ackqueue cannot be the same"
        ],
        4230 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall traffic shaper child queue cannot be deleted while in use"
        ],
        4231 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall states source is required"
        ],
        4232 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall states source must be valid IP address or CIDR"
        ],
        4233 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall states destination must be valid IP address or CIDR"
        ],
        4234 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall states protocol must be tcp, udp, icmp, or icmpv6"
        ],
        4235 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall states interface does not exist"
        ],
        4236 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall states sleep value must be a numeric value between 0 and 300"
        ],
        4237 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Firewall rule sorting field is required"
        ],
        4238 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown firewall rule sorting option"
        ],
        4239 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown API floating rule direction specified"
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
            "message" => "System users cannot be deleted"
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
            "message" => "Authentication server hostname or IP required"
        ],
        5012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid authentication server hostname or IP"
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
        5027 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unsupported RADIUS protocol"
        ],
        5028 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "RADIUS secret required"
        ],
        5029 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "RADIUS secret must be string type"
        ],
        5030 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid RADIUS authentication port"
        ],
        5031 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid RADIUS accounting port"
        ],
        5032 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "RADIUS authentication and/or accounting port required"
        ],
        5033 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "RADIUS timeout must be an integer of 1 or greater"
        ],
        5034 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Unknown RADIUS NAS-IP attribute"
        ],
        5035 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Authentication server name is required"
        ],
        5036 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid characters in username"
        ],
        5037 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Username is reserved by the system"
        ],
        5038 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Username cannot contain more than 32 characters"
        ],
        5039 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid characters is IPsec PSK"
        ],
        5040 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User expiration date must be in MM/DD/YYYY format"
        ],
        5041 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "User certificate does not exist"
        ],
        // 6000-6999 reserved for /routing API calls
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
        6007 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway interface is required"
        ],
        6008 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid gateway interface"
        ],
        6009 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway IP protocol is required"
        ],
        6010 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid gateway IP protocol"
        ],
        6011 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway name is required"
        ],
        6012 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid gateway name"
        ],
        6013 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway address is required"
        ],
        6014 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid gateway address"
        ],
        6015 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway weight must be between 1 and 30"
        ],
        6016 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway data payload must be greater than 1"
        ],
        6017 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway low latency threshold must be positive integer"
        ],
        6018 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway high latency threshold must be greater than low latency threshold"
        ],
        6019 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway low packet loss threshold must be between 1 and 100"
        ],
        6020 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway high packet loss threshold must be between the low threshold and 100"
        ],
        6021 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway probe interval must be between 1 and 3600000"
        ],
        6022 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway loss interval must be greater than the high latency threshold"
        ],
        6023 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway time period must be greater than twice the probe interval plus the loss interval"
        ],
        6024 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway alert interval must be greater than or greater to the probe interval"
        ],
        6025 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Invalid gateway monitor address"
        ],
        6026 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway name already exists"
        ],
        6027 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway ID required"
        ],
        6028 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Gateway ID does not exist"
        ],
        6029 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Only 1 dynamic gateway per eligible interface is allowed"
        ],

        // 7000-7999 reserved for /diagnostics API calls
        7000 => [
            "status" => "bad request",
            "code" => 400,
            "return" => $id,
            "message" => "Shell command is required"
        ],



    ];
    $response = $responses[(array_key_exists($id, $responses)) ? $id : 1];
    $response["data"] = $data;
    if ($all === true) {
        $response = $responses;
    }
    return $response;
}
