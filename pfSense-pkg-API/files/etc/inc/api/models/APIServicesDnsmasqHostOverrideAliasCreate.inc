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

class APIServicesDnsmasqHostOverrideAliasCreate extends APIModel {
    private $parent;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsforwarder-edithost"];
        $this->change_note = "Added DNS Forwarder host override alias via API";
        $this->init_config("dnsmasq", "hosts");
    }

    public function action() {
        $this->__init_config();
        $this->config["dnsmasq"]["hosts"][$this->id]["aliases"]["item"][] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty("hosts");
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        $this->__validate_id();
        $this->validate_host();
        $this->validate_domain();
        $this->validate_description();
    }

    private function __validate_id() {
        # Check for our required 'id' payload value. Allow this to be bypassed internally for nested calls
        if ($this->validate_id === false) {
        } elseif (isset($this->initial_data["id"])) {
            if (array_key_exists($this->initial_data["id"], $this->config["dnsmasq"]["hosts"])) {
                $this->id = $this->initial_data["id"];
                $this->parent = $this->config["dnsmasq"]["hosts"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(2062);
            }
        } else {
            $this->errors[] = APIResponse\get(2061);
        }
    }

    public function validate_host() {
        # Check for our required 'host' payload value
        if (isset($this->initial_data["host"])) {
            # Ensure it is a valid hostname
            if (is_hostname($this->initial_data["host"])) {
                $this->validated_data["host"] = $this->initial_data['host'];
            } else {
                $this->errors[] = APIResponse\get(2056);
            }
        } else {
            $this->errors[] = APIResponse\get(2063);
        }
    }

    public function validate_domain() {
        # Check for our required 'domain' payload value
        if (isset($this->initial_data["domain"])) {
            # Ensure it is a validate hostname
            if (is_hostname($this->initial_data["domain"])) {
                $this->validated_data["domain"] = $this->initial_data['domain'];
            } else {
                $this->errors[] = APIResponse\get(2058);
            }
        } else {
            $this->errors[] = APIResponse\get(2064);
        }
    }

    public function validate_description() {
        # Check for our optional 'description' payload value
        if (isset($this->initial_data["description"])) {
            $this->validated_data["description"] = strval($this->initial_data["description"]);
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

    private function __init_config() {
        # Convert aliases config value to array if not already
        if (!is_array($this->config["dnsmasq"]["hosts"][$this->id]["aliases"])) {
            $this->config["dnsmasq"]["hosts"][$this->id]["aliases"] = ["item"=>[]];
        }
    }
}
