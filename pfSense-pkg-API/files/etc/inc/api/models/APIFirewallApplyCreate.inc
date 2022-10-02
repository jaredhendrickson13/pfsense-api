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


class APIFirewallApplyCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = [
            "page-all",
            "page-firewall-rules",
            "page-firewall-rules-edit",
            "page-firewall-aliases",
            "page-firewall-alias-edit",
            "page-firewall-nat-1-1",
            "page-firewall-nat-1-1-edit",
            "page-firewall-nat-outbound",
            "page-firewall-nat-outbound-edit",
            "page-firewall-nat-portforward",
            "page-firewall-nat-portforward-edit"
        ];
    }

    public function action() {
        $this->apply($this->initial_data["async"]);
        return APIResponse\get(0);
    }

    public static function apply($async=true) {
        # Apply the filter changes synchronously if requested
        if ($async === false) {
            filter_configure_sync();
        }
        else {
            filter_configure();
        }

        # Mark changes as applied
        clear_subsystem_dirty('aliases');
        clear_subsystem_dirty('natconf');
        clear_subsystem_dirty('filter');
    }
}
