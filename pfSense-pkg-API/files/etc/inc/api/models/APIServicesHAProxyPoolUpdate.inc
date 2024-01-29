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

class APIServicesHAProxyPoolUpdate extends APIModel {

    private $allowed_algorithms;
    private $allowed_persistence;
    private $allowed_checks;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->requires_auth = false;
        $this->change_note = "Updated HAProxy backend via API.";
        $this->allowed_algorithms = ['roundrobin', 'leastconn'];
        $this->allowed_persistence = ['stick_sourceipv4', 'stick_cookie_value', 'none'];
        $this->allowed_checks = ['HTTP', 'Basic'];
        $this->current_backends = $this->get_config("installedpackages/haproxy/ha_pools/item", []);

    }

    public function validate_payload() {
        $this->__validate_target_backend();
        $this->__validate_backend();
    }

    private function __validate_target_backend() {
        if (isset($this->initial_data["id"])) {
            if ($this->get_config("installedpackages/haproxy/ha_pools/item/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_pools/item/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(2275, ['id' => $this->initial_data['id']]);
            }   
        } else {
            $this->errors[] = APIResponse\get(2274, ['request_data' => $this->initial_data]);
        }   
    }

    private function __validate_backend() {

        if (isset($this->initial_data['name'])) {
            if ($this->__validate_backend_name()) {
                $this->validated_data['name'] = $this->initial_data['name'];
            }
        }

        if (isset($this->initial_data['persist_sticky_type'])) {
            if (!in_array($this->initial_data['persist_sticky_type'], $this->allowed_persistence)) {
                $this->errors[] = APIResponse\get(2264, ['persist_sticky_type' => $this->initial_data['persist_sticky_type']]);
            } else {
                $this->validated_data['persist_sticky_type'] = $this->initial_data['persist_sticky_type'];
                if ($this->validated_data['persist_sticky_type'] == 'stick_cookie_value') {
                    if (!isset($this->initial_data['external_ref_tag'])) {
                        $this->errors[] = APIResponse\get(2273, ['request_data' => $this->initial_data]);
                    }
                    $this->validated_data['persist_cookie_enabled'] = 'yes';
                    $cookie_uniq_str = substr(md5($this->validated_data['name'].$this->validated_data['external_ref_tag'].gethostname()), 0, 10);
                    $this->validated_data['persist_stick_cookiename'] = 'X-HAProxy-Session-'.$cookie_uniq_str;
                    $this->validated_data['persist_cookie_name'] = 'X-HAProxy-Session-'.$cookie_uniq_str;
                } else {
                    $this->validated_data['persist_cookie_enabled'] = 'no';
                }
            }
        }

        if (isset($this->initial_data['balance'])) {
            if (!in_array($this->initial_data['balance'], $this->allowed_algorithms)) {
                $this->errors[] = APIResponse\get(2265, ['balance' => $this->initial_data['balance']]);
            } else {
                $this->validated_data['balance'] = $this->initial_data['balance'];
            }
        }

        if (isset($this->initial_data['check_type'])) {
            if (!in_array($this->initial_data['check_type'], $this->allowed_checks)) {
                $this->errors[] = APIResponse\get(2266, ['check_type' => $this->initial_data['check_type']]);
            } else {
                $this->validated_data['check_type'] = $this->initial_data['check_type'];
            }
        }

    }

    private function __validate_backend_name() {
        $bad_pool_name = false;
        /* Ensure only valid characters are in use*/
        if (preg_match("/[^a-zA-Z0-9_\.\-]/", $this->initial_data['name']) || str_contains($this->initial_data['name'], ' ')) {
            $this->errors[] = APIResponse\get(2262, ['name' => $this->initial_data['name']]);
            $name_bad_chars = true;
        }

        /* Ensure that our pool names are unique */
        $current_backends = get_haproxy_backends();
        foreach ($current_backends as $name => $settings) {
            if ($this->initial_data['name'] == $name) {
                $this->errors[] = APIResponse\get(2263, ['name', $this->initial_data['name']]);
                $name_already_used = true;
            }
        }

        if ($name_already_used || $name_bad_chars) {
            return false;
        } else {
            return true;
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
        $apply_msg = 'Applying backend changes via API.';
        $result = haproxy_check_and_run($apply_msg, true);
        if (!$result) {
            $haproxy_errors = $this->__parse_syslog();
            // If it fails, reset config back to original and apply.
            $this->set_config("installedpackages/haproxy/ha_pools/item", $this->current_backends);
            $this->write_config();
            $revert_msg = 'Reverting back to original configuration.';
            $result = haproxy_check_and_run($revert_msg, true);
            clear_subsystem_dirty('haproxy.conf');
            $this->initial_data['haproxy_result'] = $result;
            return APIResponse\get(2272, ['request_data' => $this->initial_data, 'haproxy_errors' => $haproxy_errors]);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, ['msg' => 'Backend successfully updated!', 'request_data' => $this->initial_data]);
        }
    }

    public function action(){

        $this->set_config("installedpackages/haproxy/ha_pools/item/{$this->id}", $this->validated_data);
        mark_subsystem_dirty('haproxy.conf');
        $this->write_config();

        if ($this->initial_data['apply'] === true) {
            $this->__apply();
        } else {
            return APIResponse\get(0, ['msg' => 'Backend successfully updated, but not applied. Apply changes using /api/v1/services/haproxy/apply', 'request_data' => $this->initial_data]);
        }
    }
}
