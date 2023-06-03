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
require_once("includes/functions.inc.php");


class APIStatusSystemRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-dashboard-widgets", "page-dashboard-all"];
    }

    public function action() {
        return APIResponse\get(0, $this->__get_metrics());
    }

    private function __get_metrics() {

        # Place each gathered metric into our return array
        $sys_info = [
            "system_platform" => system_identify_specific_platform()["descr"],
            "system_serial" => system_get_serial(),
            "system_netgate_id" => system_get_uniqueid(),
            "bios_vendor" => $this->__get_bios_info()["bios_vendor"],
            "bios_version" => $this->__get_bios_info()["bios_version"],
            "bios_date" => $this->__get_bios_info()["bios_date"],
            "cpu_model" => get_single_sysctl("hw.model"),
            "kernel_pti" => boolval(get_single_sysctl('vm.pmap.pti')),
            "mds_mitigation" => get_single_sysctl('hw.mds_disable_state'),
            "temp_c" => $this->__get_temp(),
            "temp_f" => $this->__get_temp(false),
            "load_avg" => $this->__get_load_avg(),
            "cpu_count" => (int)get_single_sysctl("hw.ncpu"),
            "mbuf_usage" => $this->__get_mbuf_usage(),
            "mem_usage" => (!is_null(mem_usage())) ? APITools\float_percent(mem_usage()) : null,
            "swap_usage" => (!is_null(swap_usage())) ? APITools\float_percent(swap_usage()) : null,
            "disk_usage" => $this->__get_filesystem_usage()
        ];
        return $sys_info;
    }

    # Gathers our MBUF usage and returns either a float percentage or null if not calculable
    private function __get_mbuf_usage() {
        $mbufs_text = null;
        $mbuf_usage = null;
        get_mbuf($mbufs_text, $mbuf_usage);
        return (isset($mbuf_usage)) ? APITools\float_percent($mbuf_usage) : null;
    }

    # Gets the current CPU temperatures in either Fº or Cº, return null if no temperature could be found
    private function __get_temp($celsius=true) {
        if ($celsius) {
            return (!empty(get_temp())) ? floatval(get_temp()) : null;
        } else {
            return (!empty(get_temp())) ? floatval(get_temp()) * 1.8 + 32 : null;
        }
    }

    # Gets the current CPU load averages and formats them into an array of float percentages
    private function __get_load_avg() {
        foreach (explode(", ", get_load_average()) as $avg) {
            $load_avg[] = floatval($avg);
        }
        return $load_avg;
    }

    # Gathers our BIOS information into a single dictionary
    private function __get_bios_info() {
        $bios_info = [
            "bios_vendor" => shell_exec('/bin/kenv -q smbios.bios.vendor 2>/dev/null'),
            "bios_version" => shell_exec('/bin/kenv -q smbios.bios.version 2>/dev/null'),
            "bios_date" => shell_exec('/bin/kenv -q smbios.bios.reldate 2>/dev/null'),
        ];

        # Remove newlines
        foreach ($bios_info as $key=>$value) {$bios_info[$key] = str_replace("\n", "", $value);}
        return $bios_info;
    }

    # Gathers filesystem usage data
    private function __get_filesystem_usage() {
        foreach(get_mounted_filesystems() as $fs) {
            if ($fs["mountpoint"] === "/") {
                return APITools\float_percent($fs["percent_used"]);
            }
        }
    }
}
