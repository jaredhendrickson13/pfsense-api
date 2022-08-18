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


class APIStatusLogSettingsUpdate extends APIModel {
    public $compression_type_changed;
    public $nginx_log_changed;
    public $filter_log_changed;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-logs-settings"];
        $this->change_note = "Modified log settings via API";
    }

    public function action() {
        global $g;

        # Unset existing log rotations if compression type has changed
        if ($this->compression_type_changed) {
            foreach (system_syslogd_get_all_logfilenames() as $log_file) {
                unlink_if_exists("{$g['varlog_path']}/{$log_file}.log.*");
            }
        }

        # Write the config changes are restart syslogd
        $this->config["syslog"] = $this->validated_data;
        system_syslogd_start();

        # Restart the webConfigurator is nginx logs were changed
        if ($this->nginx_log_changed) {
            APITools\restart_webconfigurator();
        }

        # Reload the firewall filter if firewall logs were changed
        if ($this->filter_log_changed) {
            filter_configure();
        }

        # Restart pflog
        filter_pflog_start(true);

        return APIResponse\get(0, []);
    }

    private function __validate_format() {
        global $syslog_formats;

        # Check for the optional 'format' field
        if (isset($this->initial_data["format"])) {
            # Ensure syslog format is supported
            if (array_key_exists($this->initial_data["format"], $syslog_formats)) {
                $this->validated_data["format"] = $this->initial_data["format"];
            }
            else {
                $this->errors[] = APIResponse\get(8000);
            }
        }
    }

    private function __validate_reverse() {
        # Check for the optional 'reverse' field
        if ($this->initial_data["reverse"] === true) {
            $this->validated_data["reverse"] = true;
        }
        elseif ($this->initial_data["reverse"] === false) {
            $this->validated_data["reverse"] = false;
        }
    }

    private function __validate_nentries() {
        # Check for the optional 'nentries' field
        if (isset($this->initial_data["nentries"])) {
            # Ensure value is numeric between 5 and 200000
            if (intval($this->initial_data["nentries"]) >= 5 && intval($this->initial_data["nentries"]) <= 200000) {
                $this->validated_data["nentries"] = intval($this->initial_data["nentries"]);
            }
            else {
                $this->errors[] = APIResponse\get(8001);
            }
        }
    }

    private function __validate_logdefaultblock() {
        # Check for the optional 'logdefaultblock' field
        if ($this->initial_data["logdefaultblock"] === true) {
            $this->filter_log_changed = !isset($this->validated_data["logdefaultblock"]);
            $this->validated_data["logdefaultblock"] = true;
        }
        elseif ($this->initial_data["logdefaultblock"] === false) {
            $this->filter_log_changed = isset($this->validated_data["logdefaultblock"]);
            $this->validated_data["logdefaultblock"] = false;
        }
    }

    private function __validate_logdefaultpass() {
        # Check for the optional 'logdefaultpass' field
        if ($this->initial_data["logdefaultpass"] === true) {
            $this->filter_log_changed = !isset($this->validated_data["logdefaultpass"]);
            $this->validated_data["logdefaultpass"] = true;
        }
        elseif ($this->initial_data["logdefaultpass"] === false) {
            $this->filter_log_changed = isset($this->validated_data["logdefaultpass"]);
            $this->validated_data["logdefaultpass"] = false;
        }
    }

    private function __validate_logbogons() {
        # Check for the optional 'logbogons' field
        if ($this->initial_data["logbogons"] === true) {
            $this->filter_log_changed = !isset($this->validated_data["logbogons"]);
            $this->validated_data["logbogons"] = true;
        }
        elseif ($this->initial_data["logbogons"] === false) {
            $this->filter_log_changed = isset($this->validated_data["logbogons"]);
            $this->validated_data["logbogons"] = false;
        }
    }

    private function __validate_logprivatenets() {
        # Check for the optional 'logprivatenets' field
        if ($this->initial_data["logprivatenets"] === true) {
            $this->filter_log_changed = !isset($this->validated_data["logprivatenets"]);
            $this->validated_data["logprivatenets"] = true;
        }
        elseif ($this->initial_data["logprivatenets"] === false) {
            $this->filter_log_changed = isset($this->validated_data["logprivatenets"]);
            $this->validated_data["logprivatenets"] = false;
        }
    }

    private function __validate_lognginx() {
        # Check for the optional 'lognginx' field
        if ($this->initial_data["lognginx"] === true) {
            $this->nginx_log_changed = !isset($this->validated_data["lognginx"]);
            $this->validated_data["lognginx"] = true;
        }
        elseif ($this->initial_data["lognginx"] === false) {
            $this->nginx_log_changed = isset($this->validated_data["lognginx"]);
            $this->validated_data["lognginx"] = false;
        }
    }

    private function __validate_rawfilter() {
        # Check for the optional 'rawfilter' field
        if ($this->initial_data["rawfilter"] === true) {
            $this->validated_data["rawfilter"] = true;
        }
        elseif ($this->initial_data["rawfilter"] === false) {
            $this->validated_data["rawfilter"] = false;
        }
    }

    private function __validate_disablelocallogging() {
        # Check for the optional 'disablelocallogging' field
        if ($this->initial_data["disablelocallogging"] === true) {
            $this->validated_data["disablelocallogging"] = true;
        }
        elseif ($this->initial_data["disablelocallogging"] === false) {
            $this->validated_data["disablelocallogging"] = false;
        }
    }

    private function __validate_logconfigchanges() {
        # Check for the optional 'logconfigchanges' field
        if ($this->initial_data["logconfigchanges"] === true) {
            $this->validated_data["logconfigchanges"] = true;
        }
        elseif ($this->initial_data["logconfigchanges"] === false) {
            $this->validated_data["logconfigchanges"] = false;
        }
    }

    private function __validate_filterdescriptions() {
        # Check for the optional 'filterdescriptions' field
        if (isset($this->initial_data["filterdescriptions"])) {
            # Ensure value is supported
            if (in_array($this->initial_data["format"], [0, 1, 2])) {
                $this->validated_data["format"] = $this->initial_data["format"];
            }
            else {
                $this->errors[] = APIResponse\get(8002);
            }
        }
    }

    private function __validate_enable() {
        # Check for the optional 'enable' field
        if ($this->initial_data["enable"] === true) {
            $this->validated_data["enable"] = true;
        }
        elseif ($this->initial_data["enable"] === false) {
            $this->validated_data["enable"] = false;
            unset($this->validated_data["remoteserver"]);
            unset($this->validated_data["remoteserver2"]);
            unset($this->validated_data["remoteserver3"]);
        }
    }

    private function __validate_logfilesize() {
        # Check for the optional 'logfilesize' field
        if (isset($this->initial_data["logfilesize"])) {
            # Force value to be integer
            $this->initial_data["logfilesize"] = intval($this->initial_data["logfilesize"]);

            # Require value to be in range
            if ($this->initial_data['logfilesize'] >= 100000 && $this->initial_data['logfilesize'] <= 2147483648) {
                $this->validated_data["logfilesize"] = $this->initial_data["logfilesize"];
            }
            else {
                $this->errors[] = APIResponse\get(8003);
            }
        }
    }

    private function __validate_rotatecount() {
        # Check for the optional 'rotatecount' field
        if (isset($this->initial_data["rotatecount"])) {
            # Force value to be integer
            $this->initial_data["rotatecount"] = intval($this->initial_data["rotatecount"]);

            # Require value to be in range
            if ($this->initial_data['rotatecount'] >= 0 && $this->initial_data['rotatecount'] <= 99) {
                $this->validated_data["rotatecount"] = $this->initial_data["rotatecount"];
            }
            else {
                $this->errors[] = APIResponse\get(8004);
            }
        }
    }

    private function __validate_logcompressiontype() {
        global $system_log_compression_types;

        # Check for the optional 'logcompressiontype' field
        if (isset($this->initial_data["logcompressiontype"])) {
            # Ensure syslog compression type is supported
            if (array_key_exists($this->initial_data["logcompressiontype"], $system_log_compression_types)) {
                # First check if the compression type has changed
                if ($this->validated_data["logcompressiontype"] !== $this->initial_data["logcompressiontype"]) {
                    $this->compression_type_changed = true;
                }

                # Store the validated data
                $this->validated_data["logcompressiontype"] = $this->initial_data["logcompressiontype"];
            }
            else {
                $this->errors[] = APIResponse\get(8005);
            }
        }
    }

    private function __validate_remoteserver() {
        # Only check for this field if rsyslog is enabled
        if ($this->validated_data["enable"]) {
            # Check for the optional 'remoteserver' field
            if (isset($this->initial_data["remoteserver"])) {
                # Require remote server to be valid IP or hostname with or without port
                if ($this->is_valid_syslog_server($this->initial_data["remoteserver"])) {
                    $this->validated_data["remoteserver"] = $this->initial_data["remoteserver"];
                }
                else {
                    $this->errors[] = APIResponse\get(8006);
                }
            }
        }
    }

    private function __validate_remoteserver2() {
        # Only check for this field if remoteserver is set
        if ($this->validated_data["remoteserver"]) {
            # Check for the optional 'remoteserver2' field
            if (isset($this->initial_data["remoteserver2"])) {
                # Require remote server to be valid IP or hostname with or without port
                if ($this->is_valid_syslog_server($this->initial_data["remoteserver2"])) {
                    $this->validated_data["remoteserver2"] = $this->initial_data["remoteserver2"];
                }
                else {
                    $this->errors[] = APIResponse\get(8006);
                }
            }
        }
    }

    private function __validate_remoteserver3() {
        # Only check for this field if remoteserver2 is set
        if ($this->validated_data["remoteserver2"]) {
            # Check for the optional 'remoteserver3' field
            if (isset($this->initial_data["remoteserver3"])) {
                # Require remote server to be valid IP or hostname with or without port
                if ($this->is_valid_syslog_server($this->initial_data["remoteserver3"])) {
                    $this->validated_data["remoteserver3"] = $this->initial_data["remoteserver3"];
                }
                else {
                    $this->errors[] = APIResponse\get(8006);
                }
            }
        }
    }

    private function __validate_sourceip() {
        # Only check for this field if remote syslog is enabled
        if ($this->validated_data["enable"]) {
            # Check for the optional 'sourceip' field
            if (isset($this->initial_data["sourceip"])) {
                # Unset this value if empty
                if (in_array($this->initial_data["sourceip"], ["", "any"])) {
                    unset($this->validated_data["sourceip"]);
                }
                # Otherwise, value to be known interface
                elseif (APITools\get_pfsense_if_id($this->initial_data["sourceip"])) {
                    $this->validated_data["sourceip"] = APITools\get_pfsense_if_id($this->initial_data["sourceip"]);
                }
                # If no condition was met, return an unknown value error
                else {
                    $this->errors[] = APIResponse\get(8007);
                }
            }
        }
    }

    private function __validate_ipprotocol() {
        # Only check for this field if remote syslog is enabled
        if ($this->validated_data["enable"]) {
            # Check for the optional 'ipprotocol' field
            if (isset($this->initial_data["ipprotocol"])) {
                # Ensure value is supported
                if (in_array($this->initial_data["ipprotocol"], ["ipv4", "ipv6"])) {
                    $this->validated_data["ipprotocol"] = $this->initial_data["ipprotocol"];
                } else {
                    $this->errors[] = APIResponse\get(8008);
                }
            }
        }
    }

    private function __validate_rsyslog_contents() {
        # Set supported fields
        $rsyslog_content_fields = [
            "logall", "filter", "dhcp", "auth", "portalauth", "vpn", "dpinger", "hostapd", "system", "resolver", "ppp",
            "routing", "ntpd"
        ];

        # Only validate this field if rsyslog is enabled
        if ($this->validated_data["enable"]) {
            # Validate each of these fields individually
             foreach ($rsyslog_content_fields as $field) {
                 # Check for the optional field
                 if (isset($this->initial_data[$field])) {
                     if ($this->initial_data[$field] === true) {
                         $this->validated_data[$field] = true;
                     }
                     elseif ($this->initial_data[$field] === false) {
                         $this->validated_data[$field] = false;
                     }
                 }
             }
        }
    }

    public function is_valid_syslog_server($host) {
        # Allow host to be IP address or hostname with or without a port.
        if (is_ipaddr($host) or is_ipaddrwithport($host) or is_hostname($host) or is_hostnamewithport($host)) {
            return true;
        }

        return false;
    }
}
