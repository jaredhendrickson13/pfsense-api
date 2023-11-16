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

class APIServicesHAProxyPoolServersUpdate extends APIModel {

    private $current_servers;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->requires_auth = false;
        $this->change_note = "Updated HAProxy backend server pool via API.";
        $this->current_servers = [];
    }

    public function validate_payload() {
        $this->__validate_target_backend();
        $this->__validate_servers($this->initial_data['servers']);
    }

    private function __validate_target_backend() {
        if (isset($this->initial_data["name"])) {
            $backend_id = get_backend_id($this->initial_data["name"]);
            if ($this->get_config("installedpackages/haproxy/ha_pools/item/{$backend_id}", false)) {
                $this->id = $backend_id;
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_pools/item/{$this->id}");
                $this->current_servers = $this->validated_data['ha_servers']['item'];
            } else {
                $this->errors[] = APIResponse\get(2276, ['name' => $this->initial_data['name']]);
            }
        } elseif (isset($this->initial_data["id"])) {
            if ($this->get_config("installedpackages/haproxy/ha_pools/item/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_pools/item/{$this->id}");
                $this->current_servers = $this->validated_data['ha_servers']['item'];
            } else {
                $this->errors[] = APIResponse\get(2275, ['id' => $this->initial_data['id']]);
            }   
        } else {
            $this->errors[] = APIResponse\get(2274, ['request_data' => $this->initial_data]);
        }   
    }

    private function __validate_servers($servers) {

        $this->validated_data['ha_servers']['item'] = [];
        foreach ($servers as $server) {
            $server_bad_name = false;
            $server_bad_address = false;
            $server_bad_port = false;
            $server_bad_ssl = false;

            if (preg_match("/[^a-zA-Z0-9\.\-_]/", $server['name'])) {
                $this->errors[] = APIResponse\get(2267, [$server['name'] => $server]);
                $server_bad_name = true;
            }
            if (!preg_match("/.{2,}/", $server['name'])) {
                $this->errors[] = APIResponse\get(2268, [$server['name'] => $server]);
                $server_bad_name = true;
            }
            if (!is_ipaddr($server['address'])) {
                $this->errors[] = APIResponse\get(2269, [$server['name'] => $server]);
                $server_bad_address = true;
            }
            if (!is_numeric($server['port'])) {
                $this->errors[] = APIResponse\get(2270, [$server['name'] => $server]);
                $server_bad_address = true;
            }
            if (!array_key_exists('ssl', $server) || !is_bool($server['ssl'])) {
                $this->errors[] = APIResponse\get(2271, [$server['name'] => $server]);
                $server_bad_ssl = true;
            } elseif ($server['ssl'] === 'no') {
                unset($server['ssl']);
            }
            if(!$server_bad_name && !$server_bad_address && !$server_bad_port && !$server_bad_ssl) {
                $server['cookie'] = $server['name'];
                $this->validated_data['ha_servers']['item'][] = $server;
            }
        }
    }

    private function __populate_defaults() {

        foreach ($this->backend_defaults as $setting => $value) {
            if (!array_key_exists($setting, $this->validated_data)) {
                $this->validated_data[$setting] = $this->backend_defaults[$setting];
            }
        }

    }

    private function __apply() {
        $apply_msg = 'Applying backend changes via API.';
        $result = haproxy_check_and_run($apply_msg, true);
        if (!$result) {
            // If it fails, reset config back to original and apply.
            $this->set_config("installedpackages/haproxy/ha_pools/item{$this->id}/ha_servers/item", $this->current_servers);
            $this->write_config();
            $revert_msg = 'Reverting back to original configuration.';
            $result = haproxy_check_and_run($revert_msg, true);
            clear_subsystem_dirty('haproxy.conf');
            $this->initial_data['haproxy_result'] = $result;
            return APIResponse\get(2272, $this->initial_data);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, 'Backend servers successfully updated');
        }
    }

    public function action(){

        $this->set_config("installedpackages/haproxy/ha_pools/item/{$this->id}/ha_servers/item", $this->validated_data['ha_servers']['item']);
        mark_subsystem_dirty('haproxy.conf');
        $this->write_config();

        if ($this->initial_data['apply']) {
            $this->__apply();
        }
    }
}
