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
require_once("api/models/APISystemDNSRead.inc");


class APISystemDNSServerCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system"];
        $this->change_note = "Added system DNS servers via API";
    }

    public function action() {
        $this->config["system"]["dnsserver"] = $this->validated_data["dnsserver"];
        $this->write_config();    // Apply our configuration change

        // Update a slew of backend services
        system_resolvconf_generate();
        if (isset($this->config['dnsmasq']['enable'])) {
            services_dnsmasq_configure();
        } elseif (isset($this->config['unbound']['enable'])) {
            services_unbound_configure();
        }

        // Reload DNS services and firewall filter
        send_event("service reload dns");
        filter_configure();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['dnsserver'])) {
            $this->validated_data["dnsserver"] = $this->config["system"]["dnsserver"];

            # If values are not an array, convert it
            if (!is_array($this->initial_data["dnsserver"])) {
                $this->initial_data["dnsserver"] = array($this->initial_data["dnsserver"]);
            }
            if (!is_array($this->validated_data["dnsserver"])) {
                $this->validated_data["dnsserver"] = [$this->validated_data["dnsserver"]];

            }

            # Loop through each requested DNS server and ensure it is valid. Add this to our existing servers.
            foreach ($this->initial_data["dnsserver"] as $ds) {
                # Check if our DNS server is a valid IP
                if (!is_ipaddrv4($ds) and !is_ipaddrv6($ds)) {
                    $this->errors[] = APIResponse\get(1007);
                    break;
                } else {
                    $this->validated_data["dnsserver"][] = $ds;
                }
            }
            $this->validated_data = array_filter($this->validated_data);
        }
    }
}
