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

class APIServicesUnboundHostOverrideUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->change_note = "Modified DNS Resolver host override via API";
    }

    public function action() {
        // Sort and write our new configuration
        $this->config["unbound"]["hosts"][$this->id] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->initial_data["apply"] === true) {
            APITools\unbound_reload_config();
        }
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Check for our required 'id' payload value.
        if (isset($this->initial_data["id"])) {
            if (array_key_exists($this->initial_data["id"], $this->config["unbound"]["hosts"])) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->config["unbound"]["hosts"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(2016);
            }
        } else {
            $this->errors[] = APIResponse\get(2015);
        }

        # Check for our optional 'host' payload value
        if (isset($this->initial_data['host'])) {
            $this->validated_data["host"] = trim($this->initial_data['host']);
        }

        # Check for our optional 'domain' payload value
        if (isset($this->initial_data['domain'])) {
            $this->validated_data["domain"] = trim($this->initial_data['domain']);
        }

        # Check for our required 'ip' payload value
        if (isset($this->initial_data['ip'])) {
            if (!is_ipaddrv4($this->initial_data["ip"]) and !is_ipaddrv6($this->initial_data["ip"])) {
                $this->errors[] = APIResponse\get(2011);
            } else {
                $this->validated_data["ip"] = trim($this->initial_data['ip']);
            }
        }

        # Check for our optional 'descr' payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }

        # If host or domain was updated, ensure this host override doesn't already exist
        if (isset($this->initial_data["host"]) or isset($this->initial_data["domain"])) {
            if (APITools\is_unbound_fqdn($this->validated_data["host"], $this->validated_data["domain"], $this->id)) {
                $this->errors[] = APIResponse\get(2010);
            }
        }

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
                $alias_create->validate_id = false;
                $alias_create->validate_payload();
                $this->errors = array_merge($this->errors, $alias_create->errors);
                $this->validated_data["aliases"]["item"][] = $alias_create->validated_data;
            }
        }
    }
}