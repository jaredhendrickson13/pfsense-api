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

class APIServicesHAProxyFrontendUpdate extends APIModel {

    private $allowed_modes;
    private $validate_crt;
    private $current_frontends;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->change_note = "Created HAProxy backend via API.";
        $this->requires_auth = false;
        $this->allowed_modes = ["http", "tcp"];
        $this->validate_crt = false;
        $this->current_frontends = $this->get_config("installedpackages/haproxy/ha_backends/item", []);
    }

    public function validate_payload() {
        $this->__validate_target_frontend();
        if (isset($this->initial_data['name']) && $this->__validate_name()) {
            $this->validated_data['name'] = $this->initial_data['name'];
        }
        if (isset($this->initial_data['bind_addrs'])) {
            $this->__validate_extaddrs();
        }
        if (isset($this->initial_data['backend_serverpool'])) {
            $this->__validate_default_backend();
        }
        if ($this->validate_crt && isset($this->initial_data['ssloffloadcert'])) {
            $this->__validate_certificate();
        }
    }

    private function __validate_target_frontend() {
        if (isset($this->initial_data["id"])) {
            if ($this->get_config("installedpackages/haproxy/ha_backends/item/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_backends/item/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(2275, ['id' => $this->initial_data["id"]]);
            }   
        } else {
            $this->errors[] = APIResponse\get(2274, ['request_data' => $this->initial_data]);
        }   
    } 

    private function __validate_name() {
        $name_bad_chars = false;
        $name_already_used = false;

        /* Ensure only valid characters are in use*/
        if (preg_match("/[^a-zA-Z0-9_\.\-]/", $this->initial_data['name']) || str_contains($this->initial_data['name'], ' ')) {
            $this->errors[] = APIResponse\get(2262, ['name' => $this->initial_data['name']]);
            $name_bad_chars = true;
        }   

        /* Ensure that our frontend names are unique */
        foreach ($this->current_frontends as $frontend) {
            if ($this->initial_data['name'] == $frontend['name']) {
                $this->errors[] = APIResponse\get(2263, ['name' => $this->initial_data['name']]);
                $name_already_used = true;
            }   
        }   

        if ($name_already_used || $name_bad_chars) {
            return false;
        } else {
            return true;
        }   
    }

    private function __validate_extaddrs() {

        $this->validated_data['a_extaddr']['item'] = [];
        foreach ($this->initial_data['bind_addrs'] as $extaddr) {
            $bind_address = [];
            $extaddr_bad_ssl = false;
            $extaddr_bad_address = false;
            $extaddr_bad_port = false;

            if (!is_ipaddr($extaddr['address'])) {
                $this->errors[] = APIResponse\get(2269, ['extaddr' => $extaddr]);
                $extaddr_bad_address = true;
            } elseif (!$this->__validate_vip($extaddr['address'])) {
                $this->errors[] = APIResponse\get(2281, ['extaddr' => $extaddr]);
            } else {
                $bind_address['extaddr'] = "{$extaddr['address']}_ipv4";
            }

            $this->__check_ip_in_use($extaddr['address']);

            if (!is_numeric($extaddr['port'])) {
                $this->errors[] = APIResponse\get(2270, ['extaddr' => $extaddr]);
                $extaddr_bad_port = true;
            } else {
                $bind_address['extaddr_port'] = $extaddr['port'];
            }
            if (isset($extaddr['ssl'])) {
                if (!is_bool($extaddr['ssl'])) {
                    $this->errors[] = APIResponse\get(2271, ['extaddr' => $extaddr]);
                    $extaddr_bad_ssl = true;
                } elseif ($extaddr['ssl'] === 'no') {
                    unset($extaddr['ssl']);
                } else {
                    $this->validate_crt = true;
                    $bind_address['extaddr_ssl'] = $extaddr['ssl'];
                }
            }

            if(!$extaddr_bad_address && !$extaddr_bad_port && !$extaddr_bad_ssl) {
                $bind_address['_index'] = '';
                $this->validated_data['a_extaddr']['item'][] = $bind_address;
            }
            unset($bind_address);
        }
    }

    private function __validate_vip($address) {
        $all_vips = get_configured_vip_list('all', VIP_IPALIAS);
        foreach ($all_vips as $id => $vip) {
            if ($vip == $address) {
                return true;
            }
        }
        return false;
    }

    private function __check_ip_in_use($address) {
        foreach ($this->current_frontends as $frontend) {
            foreach ($frontend['a_extaddr']['item'] as $front_addr) {
                $used_address = substr($front_addr['extaddr'], 0, -5);
                if ($address == $used_address) {
                    $this->errors[] = APIResponse\get(2283, ['provided_address' => $address, 'existing_frontend' => $frontend['name']]);
                }
            }
        }
    }

    private function __validate_certificate() {
        $current_certs = $this->get_config('cert', []);
        $cert_valid = false;

        if (!isset($this->initial_data['ssloffloadcert'])) {
            $this->errors[] = APIResponse\get(2277, ['request_data' => $this->initial_data]);
        }

        foreach ($current_certs as $cert) {
            if ($this->initial_data['ssloffloadcert'] == $cert['refid']) {
                $this->validated_data['ssloffloadcert'] = $this->initial_data['ssloffloadcert'];
                $cert_valid = true;
                break;
            }
        }

        if (!$cert_valid) {
            $this->errors[] = APIResponse\get(2278, ['ssloffloadcert' => $this->initial_data['ssloffloadcert']]);
        }
    }

    private function __validate_default_backend() {
        if(!isset($this->initial_data['backend_serverpool'])) {
            $this->errors[] = APIResponse\get(2279, ['request_data' => $this->initial_data]);
        }
        $valid_backend = false;
        $current_backends = get_haproxy_backends();
        foreach ($current_backends as $name => $backend) {
            if ($this->initial_data['backend_serverpool'] == $name) {
                $this->validated_data['backend_serverpool'] = $this->initial_data['backend_serverpool'];
                $valid_backend = true;
                break;
            }
        }

        if (!$valid_backend) {
            $this->errors[] = APIResponse\get(2280, ['backend_serverpool' => $this->initial_data['backend_serverpool']]);
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
            $this->set_config("installedpackages/haproxy/ha_backends/item", $this->current_frontends);
            $this->write_config();
            $revert_msg = 'Reverting back to original configuration.';
            $result = haproxy_check_and_run($revert_msg, true);
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(2272, ['request_data' => $this->initial_data, 'haproxy_errors' => $haproxy_errors]);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, ['msg' => 'Frontend successfully updated!', 'request_data' => $this->initial_data]);
        }
    }

    public function action() {

        $this->set_config("installedpackages/haproxy/ha_backends/item/{$this->id}", $this->validated_data);
        mark_subsystem_dirty('haproxy.conf');
        $this->write_config();

        if ($this->initial_data['apply'] === true) {
            $this->__apply();
        } else {
            return APIResponse\get(0, ['msg' => 'Frontend successfully updated, but not applied. Apply changes using /api/v1/services/haproxy/apply', 'request_data' => $this->initial_data]);
        }
    }
}
