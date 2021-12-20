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
require_once("api/models/APISystemDNSRead.inc");


class APISystemDNSUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system"];
        $this->change_note = "Modified system DNS servers via API";
    }

    public function action() {
        $this->write_config();
        $this->apply_backend_changes();
        return APIResponse\get(0, (new APISystemDNSRead())->action()["data"]);
    }

    public function validate_payload() {
        $this->__validate_dnsserver();
        $this->__validate_dnsallowoverride();
        $this->__validate_dnslocalhost();
    }

    private function __validate_dnsallowoverride() {
        if ($this->initial_data['dnsallowoverride'] === true) {
            $this->config["system"]["dnsallowoverride"] = "";
        } elseif ($this->initial_data['dnsallowoverride'] === false) {
            unset($this->config["system"]["dnsallowoverride"]);
        }
    }

    private function __validate_dnslocalhost() {
        # Validate this field as needed for pfSense 2.5+
        if ($this->initial_data['dnslocalhost'] === true) {
            unset($this->config["system"]["dnslocalhost"]);
        } elseif ($this->initial_data['dnslocalhost'] === false) {
            $this->config["system"]["dnslocalhost"] = "remote";
        }
    }

    private function __validate_dnsserver() {
        if (isset($this->initial_data['dnsserver'])) {
            # Use an internal API call to the APISystemDNSServerCreate model
            $dns_server_c = new APISystemDNSServerCreate();
            $dns_server_c->initial_data["dnsserver"] = $this->initial_data["dnsserver"];
            $dns_server_c->__validate_dnsserver();
            $this->errors = $this->errors + $dns_server_c->errors;
            $this->config["system"]["dnsserver"] = $dns_server_c->validated_data["dnsserver"];
        }
    }

    public function apply_backend_changes() {
        # Update a slew of backend services
        system_resolvconf_generate();
        if (isset($this->config['dnsmasq']['enable'])) {
            services_dnsmasq_configure();
        } elseif (isset($this->config['unbound']['enable'])) {
            services_unbound_configure();
        }

        # Reload DNS services and firewall filter
        send_event("service reload dns");
        filter_configure();
    }
}
