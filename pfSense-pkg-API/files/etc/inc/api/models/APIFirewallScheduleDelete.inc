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


class APIFirewallScheduleDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-schedules-edit"];
        $this->change_note = "Deleted firewall schedule via API";
        $this->init_config('schedules', 'schedule');
    }

    public function action() {
        # Delete this schedule from the config and reload the firewall filter
        unset($this->config["schedules"]["schedule"][$this->id]);
        $this->__sort_schedules();
        $this->write_config();
        filter_configure();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_name();
    }

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            if ($this->get_schedule_id_by_name($this->initial_data["name"], true)) {
                # Do not allow this schedule to be deleted if it is in use
                if (!$this->is_schedule_in_use($this->initial_data["name"])) {
                    $this->id = $this->get_schedule_id_by_name($this->initial_data["name"]);
                    $this->validated_data = $this->config["schedules"]["schedule"][$this->id];
                } else {
                    $this->errors[] = APIResponse\get(4166);
                }
            } else {
                $this->errors[] = APIResponse\get(4150);
            }
        } else {
            $this->errors[] = APIResponse\get(4146);
        }
    }

    public function is_schedule_in_use($name) {
        # Loop through each firewall rule and check if it uses this schedule
        if (is_array($this->config['filter']['rule'])) {
            foreach ($this->config['filter']['rule'] as $rule) {
                if ($rule['sched'] === $name) {
                    return true;
                }
            }
        }
        return false;
    }

    public function get_schedule_id_by_name($name, $as_bool=false) {
        # Loop through each schedule configured and check it's name
        foreach ($this->config["schedules"]["schedule"] as $id=>$schedule) {
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
        if (is_array($this->config['schedules']['schedule'])) {
            usort($this->config['schedules']['schedule'], ["APIFirewallScheduleDelete", "compare_schedules"]);
        }
    }
}
