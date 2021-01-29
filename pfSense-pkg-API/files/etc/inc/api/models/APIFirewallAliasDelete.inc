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

class APIFirewallAliasDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
        $this->change_note = "Deleted firewall alias via API";
    }

    public function action() {
        $del_conf = $this->config["aliases"]["alias"][$this->id];
        unset($this->config["aliases"]["alias"][$this->id]);
        $this->config["aliases"]["alias"] = array_values($this->config["aliases"]["alias"]);
        $this->write_config();
        send_event("filter reload");
        return APIResponse\get(0, $del_conf);
    }

    private function __validate_id() {
        # Require clients to pass in an ID field containing the alias name or index value
        if (isset($this->initial_data["id"])) {
            # Loop through each alias and check for a match
            foreach ($this->config["aliases"]["alias"] as $id=>$alias) {
                # First check if the ID matches the index value or the alias name
                if ($this->initial_data["id"] === $id or $this->initial_data["id"] === $alias["name"]) {
                    # Only allow deletion if the firewall alias is not in use.
                    if (!APITools\alias_in_use($alias["name"])) {
                        $this->id = $id;
                        $this->validated_data = $alias;
                    } else {
                        $this->errors[] = APIResponse\get(4108);
                    }
                    break;
                }
            }
            # If we did not find an ID in the loop, return a not found error
            if (is_null($this->id)) {
                $this->errors[] = APIResponse\get(4055);
            }
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
    }
    
    public function validate_payload() {
        $this->__validate_id();
    }
}