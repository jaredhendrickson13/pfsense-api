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


class APIFirewallAliasRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-aliases"];
    }

    public function action() {
        // Check that we have a configuration
        if (!empty($this->config["aliases"]["alias"])) {
            $alias_array = $this->config["aliases"]["alias"];
        } else {
            $alias_array = [];
        }
        return APIResponse\get(0, $alias_array);
    }
}