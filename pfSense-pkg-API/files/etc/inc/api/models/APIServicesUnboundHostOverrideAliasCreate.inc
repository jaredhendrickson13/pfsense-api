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

class APIServicesUnboundHostOverrideAliasCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->change_note = "Added DNS Resolver host override alias via API";
    }

    public function action() {
        # Convert aliases config value to array if not already
        if (!is_array($this->config["unbound"]["hosts"][$this->id]["aliases"])) {
            $this->config["unbound"]["hosts"][$this->id]["aliases"] = ["item"=>[]];
        }

        $this->config["unbound"]["hosts"][$this->id]["aliases"]["item"][] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->initial_data['apply'] === true) {
            APITools\unbound_reload_config();
        }
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        $this->__validate_id();
        $this->validate_host();
        $this->validate_domain();
        $this->validate_description();

        # Ensure alias host/domain combo isn't already used
        if (($this->alias_exists($this->validated_data))) {
            $this->errors[] = APIResponse\get(2009);
        }
    }

    private function __validate_id() {
        # Check for our required 'id' payload value. Allow this to be bypassed internally for nested calls
        if ($this->validate_id === false) {
        } elseif (isset($this->initial_data["id"])) {
            if (array_key_exists($this->initial_data["id"], $this->config["unbound"]["hosts"])) {
                $this->id = $this->initial_data["id"];
            } else {
                $this->errors[] = APIResponse\get(2016);
            }
        } else {
            $this->errors[] = APIResponse\get(2015);
        }
    }

    public function validate_host() {
        # Check for our required 'host' payload value
        if (isset($this->initial_data["host"])) {
            # Ensure it is a valid hostname
            if (is_hostname($this->initial_data["host"])) {
                $this->validated_data["host"] = $this->initial_data['host'];
            } else {
                $this->errors[] = APIResponse\get(2046);
            }
        } else {
            $this->errors[] = APIResponse\get(2007);
        }
    }

    public function validate_domain() {
        # Check for our required 'domain' payload value
        if (isset($this->initial_data["domain"])) {
            # Ensure it is a validate hostname
            if (is_hostname($this->initial_data["domain"])) {
                $this->validated_data["domain"] = $this->initial_data['domain'];
            } else {
                $this->errors[] = APIResponse\get(2047);
            }
        } else {
            $this->errors[] = APIResponse\get(2008);
        }
    }

    public function validate_description() {
        # Check for our optional 'description' payload value
        if (isset($this->initial_data["description"])) {
            $this->validated_data["description"] = strval($this->initial_data["description"]);
        }
    }

    public function alias_exists($parent, $id=null) {
        # Local variables
        $host = $this->validated_data["host"];
        $domain = $this->validated_data["domain"];

        # Loop through each host override and check if the FQDN already exists
        foreach ($this->config["unbound"]["hosts"] as $index=>$ent) {
            # Check the FQDN matches this entry
            if ($ent["host"] === $host and $ent["domain"] === $domain) {
                $this->errors[] = APIResponse\get(2009);
            }

            # Check FQDN within host override aliases as well
            if (is_array($ent["aliases"])) {
                foreach ($ent["aliases"]["item"] as $alias_ent) {
                    if ($alias_ent["host"] === $host and $alias_ent["domain"] === $domain) {
                        $ip = $parent["ip"];
                        # Aliases are allowed for an IPv4 and IPv6 alias, check if it already has one.
                        if ((is_ipaddrv4($ent["ip"]) and is_ipaddrv4($ip)) or (is_ipaddrv6($ent["ip"]) and is_ipaddrv6($ip))){
                            if ($index !== $id) {
                                $this->errors[] = APIResponse\get(2009);
                            }
                        }
                    }
                }
            }
        }
    }
}
