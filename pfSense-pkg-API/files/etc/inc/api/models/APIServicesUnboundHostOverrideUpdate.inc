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

class APIServicesUnboundHostOverrideUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified DNS Resolver host override via API";
    }

    public function action() {
        // Sort and write our new configuration
        $this->set_config("unbound/hosts/{$this->id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->initial_data["apply"] === true) {
            APITools\unbound_reload_config();
        }
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_id();
        $this->__validate_ip();
        $this->__validate_host();
        $this->__validate_domain();
        $this->__validate_descr();
        $this->__validate_aliases();
    }

    private function __validate_id() {
        # Check for our required 'id' payload value.
        if (isset($this->initial_data["id"])) {
            if ($this->get_config("unbound/hosts/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("unbound/hosts/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(2016);
            }
        } else {
            $this->errors[] = APIResponse\get(2015);
        }
    }

    private function __validate_ip() {
        # Local variables
        $valid_ips = [];

        # Check for our optional 'ip' payload value
        if (isset($this->initial_data['ip']) and !empty($this->initial_data["ip"])) {
            # This field is expected to be an array, if a string was passed in, convert it to a single item array.
            if (!is_array($this->initial_data["ip"])) {
                $this->initial_data["ip"] = array($this->initial_data["ip"]);
            }

            # Loop through each provided IP and ensure it is valid.
            foreach ($this->initial_data["ip"] as $ip) {
                if (!is_ipaddrv4($ip) and !is_ipaddrv6($ip)) {
                    $this->errors[] = APIResponse\get(2011);
                } else {
                    $valid_ips[] = trim($ip);
                }
            }
            # Join our valid IPs into a comma separated string as expected by pfSense
            $this->validated_data["ip"] = implode(",", $valid_ips);
        }
    }

    private function __validate_host() {
        # Check for our optional 'host' payload value
        if (isset($this->initial_data['host'])) {
            # Ensure it is a valid hostname
            if (is_hostname($this->initial_data["host"])) {
                $this->validated_data["host"] = $this->initial_data['host'];
            } else {
                $this->errors[] = APIResponse\get(2046);
            }        }
    }

    private function __validate_domain() {
        # Check for our optional 'domain' payload value
        if (isset($this->initial_data['domain'])) {
            # Ensure it is a validate hostname
            if (is_hostname($this->initial_data["domain"])) {
                $this->validated_data["domain"] = $this->initial_data['domain'];
            } else {
                $this->errors[] = APIResponse\get(2047);
            }
        }
    }

    private function __validate_descr() {
        # Check for our optional 'descr' payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
    }

    private function __validate_aliases() {
        # Check for our optional 'aliases' payload value. This is a nested model creation.
        if (isset($this->initial_data['aliases'])) {
            # Allow aliases to be nested under "item" as found in the XML config
            if (array_key_exists("item", $this->initial_data['aliases'])) {
                $this->initial_data['aliases'] = $this->initial_data['aliases']["item"];
            }

            $this->validated_data["aliases"] = ["item"=>[]];
            foreach ($this->initial_data['aliases'] as $alias) {
                $alias_create = new APIServicesUnboundHostOverrideAliasCreate();
                $alias_create->initial_data = $alias;
                $alias_create->validate_host();
                $alias_create->validate_domain();
                $alias_create->validate_description();
                $this->errors = array_merge($this->errors, $alias_create->errors);
                $this->validated_data["aliases"]["item"][] = $alias_create->validated_data;
            }
        }
    }
}
