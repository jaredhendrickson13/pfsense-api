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


class APIServicesUnboundAccessListRowCreate extends APIModel {
    private $ip_version;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver-acls"];
        $this->change_note = "Added DNS Resolver access list network via API";
        $this->ip_version = "";
        $this->init_config("unbound", "acls");
    }

    public function action() {
        $this->config["unbound"]["acls"][$this->id]["row"][] = $this->validated_data;
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
        $this->__validate_acl_network();
        $this->__validate_mask();
        $this->__validate_description();
    }

    private function __validate_id() {
        # Check for our required 'id' payload value
        if (isset($this->initial_data["id"])) {
            # Check the Unbound ACLs array for ID
            if (array_key_exists($this->initial_data["id"], $this->config["unbound"]["acls"])) {
                $this->id = $this->initial_data["id"];
            } else {
                $this->errors[] = APIResponse\get(2074);
            }
        } else {
            $this->errors[] = APIResponse\get(2072);
        }
    }

    public function __validate_acl_network() {
        # Check for our required 'acl_network' payload value
        if (isset($this->initial_data['acl_network'])) {
            # Check if this is an IPv4 or IPv6 address
            if (is_ipaddr($this->initial_data["acl_network"])) {
                $this->ip_version = intval(is_ipaddr($this->initial_data["acl_network"]));
                $this->validated_data["acl_network"] = $this->initial_data['acl_network'];
            } else {
                $this->errors[] = APIResponse\get(2069);
            }
        } else {
            $this->errors[] = APIResponse\get(2068);
        }
    }

    public function __validate_mask() {
        # Check for our required 'mask' payload value
        if (isset($this->initial_data['mask'])) {
            # Check that our mask is within range (between 0-32) for IPv4 network.
            if ($this->ip_version === 4 and $this->initial_data["mask"] >= 0 and $this->initial_data["mask"] <= 32)  {
                $this->validated_data["mask"] = $this->initial_data['mask'];
            }
            elseif ($this->ip_version === 6 and $this->initial_data["mask"] >= 0 and $this->initial_data["mask"] <= 128)  {
                $this->validated_data["mask"] = $this->initial_data['mask'];
            }
            else {
                $this->errors[] = APIResponse\get(2070);
            }
        } else {
            $this->errors[] = APIResponse\get(2071);
        }
    }

    public function __validate_description() {
        # Check for our optional 'description' payload value
        if (isset($this->initial_data['description'])) {
            $this->validated_data["description"] = $this->initial_data['description'];
        } else {
            $this->validated_data["description"] = "";
        }
    }
}
