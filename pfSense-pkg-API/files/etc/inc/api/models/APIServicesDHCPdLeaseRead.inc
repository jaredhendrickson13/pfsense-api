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


class APIServicesDHCPdLeaseRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-dhcpleases"];
    }

    public function action() {
        return APIResponse\get(0, $this->get_dhcpd_leases());
    }

    public function get_dhcpd_leases() {
        # Local variables
        $leases = [];

        # First read our DHCP leases file, then split the string upon every individual lease
        $leases_raw = file_get_contents("/var/dhcpd/var/db/dhcpd.leases");
        $leases_raw = explode("lease ", $leases_raw);

        # Loop through each lease and parse the needed data
        foreach ($leases_raw as $lease_block) {
            # Only parse this block if it is a valid DHCPD lease block
            if ($this->__is_lease($lease_block)) {
                # Create a new lease object and add the corresponding data. Add this object to our leases array.
                $lease = [];
                $lease["state"] = $this->__get_lease_state($lease_block);
                $lease["ip"] = $this->__is_lease($lease_block);
                $lease["mac"] = $this->__get_lease_mac($lease_block);
                $lease["starts"] = $this->__get_lease_starts($lease_block);
                $lease["ends"] = $this->__get_lease_ends($lease_block);
                $lease["ends"] = $this->__get_lease_ends($lease_block);
                $lease["hostname"] = $this->__get_lease_hostname($lease_block);
                $lease["online"] = $this->__is_lease_online($lease);
                $leases[] = $lease;
            }
        }
        return $leases;
    }

    # Checks if a given string is a DHCPD lease configuration block. Returns the IP of the lease or false if none.
    private function __is_lease($lease) {
        # Split the lease string on each line
        $lease_data = explode(PHP_EOL, $lease);
        $lease_heading = explode(" ", $lease_data[0]);

        # Check if the lease heading includes a valid IPv4 address and ends with an open bracket
        if (is_ipaddrv4($lease_heading[0]) and $lease_heading[1] === "{") {
            return $lease_heading[0];
        } else {
            return false;
        }
    }

    # Checks the current state of a DHCP lease
    private function __get_lease_state($lease) {
        # Split the lease string on each line
        $lease_data = explode(PHP_EOL, $lease);

        # For each line in the lease, check for our binding state setting
        foreach ($lease_data as $line) {
            # Remove excessive whitespace
            $line = str_replace("  ", "", $line);
            $line = str_replace("   ", "", $line);

            # Check if our binding state is active
            if ($line === "binding state active;") {
                return "active";
            }
            # Otherwise, check if our binding state is free
            elseif ($line === "binding state free;") {
                return "expired";
            }
        }
        # If we did not find a binding state, return unknown
        return "unknown";
    }

    # Checks the current MAC address of a DHCP lease
    private function __get_lease_mac($lease) {
        # Split the lease string on each line
        $lease_data = explode(PHP_EOL, $lease);

        # For each line in the lease, check for our hardware ethernet setting
        foreach ($lease_data as $line) {
            # Remove excessive whitespace
            $line = str_replace("  ", "", $line);
            $line = str_replace("   ", "", $line);

            # Check if this line holds our hardware ethernet value
            $locator = "hardware ethernet";
            if (substr($line, 0, strlen($locator)) === $locator) {
                return str_replace(";", "", explode($locator." ", $line)[1]);
            }
        }
    }

    # Checks the current start datetime of a DHCP lease
    private function __get_lease_starts($lease) {
        # Split the lease string on each line
        $lease_data = explode(PHP_EOL, $lease);

        # For each line in the lease, check for our starts setting
        foreach ($lease_data as $line) {
            # Remove excessive whitespace
            $line = str_replace("  ", "", $line);
            $line = str_replace("   ", "", $line);

            # Check if this line holds our start value
            $locator = "starts";
            if (substr($line, 0, strlen($locator)) === $locator) {
                return substr(str_replace(";", "", explode($locator." ", $line)[1]), 2);
            }
        }
    }

    # Checks the current end datetime of a DHCP lease
    private function __get_lease_ends($lease) {
        # Split the lease string on each line
        $lease_data = explode(PHP_EOL, $lease);

        # For each line in the lease, check for our ends setting
        foreach ($lease_data as $line) {
            # Remove excessive whitespace
            $line = str_replace("  ", "", $line);
            $line = str_replace("   ", "", $line);

            # Check if this line holds our end value
            $locator = "ends";
            if (substr($line, 0, strlen($locator)) === $locator) {
                return substr(str_replace(";", "", explode($locator." ", $line)[1]), 2);
            }
        }
    }

    # Checks the current hostname of a DHCP lease
    private function __get_lease_hostname($lease) {
        # Split the lease string on each line
        $lease_data = explode(PHP_EOL, $lease);

        # For each line in the lease, check for our client-hostname setting
        foreach ($lease_data as $line) {
            # Remove excessive whitespace
            $line = str_replace("  ", "", $line);
            $line = str_replace("   ", "", $line);

            # Check if this line holds our client-hostname value
            $locator = "client-hostname";
            if (substr($line, 0, strlen($locator)) === $locator) {
                $hostname = str_replace(";", "", explode($locator." ", $line)[1]);
                $hostname = str_replace("\"", "", $hostname);
                return $hostname;
            }
        }
        # Return empty string if none
        return "";
    }

    # Checks if a given IP and MAC are present in the ARP table
    private function __is_lease_online($lease) {
        # Get our current ARP table
        $arp_table = APITools\get_arp_table();

        # Loop through each ARP table entry and see if this lease matches
        foreach ($arp_table as $arp_entry) {
            if ($arp_entry["ip"] === $lease["ip"] and strtolower($arp_entry["mac"]) === strtolower($lease["mac"])) {
                return true;
            }
        }

        # If we did not find a match in the loop, return false
        return false;
    }
}