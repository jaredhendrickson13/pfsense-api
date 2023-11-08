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
    private $allowed_ssl;
    private $validate_crt;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->change_note = "Created HAProxy backend via API.";
        $this->requires_auth = false;
        $this->allowed_ssl = ["yes", "no"];
        $this->allowed_modes = ["http", "tcp"];
        $this->validate_crt = false;
        $this->current_frontends = $this->get_config("installedpackaged/haproxy/ha_backends/item", []);
    }

    public function validate_payload() {
        if (isset($this->initial_data['new_name']) && $this->__validate_name()) {
            $this->validated_data['name'] = $this->initial_data['name'];
        }
        if (isset($this->intitial_data['a_extaddrs'])) {
            $this->__validate_extaddrs();
        }
        if (isset($this->initial_data['backend_serverpool'])) {
            $this->__validate_default_backend();
        }
        if ($this->validate_crt) {
            $this->__validate_certificate();
        }
    }

    private function __validate_target_frontend() {
        if (isset($this->initial_data["name"])) {
            $frontend_id = get_frontend_id($this->initial_data["name"]);
            if ($this->get_config("installedpackages/haproxy/ha_backends/item/{$frontend_id}", false)) {
                $this->id = $frontend_id;
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_backends/item/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(2276);
            }   
        } elseif (isset($this->initial_data["id"])) {
            if ($this->get_config("installedpackages/haproxy/ha_backends/item/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_backends/item/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(2275);
            }   
        } else {
            $this->errors[] = APIResponse\get(2274);
        }   
    } 

    private function __validate_name() {
        $name_bad_chars = false;
        $name_already_used = false;

        /* Ensure only valid characters are in use*/
        if (preg_match("/[^a-zA-Z0-9_\.\-]/", $this->initial_data['name']) || str_contains($this->initial_data['name'], ' ')) {
            $this->errors[] = APIResponse\get(2262, $this->initial_data['name']);
            $name_bad_chars = true;
        }   

        /* Ensure that our frontend names are unique */
        foreach ($this->current_frontends as $frontend) {
            if ($this->initial_data['name'] == $frontend['name']) {
                $this->errors[] = APIResponse\get(2263, $this->initial_data['name']);
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
        foreach ($this->initial_data['a_extaddr']['item'] as $extaddr) {
            $extaddr_bad_ssl = false;
            $extaddr_bad_address = false;
            $extaddr_bad_port = false;

            if (!is_ipaddr($extaddr['address'])) {
                $this->errors[] = APIResponse\get(2269, [$extaddr['name'] => $extaddr['address']]);
                $extaddr_bad_address = true;
            }
            if (!is_numeric($extaddr['port'])) {
                $this->errors[] = APIResponse\get(2270, [$extaddr['name'] => $extaddr['port']]);
                $extaddr_bad_port = true;
            }
            if (isset($extaddr['ssl'])) {
                if (!in_array($extaddr['ssl'], $this->allowed_server_ssl)) {
                    $this->errors[] = APIResponse\get(2271, [$extaddr['name'] => $extaddr['ssl']]);
                    $extaddr_bad_ssl = true;
                } elseif ($extaddr['ssl'] === 'no') {
                    unset($extaddr['ssl']);
                }
            }

            if(!$extaddr_bad_address && !$extaddr_bad_port && !$extaddr_bad_ssl) {
                $this->__validate_crt = true;
                $this->validated_data['a_extaddr']['item'][] = $extaddr;
            }        
        }
    }

    private function __validate_certificate() {
        $current_certs = get_config('cert', []);
        $cert_valid = false;

        if (!isset($this->initial_data['ssloffloadcert'])) {
            $this->errors[] = APIResponse\get(2277, $this->initial_data);
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
            $this->errors[] = APIResponse\get(2279, $this->initial_data);
        }
        $valid_backend = false;
        $current_backends = get_haproxy_backends();
        foreach ($current_backends as $name => $backend) {
            if ($this->initial_data['name'] == $name) {
                $this->validated_data['name'] = $this->initial_data['name'];
                $valid_backend = true;
                break;
            }
        }

        if (!$valid_backend) {
            $this->errors[] = APIResponse\get(2280, ['backend_serverpool' => $this->initial_data['backend_serverpool']]);
        }

    }

    public function action(){

        $this->set_config("installedpackages/haproxy/ha_backends/item/{$this->id}", $this->validated_data);
        mark_subsystem_dirty('haproxy.conf');
        $this->write_config();

        $apply_msg = 'Applying frontend changes via API.';
        $result = haproxy_check_and_run($apply_msg, true);
        if (!$result) {
            // If it fails, reset config back to original and apply.
            $this->set_config("installedpackages/haproxy/ha_backends/item", $this->current_frontends);
            $this->write_config();
            $revert_msg = 'Reverting back to original configuration.';
            $result = haproxy_check_and_run($revert_msg, true);
            clear_subsystem_dirty('haproxy.conf');
            $this->initial_data['haproxy_result'] = $result;
            return APIResponse\get(2272, $this->initial_data);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, 'Frontend successfully updated!');
        }
    }
}
