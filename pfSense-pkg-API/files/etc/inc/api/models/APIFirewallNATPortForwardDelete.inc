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

class APIFirewallNATPortForwardDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-nat-portforward-edit"];
        $this->change_note = "Deleted NAT rule via API";
    }

    public function action() {
        # Capture the deleted item then remove it. Mark the subsystem as dirty until it is applied.
        $del_rule = $this->del_config("nat/rule/{$this->id}");
        APITools\sort_nat_rules();
        $this->write_config();
        mark_subsystem_dirty('natconf');

        # Only reload the firewall filter if it was requested by the client
        if ($this->initial_data["apply"] === true) {
            APIFirewallApplyCreate::apply();
        }
        return APIResponse\get(0, $del_rule);
    }
    
    public function validate_payload() {
        # Require client to pass in a port forward ID to locate the port forward to delete
        if (isset($this->initial_data['id'])) {
            // Check that our rule ID exists
            if ($this->get_config("nat/rule/{$this->initial_data['id']}", false)) {
                $this->id = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4016);
            }
        } else {
            $this->errors[] = APIResponse\get(4015);
        }
    }
}
