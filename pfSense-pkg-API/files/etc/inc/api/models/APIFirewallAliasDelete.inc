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

class APIFirewallAliasDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
        $this->change_note = "Deleted firewall alias via API";
    }

    public function action() {
        $del_conf = $this->config["aliases"]["alias"][$this->id];    // Capture alias config before deleting
        unset($this->config["aliases"]["alias"][$this->id]);    // Remove this alias from our configuration
        $this->config["aliases"]["alias"] = array_values($this->config["aliases"]["alias"]);    // Reindex array
        $this->write_config();    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $del_conf);
    }
    
    public function validate_payload() {

        if (isset($this->initial_data['id'])) {
            $name = $this->initial_data['id'];
            $name = APITools\sanitize_str($name);

            // Check that alias is not in use in our configuration
            if (!APITools\alias_in_use($name)) {
                // Loop through our current config and find the index ID for our alias to delete
                $c_count = 0;    // Init loop counter
                foreach ($this->config["aliases"]["alias"] as $ce) {
                    // Check if this entry matches our requested value
                    if ($ce["name"] === $name) {
                        $del_index = $c_count;
                        break;
                    }
                    $c_count++;
                }

                if (is_numeric($del_index)) {
                    $this->id = $del_index;
                } else {
                    $this->errors[] = APIResponse\get(4055);
                }

            } else {
                $this->errors[] = APIResponse\get(4051);
            }
        } else {
            $this->errors = APIResponse\get(4074);
        }

    }
}