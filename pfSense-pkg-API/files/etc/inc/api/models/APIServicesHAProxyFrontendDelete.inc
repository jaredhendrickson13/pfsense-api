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

class APIServicesHAProxyFrontendDelete extends APIModel {

    private $current_frontend;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->requires_auth = false;
        $this->change_note = "Deleted HAProxy backend via API.";
        $this->current_frontend = [];

    }

    public function validate_payload() {
        $this->__validate_target_frontend();
    }

    private function __validate_target_frontend() {
        if (isset($this->initial_data["id"])) {
            if ($this->get_config("installedpackages/haproxy/ha_backends/item/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_backends/item/{$this->id}");
                $this->current_frontend= $this->get_config("installedpackages/haproxy/ha_backends/item/{$this->id}", []);
            } else {
                $this->errors[] = APIResponse\get(2275, ['id' => $this->initial_data['id']]);
            }   
        } else {
            $this->errors[] = APIResponse\get(2274, ['request_data' => $this->initial_data]);
        }   
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
        return array_slice($error_matches,-3);
    }

    private function __apply() {
        $apply_msg = 'Applying frontend changes via API.';
        $result = haproxy_check_and_run($apply_msg, true);
        if (!$result) {
            $haproxy_errors = $this->__parse_syslog();
            // If it fails, reset config back to original and apply.
            $this->set_config("installedpackages/haproxy/ha_backends/item/{$this->id}", $this->current_frontend);
            $this->write_config();
            $revert_msg = 'Reverting back to original configuration.';
            $result = haproxy_check_and_run($revert_msg, true);
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(2272, ['request_data' => $this->initial_data, 'haproxy_errors' => $haproxy_errors]);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, 'Frontend successfully created!');
        }   
    }   

    public function action() {

        $this->del_config("installedpackages/haproxy/ha_backends/item/{$this->id}", $this->validated_data);
        mark_subsystem_dirty('haproxy.conf');
        $this->write_config();

        if ($this-initial_data['apply'] === true) {
            $this->__apply();
        }   
    }   
}
