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

class APIFirewallRuleDelete extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Deleted firewall rule via API";
    }

    public function action() {
        # Remove rule from config, sort and apply filter if requested
        $this->del_config("filter/rule/{$this->id}");
        filter_rules_sort();
        $this->write_config();
        mark_subsystem_dirty('filter');

        # Only reload the firewall filter if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            APIFirewallApplyCreate::apply();
        }
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        $this->__validate_tracker();
    }

    private function __validate_tracker() {
        # Check for our required 'tracker' payload value
        if (isset($this->initial_data['tracker'])) {
            # Ensure the tracker provided is a numeric
            if (is_numeric($this->initial_data["tracker"])) {
                # Loop through each rule and check if our tracker matches
                foreach ($this->get_config("filter/rule", []) as $id => $rule) {
                    if (intval($rule["tracker"]) === intval($this->initial_data['tracker'])) {
                        $this->id = $id;
                        $this->validated_data = $this->get_config("filter/rule/{$this->id}");
                        break;
                    }
                }
            }

            # Return error if no matching rule was found
            if (!isset($this->id)) {
                $this->errors[] = APIResponse\get(4032);
            }
        } else {
            $this->errors[] = APIResponse\get(4031);
        }
    }
}
