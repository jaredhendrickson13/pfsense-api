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
require_once("haproxy/haproxy.inc");
require_once("haproxy/haproxy_utils.inc");

class APIServicesHAProxyApplyCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->change_note = "Applied HAProxy changes via API.";
        $this->privileges = ["page-all", "page-service-haproxy"];
    }

    private function __parse_syslog() {
        $syslog_file_handle = fopen('/var/log/system.log', 'r');
        $error_matches = [];
        while (!feof($syslog_file_handle)) {
            $log_line = fgets($syslog_file_handle, 4096);
            if (preg_match('/haproxy.*Fatal errors/', $line)) {
                $error_matches[] = $line;
            }
        }
        return array_slice($error_matches, -3);
    }

    public function action() {
        $apply_msg = 'Applying HAProxy changes...';
        $result = haproxy_check_and_run($apply_msg, true);
        if (!$result) {
            $haproxy_errors = $this->__parse_syslog();
            return APIResponse\get(2285, ['haproxy_errors' => $haproxy_errors]);
        }
    }
}
