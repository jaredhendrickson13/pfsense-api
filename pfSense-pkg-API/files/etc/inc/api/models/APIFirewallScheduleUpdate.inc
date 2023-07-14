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


class APIFirewallScheduleUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified firewall schedule via API";
    }

    public function action() {
        # Update this schedule in the config and reload the firewall filter
        $this->set_config("schedules/schedule/{$this->id}", $this->validated_data);
        $this->__sort_schedules();
        $this->write_config();
        filter_configure();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_name();
        $this->__validate_timerange();
        $this->__validate_descr();
    }

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            if ($this->get_schedule_id_by_name($this->initial_data["name"], true)) {
                $this->id = $this->get_schedule_id_by_name($this->initial_data["name"]);
                $this->validated_data = $this->get_config("schedules/schedule/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(4150);
            }
        } else {
            $this->errors[] = APIResponse\get(4146);
        }
    }

    private function __validate_timerange() {
        # Check for the optional `timerange` payload value
        if (isset($this->initial_data["timerange"])) {
            # Initialize the array to store validated time ranges
            $this->validated_data["timerange"];

            # Require at least 1 time range to be configured
            if (is_array($this->initial_data["timerange"]) and count($this->initial_data["timerange"]) >= 1) {
                # Loop through each requested time range to validate
                foreach ($this->initial_data["timerange"] as $tr_data) {
                    # Validate the time range using the APIFirewallScheduleTimeRangeCreate model
                    $time_range = new APIFirewallScheduleTimeRangeCreate();
                    $time_range->initial_data = $tr_data;
                    $time_range->validate_payload(true);

                    # Add the validated time range if no errors were found
                    if (empty($time_range->errors)) {
                        $this->validated_data["timerange"][] = $time_range->validated_data;
                    } else {
                        $this->errors = $this->errors + $time_range->errors;
                    }
                }
            } else {
                $this->errors[] = APIResponse\get(4162);
            }
        }
    }

    private function __validate_descr() {
        # Check for the optional `descr` payload value
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = strval($this->initial_data["descr"]);
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

    public static function compare_schedules($a, $b) {
        return strcmp($a['name'], $b['name']);
    }

    private function __sort_schedules() {
        $schedules = $this->get_config('schedules/schedule');
        if (is_array($schedules)) {
            usort($schedules, ["APIFirewallScheduleUpdate", "compare_schedules"]);
        }
    }
}
