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


class APIFirewallScheduleTimeRangeDelete extends APIModel {
    private $parent_id;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-schedules-edit"];
        $this->change_note = "Deleted firewall schedule time range via API";
    }

    public function action() {
        # Remove this schedule time range to the config and reload the firewall filter
        $this->del_config("schedules/schedule/{$this->parent_id}/timerange/{$this->id}");
        $this->write_config();
        filter_configure();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_name();
        $this->__validate_id();
    }

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            if ($this->get_schedule_id_by_name($this->initial_data["name"], true)) {
                $this->parent_id = $this->get_schedule_id_by_name($this->initial_data["name"]);
            } else {
                $this->errors[] = APIResponse\get(4150);
            }
        } else {
            $this->errors[] = APIResponse\get(4146);
        }
    }

    private function __validate_id() {
        # Check for our required `id` payload value
        if (isset($this->initial_data["id"])) {
            # Ensure ID is numeric
            $id = (is_numeric($this->initial_data["id"])) ? intval($this->initial_data["id"]) : null;

            if ($this->get_config("schedules/schedule/{$this->parent_id}/timerange/{$id}", false)) {
                # Don't allow client to delete the last time range
                if (count($this->get_config("schedules/schedule/{$this->parent_id}/timerange")) !== 1) {
                    $this->id = intval($id);
                    $this->validated_data = $this->get_config("schedules/schedule/{$this->parent_id}/timerange/{$id}");
                } else {
                    $this->errors[] = APIResponse\get(4165);
                }
            } else {
                $this->errors[] = APIResponse\get(4164);
            }
        } else {
            $this->errors[] = APIResponse\get(4163);
        }
    }

    public function get_schedule_id_by_name($name, $as_bool=false) {
        # Loop through each schedule configured and check it's name
        foreach ($this->get_config("schedules/schedule", []) as $id=>$schedule) {
            # Check if this $schedule's name matches our requested name
            if ($name === $schedule["name"]) {
                return ($as_bool) ? true : $id;
            }
        }
        return ($as_bool) ? false : null;
    }
}
