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

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIServicesDDNSRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dynamicdnsclients"];
    }

    public function action() {
        # Check that we have at least 1 DDNS configuration, otherwise return empty array
        if (!empty($this->config['dyndnses']['dyndns'])) {
            $this->validated_data = $this->config['dyndnses']['dyndns'];
            # Loop through each entry and check if it's address is current
            foreach ($this->validated_data as $id=>$dyndns_ent) {
                $this->validated_data[$id]["current"] = $this->__is_dyndns_current($dyndns_ent);
            }
        } else {
            $this->validated_data = [];
        }
        return APIResponse\get(0, $this->validated_data);
    }

    private function __is_dyndns_current($ddns_ent) {
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

        # Check if either the IPv4 or IPv6 files exist, otherwise return false.
        if (file_exists($file)) {
            $ipaddr = dyndnsCheckIP($ddns_ent['interface']);
            $cached_ip_s = explode("|", file_get_contents($file));
            $cached_ip = $cached_ip_s[0];
            if ($ipaddr != $cached_ip) {
                return false;
            } else {
                return true;
            }
        } else if (file_exists($file_v6)) {
            $ipv6addr = get_interface_ipv6($ddns_ent['interface']);
            $cached_ipv6_s = explode("|", file_get_contents($file_v6));
            $cached_ipv6 = $cached_ipv6_s[0];
            if ($ipv6addr != $cached_ipv6) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }
}