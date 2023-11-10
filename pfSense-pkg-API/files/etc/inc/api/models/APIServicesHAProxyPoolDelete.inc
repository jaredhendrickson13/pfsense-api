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

class APIServicesHAProxyPoolDelete extends APIModel {

    private $current_backend;

    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-service-haproxy"];
        $this->packages = ["pfSense-pkg-haproxy"];
        $this->requires_auth = false;
        $this->change_note = "Deleted HAProxy backend via API.";
        $this->current_backend = [];

    }

    public function validate_payload() {
        $this->__validate_target_backend();
    }

    private function __validate_target_backend() {
        if (isset($this->initial_data["name"])) {
            $backend_id = get_backend_id($this->initial_data["name"]);
            if ($this->get_config("installedpackages/haproxy/ha_pools/item/{$backend_id}", false)) {
                $this->id = $backend_id;
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_pools/item/{$this->id}");
                $this->current_backend= $this->get_config("installedpackages/haproxy/ha_pools/item/{$this->id}", []);
            } else {
                $this->errors[] = APIResponse\get(2276, ['name' => $this->initial_data['name']]);
            }   
        } elseif (isset($this->initial_data["id"])) {
            if ($this->get_config("installedpackages/haproxy/ha_pools/item/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("installedpackages/haproxy/ha_pools/item/{$this->id}");
                $this->current_backend= $this->get_config("installedpackages/haproxy/ha_pools/item/{$this->id}", []);
            } else {
                $this->errors[] = APIResponse\get(2275, ['id' => $this->initial_data['id']]);
            }   
        } else {
            $this->errors[] = APIResponse\get(2274, ['request_data' => $this->initial_data]);
        }   
    }

    # Tell our API model what to do after successfully validating the client's request
    public function action(){

        $this->del_config("installedpackages/haproxy/ha_pools/item/{$this->id}");
        mark_subsystem_dirty('haproxy.conf');
        $this->write_config();

        $apply_msg = 'Applying backend changes via API.';
        $result = haproxy_check_and_run($apply_msg, true);
        if (!$result) {
            // If it fails, reset config back to original and apply.
            $this->set_config("installedpackages/haproxy/ha_pools/item/{$this->id}", $this->current_backend);
            $this->write_config();
            $revert_msg = 'Reverting back to original configuration.';
            $result = haproxy_check_and_run($revert_msg, true);
            clear_subsystem_dirty('haproxy.conf');
            $this->initial_data['haproxy_result'] = $result;
            return APIResponse\get(2272, $this->initial_data);
        } else {
            clear_subsystem_dirty('haproxy.conf');
            return APIResponse\get(0, 'Backend successfully deleted');
        }
    }
}
