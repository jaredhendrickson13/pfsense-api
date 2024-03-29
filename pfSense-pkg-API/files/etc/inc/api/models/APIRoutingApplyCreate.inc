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


class APIRoutingApplyCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = [
            "page-all",
            "page-system-gateways",
            "page-system-gateways-editgateway",
            "page-system-staticroutes",
            "page-system-staticroutes-editroute"
        ];
    }

    public function action() {
        $this->apply();
        return APIResponse\get(0);
    }

    public static function apply() {
        global $g;

        # Check for the pending changes file and unserialize it
        if (file_exists("{$g['tmp_path']}/.system_routes.apply")) {
            $to_apply_list = unserialize(file_get_contents("{$g['tmp_path']}/.system_routes.apply"));

            # Run commands to apply these changes
            foreach ($to_apply_list as $to_apply) {
                mwexec("{$to_apply}");
            }

            @unlink("{$g['tmp_path']}/.system_routes.apply");
        }

        # Reload associated services
        system_routing_configure();
        system_resolvconf_generate();
        filter_configure();
        setup_gateways_monitor();
        send_event("service reload dyndnsall");
        clear_subsystem_dirty("staticroutes");
    }
}
