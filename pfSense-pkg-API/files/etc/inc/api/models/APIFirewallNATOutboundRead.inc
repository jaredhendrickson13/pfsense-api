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

class APIFirewallNATOutboundRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-nat-outbound"];
    }

    public function action() {
        # If default outbound NAT mode (auto) is configured, set a representation of this value since none is shown
        if (empty($this->config["nat"]["outbound"]["mode"])) {
            $this->validated_data = ["mode" => "automatic"];
        } else {
            $this->validated_data = ["mode" => $this->config["nat"]["outbound"]["mode"]];
        }
        return APIResponse\get(0, $this->validated_data);
    }
}