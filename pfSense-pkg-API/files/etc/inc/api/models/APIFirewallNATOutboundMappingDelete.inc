<?php
//   Copyright 2020 Jared Hendrickson
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

class APIFirewallNATOutboundMappingDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-nat-outbound-edit"];
        $this->change_note = "Deleted outbound NAT mapping via API";
    }

    public function action() {
        # Capture the deleted item then remove it. Mark the subsystem as dirty until it is applied.
        $del_rule = $this->config["nat"]["outbound"]["rule"][$this->id];
        unset($this->config["nat"]["outbound"]["rule"][$this->id]);
        APITools\sort_nat_rules(null, null, "outbound");
        $this->write_config();
        $this->apply();
        return APIResponse\get(0, $del_rule);
    }
    
    public function validate_payload() {
        # Require clients to pass in a outbound rule ID to locate the rule to delete
        if (isset($this->initial_data['id'])) {
            // Check that our rule ID exists
            if (array_key_exists($this->initial_data['id'], $this->config["nat"]["outbound"]["rule"])) {
                $this->id = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4105);
            }
        } else {
            $this->errors[] = APIResponse\get(4104);
        }
    }

    public function apply() {
        # Mark the NAT subsystem as changed, clear if applied
        mark_subsystem_dirty('natconf');

        # Allow clients to apply this rule immediately if they passed in an apply value
        if ($this->initial_data["apply"] === true) {
            filter_configure();
            clear_subsystem_dirty('natconf');
            clear_subsystem_dirty('filter');
        }
    }
}