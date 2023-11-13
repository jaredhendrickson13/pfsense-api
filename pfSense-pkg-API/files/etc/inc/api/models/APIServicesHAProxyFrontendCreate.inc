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

class APIServicesHAProxyFrontendCreate extends APIModel {

    private $frontend_defaults;
    private $allowed_modes;
    private $allowed_ssl;
    private $validate_crt;
    private $current_frontends;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->change_note = "Created HAProxy backend via API.";
        $this->requires_auth = false;
        $this->allowed_ssl = ["yes", "no"];
        $this->allowed_modes = ["http", "tcp"];
        $this->validate_crt = false;
        $this->current_frontends = $this->get_config("installedpackages/haproxy/ha_backends/item", []);
        $this->frontend_defaults = [
            "name" => "template-frontend",
            "status" => "active",
            "type" => "http",
            "httpclose" => "http-keep-alive",
            "ssloffloadcert" => "",
            "advanced" => "",
            "ha_acls" => [
                "item" => [
                    [
                        "name" => "acme_validation",
                        "expression" => "path_starts_with",
                        "value" => "/.well-known/acme-challenge",
                        "backendservercountbackend" => "acme-validation",
                        "_index" => "",
                    ],
                    [
                        "name" => "is_https",
                        "expression" => "custom",
                        "value" => "ssl_fc",
                        "backendservercountbackend" => "",
                        "_index" => "",
                    ],
                ],
            ],
            "ha_certificates" => "",
            "clientcert_ca" => "",
            "clientcert_crl" => "",
            "a_extaddr" => [
                "item" => [],
            ],
            "a_actionitems" => [
                "item" => [
                    [
                        "action" => "use_backend",
                        "acl" => "acme_validation",
                        "use_backendbackend" => "acme-validation",
                        "_index" => "",
                    ],
                    [
                        "action" => "http-request_set-header",
                        "use_backendbackend" => "acme-validation",
                        "http-request_set-headername" => "X-Cluster-Client-IP",
                        "http-request_set-headerfmt" => "%[src]",
                        "_index" => "",
                    ],
                ],
            ],
            "a_errorfiles" => "",
            "backend_serverpool" => "",
            "descr" => "",
            "forwardfor" => "yes",
            "ssl_crtlist_advanced" => "alpn h2,http/1.1",
            "max_connections" => "8192",
            "client_timeout" => "300000",
            "dontlognull" => "yes",
        ];
    }

    public function validate_payload() {
        if ($this->__validate_name()) {
            $this->validated_data['name'] = $this->initial_data['name'];
        }
        $this->__validate_subacct();
        $this->__validate_mode();
        $this->__validate_descr();
        $this->__validate_extaddrs();
        $this->__validate_default_backend();

        if ($this->validate_crt) {
            $this->__validate_certificate();
        }

        $this->__populate_defaults();
    }

    private function __validate_mode() {
        if (!isset($this->initial_data['type'])) {
            $this->errors[] = APIResponse\get(2282, ['type' => $this->initial_data['type']]);
        } elseif (!in_array($this->initial_data['type'], $this->allowed_modes)){
            $this->errors[] = APIResponse\get(2282, ['type' => $this->initial_data['type']]);
        }
    }

    private function __validate_subacct() {
        if (isset($this->initial_data['lw_subacct'])) {
            if (strlen($this->initial_data['lw_subacct']) == 6 && ctype_alnum($this->initial_data['lw_subacct'])) {
                $this->validated_data['lw_subacct'] = $this->initial_data['lw_subacct'];
            }
        } else {
            $this->errors[] = APIResponse\get(2273, ['lw_subacct' => $this->initial_data['lw_subacct']]);
        }
    }

    private function __validate_descr() {
        if (!isset($this->initial_data['descr'])) {
            $this->errors[] = APIResponse\get(2281, ['request_data' => $this->initial_data]);
        } else {
            $this->validated_data['descr'] = $this->initial_data['descr'];
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
            if (!is_numeric($extaddr['port'])) {
                $this->errors[] = APIResponse\get(2270, ['extaddr' => $extaddr]);
                $extaddr_bad_port = true;
            } else {
                $bind_address['extaddr_port'] = $extaddr['port'];
            }
            if (isset($extaddr['ssl'])) {
                if (!in_array($extaddr['ssl'], $this->allowed_ssl)) {
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

    private function __populate_defaults() {

        foreach ($this->frontend_defaults as $setting => $value) {
            if (!array_key_exists($setting, $this->validated_data)) {
                $this->validated_data[$setting] = $this->frontend_defaults[$setting];
            }
        }

    }

    public function action() {

        $this->id = $this->get_next_id("installedpackages/haproxy/ha_backends/item");
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
            return APIResponse\get(2272, $this->initial_data);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, 'Frontend successfully created!');
        }
    }
}
