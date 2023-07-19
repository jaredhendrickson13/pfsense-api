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


class APIFirewallScheduleTimeRangeCreate extends APIModel {
    private $positions;
    private $months;
    private $days;

    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Added firewall schedule time range via API";
    }

    public function action() {
        # Write this schedule time range to the config and reload the firewall filter
        $next_id = $this->get_next_id("schedules/schedule/{$this->id}/timerange");
        $this->set_config("schedules/schedule/{$this->id}/timerange/{$next_id}", $this->validated_data);
        $this->write_config();
        filter_configure();

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload($nested=false) {
        # Don't require validation of the name on nested API calls
        if (!$nested) {
            $this->__validate_name();
        }

        $this->__validate_position();
        $this->__validate_month();
        $this->__validate_day();
        $this->__validate_hour();
        $this->__validate_rangedescr();
    }

    private function __validate_name() {
        # Check for our required `name` payload value
        if (isset($this->initial_data["name"])) {
            if ($this->get_schedule_id_by_name($this->initial_data["name"], true)) {
                $this->id = $this->get_schedule_id_by_name($this->initial_data["name"]);
            } else {
                $this->errors[] = APIResponse\get(4150);
            }
        } else {
            $this->errors[] = APIResponse\get(4146);
        }
    }

    private function __validate_position() {
        # Check for the optional `position` payload value
        if (isset($this->initial_data["position"])) {
            # Save a copy of the input as an array
            $this->positions = explode(",", $this->initial_data["position"]);

            # Loop through each position and ensure it is valid
            foreach ($this->positions as $position) {
                # Ensure this weekday is numeric value 1-7
                if (!is_numeric($position) or intval($position) < 1 or intval($position) > 7) {
                    $this->errors[] = APIResponse\get(4160);
                    return;
                }
            }
            $this->validated_data["position"] = $this->initial_data["position"];
        }
    }

    private function __validate_month() {
        # Check for the required `month` payload value
        if (isset($this->initial_data["month"])) {
            # Save a copy of the input as an array
            $this->months = explode(",", $this->initial_data["month"]);

            # Loop through each month and ensure it is valid
            foreach ($this->months as $month) {
                # Ensure this month is numeric value 1-12
                if (!is_numeric($month) or intval($month) < 1 or intval($month) > 12) {
                    $this->errors[] = APIResponse\get(4152);
                    return;
                }
            }
            $this->validated_data["month"] = $this->initial_data["month"];
        } elseif (!isset($this->validated_data["position"])) {
            $this->errors[] = APIResponse\get(4151);
        }
    }

    private function __validate_day() {
        # Check for the required `day` payload value
        if (isset($this->initial_data["day"])) {
            # Save a copy of the input as an array
            $this->days = explode(",", $this->initial_data["day"]);

            # Check that the number of days matches the number of months
            if (count($this->months) === count($this->days)) {
                # Loop through each day and ensure it is valid
                foreach ($this->days as $id=>$day) {
                    # Require the day to be within the number of days in this month
                    if (!$this->is_day_in_month($this->months[$id], $day)) {
                        $this->errors[] = APIResponse\get(4155);
                    }
                }
                $this->validated_data["day"] = $this->initial_data["day"];
            } else {
                $this->errors[] = APIResponse\get(4154);
            }
        } elseif (!isset($this->validated_data["position"])) {
            $this->errors[] = APIResponse\get(4153);
        }
    }

    private function __validate_hour() {
        # Check for the required `hour` payload value
        if (isset($this->initial_data["hour"])) {
            # Split the range on the hyphen character
            $start_time = $this->is_valid_24_hour_time(explode("-", $this->initial_data["hour"])[0]);
            $end_time = $this->is_valid_24_hour_time(explode("-", $this->initial_data["hour"])[1]);

            # Check that the start time is valid
            if ($start_time) {
                # Check that the end time is valid
                if ($end_time) {
                    # Check that the start hour is less than or equal to the end hour
                    if ($this->is_valid_24_hour_time_range($start_time, $end_time)) {
                        $this->validated_data["hour"] = $start_time."-".$end_time;
                    } else {
                        $this->errors[] = APIResponse\get(4159);
                    }
                } else {
                    $this->errors[] = APIResponse\get(4158);
                }
            } else {
                $this->errors[] = APIResponse\get(4157);
            }
        } else {
            $this->errors[] = APIResponse\get(4156);
        }
    }

    private function __validate_rangedescr() {
        # Check for the optional `rangedescr` payload value
        if (isset($this->initial_data["rangedescr"])) {
            $this->validated_data["rangedescr"] = strval($this->initial_data["rangedescr"]);
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

    public function is_day_in_month($month, $day) {
        # Local variables
        $month = (is_numeric($month)) ? intval($month) : null;
        $day = (is_numeric($day)) ? intval($day) : null;
        $month_days = [1=>31, 2=>29, 3=>31, 4=>30, 5=>31, 6=>30, 7=>31, 8=>31, 9=>30, 10=>31, 11=>30, 12=>31];

        # Check that the day is within the number of days in it's month
        if (1 <= $day and $day <= $month_days[$month]) {
            return true;
        } else {
            return false;
        }
    }

    public function is_valid_24_hour_time($time) {
        # Convert H:MM to HH:MM
        $time = (substr($time, 1, 1) === ":") ? "0".$time : $time;

        # Check if this string is formatted as a 24 hour time
        if (preg_match("/^(?:2[0-3]|[01][0-9]):[0-5][0-9]$/", $time)) {
            # Remove the first character if H:MM time
            $time = (substr($time, 0, 1) === "0") ? substr($time, 1, strlen($time)) : $time;

            # Check if the minutes are 00, 15, 30, 45, 59 (as required by pfSense)
            if (in_array(substr($time, -2), ["00", "15", "30", "45", "59"])) {
                return $time;
            }
        }
        return false;
    }

    public function is_valid_24_hour_time_range($start_time, $end_time) {
        # Check if the start time is less than the end time
        if (strtotime($start_time) <= strtotime($end_time)) {
            return true;
        }
        return false;
    }
}
