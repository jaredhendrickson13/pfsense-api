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

class APIFirewallNATOneToOneDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-nat-1-1-edit"];
        $this->change_note = "Deleted 1:1 NAT mapping via API";
    }

    public function action() {
        # Capture the deleted item then remove it. Mark the subsystem as dirty until it is applied.
        $del_rule = $this->del_config("nat/onetoone/{$this->id}");
        APITools\sort_nat_rules(null, null, "onetoone");
        $this->write_config();
        $this->apply();
        return APIResponse\get(0, $del_rule);
    }
    
    public function validate_payload() {
        # Require clients to pass in 1:1 rule ID to locate the rule to delete
        if (isset($this->initial_data['id'])) {
            // Check that our rule ID exists
            if ($this->get_config("nat/onetoone/{$this->initial_data['id']}", false)) {
                $this->id = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4084);
            }
        } else {
            $this->errors[] = APIResponse\get(4083);
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
