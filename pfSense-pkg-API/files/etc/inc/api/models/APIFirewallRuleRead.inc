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


class APIFirewallRuleRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules"];
    }

    public function action() {
        $rule_array = $this->get_config("filter/rule", []);

        // Convert some fields to booleans, to better convenience
        foreach ($rule_array as $i=>$rule) {
            $rule_array[$i]["floating"] = $rule["floating"] === "yes";
            $rule_array[$i]["disabled"] = $rule["disabled"] === "";
            $rule_array[$i]["log"] = $rule["log"] === "";
        }

        return APIResponse\get(0, $rule_array);
    }
}
