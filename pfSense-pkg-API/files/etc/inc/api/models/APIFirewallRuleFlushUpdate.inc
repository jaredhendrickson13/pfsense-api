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

class APIFirewallRuleFlushUpdate extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Flushed and replaced firewall rules via API";
    }

    public function action() {
        $this->__randomize_tracker();
        $this->set_config("filter/rule", $this->validated_data);
        filter_rules_sort();
        $this->write_config();
        mark_subsystem_dirty('filter');

        # Only reload the firewall filter if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            APIFirewallApplyCreate::apply();
        }

        return APIResponse\get(0, $this->get_config("filter/rule", []));
    }

    public function validate_payload() {
        # Require data to be passed in as an array
        if (is_array($this->initial_data["rules"])) {
            # Require at least one rule to be present
            if (count($this->initial_data["rules"]) >= 1) {
                # Loop through and validate each rule entry requested
                foreach ($this->initial_data["rules"] as $initial_rule) {
                    # Check if this entry is valid for creation using the APIFirewallRuleCreate class
                    $ent = new APIFirewallRuleCreate($this->initial_data);
                    $ent->client = $this->client;
                    $ent->initial_data = $initial_rule;
                    $ent->validate_payload(false);

                    # Check if an occurred while validating this entry
                    if ($ent->errors) {
                        # Grab the error's return code and raise error including the bad entry in the data field
                        $rc = $ent->errors[0]["return"];
                        $this->errors[] = APIResponse\get($rc, $initial_rule);
                        break;
                    } # Otherwise, if the entry was valid, add it to our validated host overrides
                    else {
                        $this->validated_data[] = $ent->validated_data;
                    }
                }
            }
            # Raise an error if an empty array was passed in
            else {
                $this->errors[] = APIResponse\get(4241);
            }
        }
        # Raise an error if rules were not passed in as an array
        else {
            $this->errors[] = APIResponse\get(4240);
        }
    }

    private function __randomize_tracker() {
        # Capture the current microsecond value as a starting point
        $tracker = (int)microtime(true);

        # Loop through each tracker ID and assign it a unique tracker
        foreach($this->validated_data as $id=>$rule) {
            $this->validated_data[$id]["tracker"] = $tracker;
            $tracker--;
        }
    }
}
