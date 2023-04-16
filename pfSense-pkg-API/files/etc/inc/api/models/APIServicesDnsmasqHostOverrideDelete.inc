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

class APIServicesDnsmasqHostOverrideDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsforwarder-edithost"];
        $this->change_note = "Deleted DNS Forwarder host override via API";
    }

    public function action() {
        $del_data = $this->del_config("dnsmasq/hosts/{$this->id}");
        $this->write_config();
        mark_subsystem_dirty("hosts");
        $this->apply();
        return APIResponse\get(0, $del_data);
    }
    
    public function validate_payload() {
        # Check for our required 'id' payload value.
        if (isset($this->initial_data["id"])) {
            if ($this->get_config("dnsmasq/hosts/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("dnsmasq/hosts/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(2062);
            }
        } else {
            $this->errors[] = APIResponse\get(2061);
        }
    }

    public function apply() {
        if ($this->initial_data['apply'] === true) {
            services_dnsmasq_configure();

            # Reload dependent services
            filter_configure();
            system_resolvconf_generate();
            system_dhcpleases_configure();
            clear_subsystem_dirty('hosts');
        }
    }
}
