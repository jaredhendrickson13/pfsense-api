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

class APIServicesDnsmasqHostOverrideAliasCreate extends APIModel {
    private $parent;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsforwarder-edithost"];
        $this->change_note = "Added DNS Forwarder host override alias via API";
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

        # Ensure alias host/domain combo isn't already used
        if ($this->alias_exists($this->parent)) {
            $this->errors[] = APIResponse\get(2065);
        }
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

    public function alias_exists() {
        $HOST_TYPES = $this->get_hostnames_ip_types();
        $HOSTNAME = $this->validated_data["host"].".".$this->validated_data["domain"];

        # Check if the requested host override is an IPv4 type
        if (is_ipaddrv4($this->validated_data["ip"])) {
            # Check if this hostname already has an IPv4 override
            if ($HOST_TYPES[$HOSTNAME]["ipv4_used"] === true) {
                return true;
            }
        }
        # Check if the requested host override is an IPv6 type
        if (is_ipaddrv6($this->validated_data["ip"])) {
            # Check if this hostname already has an IPv6 override
            if ($HOST_TYPES[$HOSTNAME]["ipv6_used"] === true) {
                return true;
            }
        }
    }

    /**
     * Loops through each configured dnqmasq host override and alias and checks which IP types are in use
     *
     * @return array $host_types    an array of hostnames with IP types in use and their respective IDs
     */
    public function get_hostnames_ip_types() {
        $host_types = [];

        # Loop through each host override and check if the FQDN already exists
        foreach ($this->config["dnsmasq"]["hosts"] as $index=>$override) {
            $parent_hostname = $override["host"].".".$override["domain"];

            # Check if this is an IPv4 address
            if (is_ipaddrv4($override["ip"])) {
                $host_types[$parent_hostname]["ipv4_id"] = $index;
                $host_types[$parent_hostname]["ipv4_used"] = true;
            }
            # Check if this is an IPv6 address
            if (is_ipaddrv6($override["ip"])) {
                $host_types[$parent_hostname]["ipv6_id"] = $index;
                $host_types[$parent_hostname]["ipv6_used"] = true;
            }

            # Loop through nested aliases and perform the same checks
            foreach ($this->config["dnsmasq"]["hosts"] as $alias) {
                $alias_hostname = $override["host"].".".$override["domain"];

                # Check if the parent has an IPv4 address
                if (is_ipaddrv4($override["ip"])) {
                    $host_types[$alias_hostname]["ipv4_id"] = $index;
                    $host_types[$alias_hostname]["ipv4_used"] = true;
                }
                # Check if the parent has an IPv4 address
                if (is_ipaddrv6($override["ip"])) {
                    $host_types[$alias_hostname]["ipv6_id"] = $index;
                    $host_types[$alias_hostname]["ipv6_used"] = true;
                }
            }
        }
        return $host_types;
    }


    private function __init_config() {
        # Convert aliases config value to array if not already
        if (!is_array($this->config["dnsmasq"]["hosts"][$this->id]["aliases"])) {
            $this->config["dnsmasq"]["hosts"][$this->id]["aliases"] = ["item"=>[]];
        }
    }
}