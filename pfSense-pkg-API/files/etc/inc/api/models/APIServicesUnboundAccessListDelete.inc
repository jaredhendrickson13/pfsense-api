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


class APIServicesUnboundAccessListDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver-acls"];
        $this->change_note = "Deleted DNS Resolver access list via API";
    }

    public function action() {
        unset($this->config["unbound"]["acls"][$this->id]);
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
    }

    private function __validate_id() {
        # Check for our required 'id' payload value
        if (isset($this->initial_data["id"])) {
            # Check the Unbound ACLs array for ID
            if (array_key_exists($this->initial_data["id"], $this->config["unbound"]["acls"])) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->config["unbound"]["acls"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(2074);
            }
        } else {
            $this->errors[] = APIResponse\get(2072);
        }
    }
}
