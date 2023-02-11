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


class APISystemTunableDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-sysctl"];
        $this->change_note = "Deleted system tunable via API";
    }

    public function action() {
        $this->del_config("sysctl/item/{$this->id}");
        $this->write_config();
        mark_subsystem_dirty('sysctl');
        system_setup_sysctl();
        clear_subsystem_dirty('sysctl');
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Ensure we have a tunable ID to delete
        if (!empty($this->initial_data["id"])) {
            $match_found = false;
            $count = 0;
            foreach ($this->get_config("sysctl/item", []) as $tunable) {
                if ($tunable["tunable"] === $this->initial_data["id"]) {
                    $this->validated_data = $tunable;
                    $this->id = $count;
                    $match_found = true;
                    break;
                }
                $count++;
            }
            # Throw error if we could not find a tunable matching this id
            if (!$match_found) {
                $this->errors[] = APIResponse\get(1017);
            }
        } else {
            $this->errors[] = APIResponse\get(1016);
        }
    }
}
