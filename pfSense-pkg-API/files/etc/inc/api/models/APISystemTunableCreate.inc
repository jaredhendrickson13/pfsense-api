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


class APISystemTunableCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Added system tunable via API";
    }

    public function action() {
        $this->id = $this->get_next_id("sysctl/item");
        $this->set_config("sysctl/item/{$this->id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty('sysctl');
        system_setup_sysctl();
        clear_subsystem_dirty('sysctl');
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Ensure we have a tunable name, that it doesn't already exist, and that it is a string
        if (!empty($this->initial_data["tunable"])) {
            if (is_string($this->initial_data["tunable"])) {
                foreach ($this->get_config("sysctl/item", []) as $item) {
                    if ($item["tunable"] === $this->initial_data["tunable"]) {
                        $this->errors[] = APIResponse\get(1012);
                        break;
                    }
                }
                $this->validated_data["tunable"] = $this->initial_data["tunable"];
            } else {
                $this->errors[] = APIResponse\get(1011);
            }
        } else {
            $this->errors[] = APIResponse\get(1010);
        }

        # Ensure we have a tunable value and that it is a string or integer
        if (isset($this->initial_data["value"])) {
            if (!preg_match("/[^a-zA-Z0-9.\-_%\/]/", $this->initial_data['value'])) {
                $this->validated_data["value"] = $this->initial_data["value"];
            } else {
                $this->errors[] = APIResponse\get(1014);
            }
        } else {
            $this->errors[] = APIResponse\get(1013);
        }

        # Optionally allow a tunable description, ensure it is a string if entered
        if (isset($this->initial_data["descr"])) {
            if (is_string($this->initial_data["descr"])) {
                $this->validated_data["descr"] = $this->initial_data["descr"];
            } else {
                $this->errors[] = APIResponse\get(1015);
            }
        }
    }
}
