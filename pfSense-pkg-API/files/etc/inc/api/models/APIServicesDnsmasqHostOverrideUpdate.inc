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

class APIServicesDnsmasqHostOverrideUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsforwarder-edithost"];
        $this->change_note = "Modified DNS Forwarder host override via API";
    }

    public function action() {
        # Update the host override and apply changes
        $this->set_config("dnsmasq/hosts/{$this->id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty("hosts");
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_id();
        $this->__validate_ip();
        $this->__validate_host();
        $this->__validate_domain();
        $this->__validate_descr();
        $this->__validate_aliases();

        # If host or domain was updated, ensure this host override doesn't already exist
        if (isset($this->initial_data["host"]) or isset($this->initial_data["domain"])) {
            if ($this->host_override_exists()) {
                $this->errors[] = APIResponse\get(2054);
            }
        }
    }

    private function __validate_id() {
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

    private function __validate_ip() {
        # Check for our required 'ip' payload value
        if (isset($this->initial_data['ip'])) {
            if (!is_ipaddrv4($this->initial_data["ip"]) and !is_ipaddrv6($this->initial_data["ip"])) {
                $this->errors[] = APIResponse\get(2060);
            } else {
                $this->validated_data["ip"] = trim($this->initial_data['ip']);
            }
        }
    }

    private function __validate_host() {
        # Check for our optional 'host' payload value
        if (isset($this->initial_data['host'])) {
            # Ensure it is a valid hostname
            if (is_hostname($this->initial_data["host"])) {
                $this->validated_data["host"] = $this->initial_data['host'];
            } else {
                $this->errors[] = APIResponse\get(2056);
            }        }
    }

    private function __validate_domain() {
        # Check for our optional 'domain' payload value
        if (isset($this->initial_data['domain'])) {
            # Ensure it is a validate hostname
            if (is_hostname($this->initial_data["domain"])) {
                $this->validated_data["domain"] = $this->initial_data['domain'];
            } else {
                $this->errors[] = APIResponse\get(2058);
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

            $this->validated_data["aliases"]["item"] = [];
            foreach ($this->initial_data['aliases'] as $alias) {
                $alias_create = new APIServicesDnsmasqHostOverrideAliasCreate();
                $alias_create->initial_data = $alias;
                $alias_create->validate_host();
                $alias_create->validate_domain();
                $alias_create->validate_description();
                $this->errors = array_merge($this->errors, $alias_create->errors);
                $this->validated_data["aliases"]["item"][] = $alias_create->validated_data;
            }
        }
    }

    public function host_override_exists() {
        # Local constants
        $HOST = $this->validated_data["host"];
        $DOMAIN = $this->validated_data["domain"];
        $IP = $this->validated_data["ip"];

        # Loop through each host override and check if the FQDN already exists
        foreach ($this->get_config("dnsmasq/hosts", []) as $index=>$ent) {
            # Check the FQDN matches this entry
            if ($ent["host"] === $HOST and $ent["domain"] === $DOMAIN) {
                # Host overrides are allowed an IPv4 and IPv6 address, check if it already has one.
                if ((is_ipaddrv4($ent["ip"]) and is_ipaddrv4($IP)) or (is_ipaddrv6($ent["ip"]) and is_ipaddrv6($IP))){
                    # If we are working with an existing instance, allow existing FQDN if ID matches
                    if ($index !== $this->id) {
                        return true;
                    }
                }
            }
        }
        return false;
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
