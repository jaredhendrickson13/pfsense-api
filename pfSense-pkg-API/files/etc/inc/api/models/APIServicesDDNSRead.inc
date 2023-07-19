<?php
//   Copyright 2023 Jared Hendrickson
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


class APIServicesDDNSRead extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
    }

    public function action() {
        # Get currently configured DDNS
        $this->validated_data = $this->get_config('dyndnses/dyndns', []);

        # Loop through each entry and check if it's address is current
        foreach ($this->validated_data as $id=>$ddns_ent) {
            $this->validated_data[$id]["cached_ip"] = $this->__get_cached_ip($ddns_ent);
            $this->validated_data[$id]["cached_ipv6"] = $this->__get_cached_ip($ddns_ent, true);
            $this->validated_data[$id]["current"] = $this->__is_dyndns_current($ddns_ent);
        }

        return APIResponse\get(0, $this->validated_data);
    }

    private function __get_cached_ip($ddns_ent, $ipv6=false) {
        global $dyndns_split_domain_types;

        # Determine our DDNS hostname
        if (in_array($ddns_ent['type'], $dyndns_split_domain_types)) {
            $host = $ddns_ent['host'] . "." . $ddns_ent['domainname'];
        } else {
            $host = $ddns_ent['host'];
        }
        # Set file paths
        $file = "/conf/dyndns_".$ddns_ent['interface'].$ddns_ent['type'].escapeshellarg($host).$ddns_ent['id'].".cache";
        $file_v6 = "/conf/dyndns_".$ddns_ent['interface'].$ddns_ent['type'].escapeshellarg($host).$ddns_ent['id']."_v6.cache";

        # Get our cached IPv4 address
        if (!$ipv6 and file_exists($file)) {
            $cached_ip_s = explode("|", file_get_contents($file));
            return $cached_ip_s[0];
        }
        # Check our cached IPv6 address +
        if ($ipv6 and file_exists($file_v6)) {
            $cached_ipv6_s = explode("|", file_get_contents($file_v6));
            return $cached_ipv6_s[0];
        }
    }

    private function __is_dyndns_current($ddns_ent) {
        # Local variables
        $cached_ip = $this->__get_cached_ip($ddns_ent);
        $cached_ipv6 = $this->__get_cached_ip($ddns_ent, true);

        # Check if we found a cached IPv4 address
        if ($cached_ip) {
            # Check if our interfaces IP matches our cached DDNS IP
            if (dyndnsCheckIP($ddns_ent['interface']) === $cached_ip) {
                return true;
            } else {
                return false;
            }
        }
        # Otherwise, check if we found a cached IPv6 address
        elseif ($cached_ipv6) {
            # Check if our interfaces IPv6 matches our cached DDNS IPv6
            if (get_interface_ipv6($ddns_ent['interface']) === $cached_ipv6) {
                return true;
            } else {
                return false;
            }
        }
        # If no DDNS cache was found, simply return false
        else {
            return false;
        }
    }
}
