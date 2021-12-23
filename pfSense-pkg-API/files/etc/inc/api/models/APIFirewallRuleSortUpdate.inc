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

class APIFirewallRuleSortUpdate extends APIModel {
    private $options;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules"];
        $this->change_note = "Sorted firewall rules via API";
        $this->options = [
            "ascend" => SORT_ASC,
            "descend" => SORT_DESC,
            "natural" => SORT_NATURAL,
            "numeric" => SORT_NUMERIC,
            "string" => SORT_STRING
        ];
    }

    public function action() {
        # Sort the firewall rules based on provied criteria
        $this->config["filter"]["rule"] = $this->sort_firewall_rules(
            $this->validated_data["field"],
            $this->validated_data["option"]
        );

        # Only write and apply changes if a dry run was not requested
        if (!$this->initial_data["dry_run"] !== true) {
            $this->write_config();
            mark_subsystem_dirty('filter');

            # Only reload the firewall filter if it was requested by the client
            if ($this->initial_data["apply"] === true) {
                APIFirewallApplyCreate::apply();
            }
        }

        return APIResponse\get(0, $this->config["filter"]["rule"]);
    }

    private function __validate_field() {
        # Check for our required field value
        if (isset($this->initial_data["field"])) {
            $this->validated_data["field"] = strval($this->initial_data["field"]);
        } else {
            $this->errors[] = APIResponse\get(4237);
        }
    }

    private function __validate_option() {
        # Check for the optional option field
        if (isset($this->initial_data["option"])) {
            # Ensure the option is valid
            if (in_array(strtolower($this->initial_data["option"]), $this->options)) {
                $this->validated_data["option"] = strtolower($this->initial_data["option"]);
            } else {
                $this->errors[] = APIResponse\get(4238);
            }
        }
        # Default this field to sort ascending
        else {
            $this->validated_data["option"] = "ascend";
        }
    }

    public function sort_firewall_rules($field, $option="ascend") {
        # Local variables
        $rules = $this->config["filter"]["rule"];
        $criteria = array_column($rules, $field);

        array_multisort($criteria, $this->options[$option], $rules["filter"]["rule"]);
        return $rules;
    }


}
