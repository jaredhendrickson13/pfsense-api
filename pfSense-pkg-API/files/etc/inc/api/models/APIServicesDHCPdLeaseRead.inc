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


class APIServicesDHCPdLeaseRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-dhcpleases"];
    }

    public function action() {
        # For pfSense 2.5+, this process was optimized into a single function. Simply call that function if 2.5.
        # TODO: once 2.4 version of this pkg is deprecated, remove version condition and simply call built-in function
        if (APITools\get_pfsense_version()["program"] >= 250) {
            return APIResponse\get(0, $this->__to_representation(system_get_dhcpleases()["lease"]));
        } else {
            return APIResponse\get(0, $this->__to_representation($this->__get_dhcp_leases()["lease"]));
        }
    }

    # Formats the leases array to be more REST like
    private function __to_representation($leases) {
        # Loop through each array item and change the online/offline keywords to True/False accordingly
        # Also change the 'act' key to 'state' to prevent breaking v1.2.x (can be removed for v1.3.x)
        foreach ($leases as $id=>$lease) {
            # Change the 'online' keyword to true
            if ($lease["online"] === "online") {
                $leases[$id]["online"] = true;
            }
            # Otherwise change the 'offline' keyword to false
            else {
                $leases[$id]["online"] = false;
            }

            # Change the 'act' key to 'state'
            $leases[$id]["state"] = $leases[$id]["act"];
            unset($leases[$id]["act"]);
        }

        return $leases;
    }

    private function __get_arp_table($resolve_hostnames = false) {
        $params="-a";
        if (!$resolve_hostnames) {
            $params .= "n";
        }

        $arp_table = array();
        $_gb = exec("/usr/sbin/arp --libxo json {$params}", $rawdata, $rc);
        if ($rc == 0) {
            $arp_table = json_decode(implode(" ", $rawdata),
                JSON_OBJECT_AS_ARRAY);
            if ($rc == 0) {
                $arp_table = $arp_table['arp']['arp-cache'];
            }
        }

        return $arp_table;
    }

    private function __get_dhcp_leases() {
        global $g;

        # Variables
        $item = array();
        $leases = array();
        $leases['lease'] = array();
        $leases['failover'] = array();

        $online_string = gettext("online");
        $offline_string = gettext("offline");
        $active_string = gettext("active");
        $expired_string = gettext("expired");
        $reserved_string = gettext("reserved");
        $dynamic_string = gettext("dynamic");
        $static_string = gettext("static");

        $lease_regex = '/^lease\s+([^\s]+)\s+{$/';
        $starts_regex = '/^\s*(starts|ends)\s+\d+\s+([\d\/]+|never)\s*(|[\d:]*);$/';
        $binding_regex = '/^\s*binding\s+state\s+(.+);$/';
        $mac_regex = '/^\s*hardware\s+ethernet\s+(.+);$/';
        $hostname_regex = '/^\s*client-hostname\s+"(.+)";$/';

        $failover_regex = '/^failover\s+peer\s+"(.+)"\s+state\s+{$/';
        $state_regex = '/\s*(my|partner)\s+state\s+(.+)\s+at\s+\d+\s+([\d\/]+)\s+([\d:]+);$/';

        $lease = false;
        $failover = false;
        $dedup_lease = false;
        $dedup_failover = false;

        $leases_file = "{$g['dhcpd_chroot_path']}/var/db/dhcpd.leases";

        if (!file_exists($leases_file)) {
            return $leases;
        }

        $leases_content = file($leases_file, FILE_IGNORE_NEW_LINES |
            FILE_IGNORE_NEW_LINES);

        if ($leases_content === FALSE) {
            return $leases;
        }

        $arp_table = $this->__get_arp_table();

        $arpdata_ip = array();
        $arpdata_mac = array();
        foreach ($arp_table as $arp_entry) {
            if (isset($arpentry['incomplete'])) {
                continue;
            }
            $arpdata_ip[] = $arp_entry['ip-address'];
            $arpdata_mac[] = $arp_entry['mac-address'];
        }
        unset($arp_table);


        foreach ($leases_content as $line) {
            /* Skip comments */
            if (preg_match('/^\s*(|#.*)$/', $line)) {
                continue;
            }

            if (preg_match('/}$/', $line)) {
                if ($lease) {
                    if (empty($item['hostname'])) {
                        $hostname = gethostbyaddr($item['ip']);
                        if (!empty($hostname)) {
                            $item['hostname'] = $hostname;
                        }
                    }
                    $leases['lease'][] = $item;
                    $lease = false;
                    $dedup_lease = true;
                } else if ($failover) {
                    $leases['failover'][] = $item;
                    $failover = false;
                    $dedup_failover = true;
                }
                continue;
            }

            if (preg_match($lease_regex, $line, $m)) {
                $lease = true;
                $item = array();
                $item['ip'] = $m[1];
                $item['type'] = $dynamic_string;
                continue;
            }

            if ($lease) {
                if (preg_match($starts_regex, $line, $m)) {
                    /*
                     * Quote from dhcpd.leases(5) man page:
                     * If a lease will never expire, date is never
                     * instead of an actual date
                     */
                    if ($m[2] == "never") {
                        $item[$m[1]] = gettext("Never");
                    } else {
                        $item[$m[1]] = $this->__adjust_dhcpd_datetime(
                            $m[2] . ' ' . $m[3]);
                    }
                    continue;
                }

                if (preg_match($binding_regex, $line, $m)) {
                    switch ($m[1]) {
                        case "active":
                            $item['act'] = $active_string;
                            break;
                        case "free":
                            $item['act'] = $expired_string;
                            $item['online'] =
                                $offline_string;
                            break;
                        case "backup":
                            $item['act'] = $reserved_string;
                            $item['online'] =
                                $offline_string;
                            break;
                    }
                    continue;
                }

                if (preg_match($mac_regex, $line, $m) &&
                    is_macaddr($m[1])) {
                    $item['mac'] = $m[1];

                    if (in_array($item['ip'], $arpdata_ip)) {
                        $item['online'] = $online_string;
                    } else {
                        $item['online'] = $offline_string;
                    }
                    continue;
                }

                if (preg_match($hostname_regex, $line, $m)) {
                    $item['hostname'] = $m[1];
                }
            }

            if (preg_match($failover_regex, $line, $m)) {
                $failover = true;
                $item = array();
                $item['name'] = $m[1] . ' (' .
                    convert_friendly_interface_to_friendly_descr(
                        substr($m[1],5)) . ')';
                continue;
            }

            if ($failover && preg_match($state_regex, $line, $m)) {
                $item[$m[1] . 'state'] = $m[2];
                $item[$m[1] . 'date'] = $this->__adjust_dhcpd_datetime($m[3] .
                    ' ' . $m[4]);
                continue;
            }
        }

        foreach ($this->config['interfaces'] as $ifname => $ifarr) {
            if (!is_array($this->config['dhcpd'][$ifname]) ||
                !is_array($this->config['dhcpd'][$ifname]['staticmap'])) {
                continue;
            }

            foreach ($this->config['dhcpd'][$ifname]['staticmap'] as $idx =>
                     $static) {
                if (empty($static['mac']) && empty($static['cid'])) {
                    continue;
                }

                $slease = array();
                $slease['ip'] = $static['ipaddr'];
                $slease['type'] = $static_string;
                if (!empty($static['cid'])) {
                    $slease['cid'] = $static['cid'];
                }
                $slease['mac'] = $static['mac'];
                $slease['if'] = $ifname;
                $slease['starts'] = "";
                $slease['ends'] = "";
                $slease['hostname'] = $static['hostname'];
                $slease['descr'] = $static['descr'];
                $slease['act'] = $static_string;
                $slease['online'] = in_array(strtolower($slease['mac']),
                    $arpdata_mac) ? $online_string : $offline_string;
                $slease['staticmap_array_index'] = $idx;
                $leases['lease'][] = $slease;
                $dedup_lease = true;
            }
        }

        if ($dedup_lease) {
            $leases['lease'] = $this->__array_remove_duplicate($leases['lease'],
                'ip');
        }
        if ($dedup_failover) {
            $leases['failover'] = $this->__array_remove_duplicate(
                $leases['failover'], 'name');
            asort($leases['failover']);
        }

        return $leases;
    }
    
    private function __array_remove_duplicate($array, $item) {
        # Loop through each array item and check if our input item is witheld
        foreach ($array as $sub) {
            if (isset($sub[$item])) {
                $loop_array[] = $sub[$item];
            }
        }

        # Remove duplicate items from the loop array
        $unique = array_unique(array_reverse($loop_array, true));
        foreach ($unique as $key => $value) {
            $new[] = $array[$key];
        }
        return $new;
    }

    private function __adjust_dhcpd_datetime($dt) {
        # Loop through each DHCPd item and format the time
        foreach ($this->config['dhcpd'] as $item) {
            if ($item['dhcpleaseinlocaltime'] == "yes") {
                $time = strtotime($dt . " GMT");
                if ($time !== false) {
                    return strftime("%Y/%m/%d %H:%M:%S", $time);
                }
            }
        }

        # Returns formatted datetime if loop occurred, otherwise returns input
        return $dt;
    }
}
