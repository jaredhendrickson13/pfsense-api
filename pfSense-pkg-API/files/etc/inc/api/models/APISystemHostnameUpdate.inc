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

class APISystemHostnameUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system"];
        $this->change_note = "Modified system hostname via API";
        $this->init_config("system");
    }

    public function action() {
        # Write our new hostname and apply changes to the backend
        $this->write_config();
        $this->apply_backend_changes();

        $data = ["hostname" => $this->config["system"]["hostname"], "domain" => $this->config["system"]["domain"]];
        return APIResponse\get(0, $data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['hostname'])) {
            # Check if our hostname is valid
            if (!is_hostname($this->initial_data['hostname']) or !is_unqualified_hostname($this->initial_data['hostname'])) {
                $this->errors[] = APIResponse\get(1000);
            } else {
                $this->config["system"]["hostname"] = $this->initial_data['hostname'];
            }
        }
        if (isset($this->initial_data['domain'])) {
            # Check if our hostname is valid
            if (!is_domain($this->initial_data['domain'])) {
                $this->errors[] = APIResponse\get(1001);
            } else {
                $this->config["system"]["domain"] = $this->initial_data['domain'];
            }
        }
    }

    public function apply_backend_changes() {
        # Update a slew of backend services
        system_hostname_configure();
        system_hosts_generate();
        system_resolvconf_generate();
        if (isset($this->config['dnsmasq']['enable'])) {
            services_dnsmasq_configure();
        } elseif (isset($this->config['unbound']['enable'])) {
            services_unbound_configure();
        }
        filter_configure();
    }
}
