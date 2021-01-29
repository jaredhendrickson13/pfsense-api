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

class APIFirewallRuleDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules-edit"];
        $this->change_note = "Deleted firewall rule via API";
    }

    public function action() {
        # Capture the rule data, remove it from config, sort and apply filter
        $del_rule = $this->config["filter"]["rule"][$this->id];
        unset($this->config["filter"]["rule"][$this->id]);
        APITools\sort_firewall_rules();
        $this->write_config();
        $this->apply();
        return APIResponse\get(0, $del_rule);
    }
    
    public function validate_payload() {
        # Check for our required 'tracker' payload value
        if (isset($this->initial_data['tracker'])) {
            # Loop through each rule and check if our tracker matches
            foreach ($this->config["filter"]["rule"] as $id=>$rule) {
                if (intval($rule["tracker"]) === intval($this->initial_data['tracker'])) {
                    $this->id = $id;
                    break;
                }
            }
            # Return not found error if no match was found
            if (!is_int($this->id)) {
                $this->errors[] = APIResponse\get(4032);
            }
        } else {
            $this->errors[] = APIResponse\get(4031);
        }
    }

    public function apply() {
        # Mark the firewall subsystem as changed, this will be cleared if applied
        mark_subsystem_dirty('filter');

        # Optionally allow clients to apply this rule immediately if they passed in a true apply value
        if ($this->initial_data["apply"] === true) {
            filter_configure();
            clear_subsystem_dirty('filter');
        }
    }
}