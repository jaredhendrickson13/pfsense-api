<?php
//   Copyright 2021 Jared Hendrickson
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

function override_create_host_cmp($a, $b) {
    return strcasecmp($a['host'], $b['host']);
}

class APIServicesDnsmasqHostOverrideCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsforwarder-edithost"];
        $this->change_note = "Added DNS Forwarder host override via API";
    }

    public function action() {
        $this->config["dnsmasq"]["hosts"][] = $this->validated_data;
        usort($this->config["dnsmasq"]["hosts"], "override_create_host_cmp");
        $this->write_config();
        mark_subsystem_dirty("hosts");
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        $this->__validate_host();
        $this->__validate_domain();
        $this->__validate_ip();

        # Before proceeding, check that a host override matching this host, domain and IP type doesn't already exist
        if ($this->host_override_exists()) {
            $this->errors[] = APIResponse\get(2054);
        }

        $this->__validate_descr();
        $this->__validate_aliases();
    }

    private function __validate_host() {
        # Check for our required 'host' payload value
        if (isset($this->initial_data['host'])) {
            # Ensure it is a valid hostname
            if (is_hostname($this->initial_data["host"])) {
                $this->validated_data["host"] = $this->initial_data['host'];
            } else {
                $this->errors[] = APIResponse\get(2056);
            }
        } else {
            $this->errors[] = APIResponse\get(2055);
        }
    }

    private function __validate_domain() {
        # Check for our required 'domain' payload value
        if (isset($this->initial_data['domain'])) {
            # Ensure it is a validate hostname
            if (is_hostname($this->initial_data["domain"])) {
                $this->validated_data["domain"] = $this->initial_data['domain'];
            } else {
                $this->errors[] = APIResponse\get(2058);
            }
        } else {
            $this->errors[] = APIResponse\get(2057);
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
        } else {
            $this->errors[] = APIResponse\get(2059);
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
                $alias_create = new APIServicesUnboundHostOverrideAliasCreate();
                $alias_create->initial_data = $alias;
                $alias_create->validate_host();
                $alias_create->validate_domain();
                $alias_create->validate_description();
                $alias_create->alias_exists($this->validated_data);
                $this->errors = array_merge($this->errors, $alias_create->errors);
                $this->validated_data["aliases"]["item"][] = $alias_create->validated_data;
            }
        }
    }

    public function host_override_exists() {
        # Local variables
        $host = $this->validated_data["host"];
        $domain = $this->validated_data["domain"];
        $ip = $this->validated_data["ip"];

        # Loop through each host override and check if the FQDN already exists
        foreach ($this->config["dnsmasq"]["hosts"] as $ent) {
            # Check the FQDN matches this entry
            if ($ent["host"] === $host and $ent["domain"] === $domain) {
                # Host overrides are allowed an IPv4 and IPv6 address, check if it already has one.
                if ((is_ipaddrv4($ent["ip"]) and is_ipaddrv4($ip)) or (is_ipaddrv6($ent["ip"]) and is_ipaddrv6($ip))){
                    return true;
                }
            }

            # Check FQDN within host override aliases as well
            if (is_array($ent["aliases"])) {
                foreach ($ent["aliases"]["item"] as $alias_ent) {
                    if ($alias_ent["host"] === $host and $alias_ent["domain"] === $domain) {
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