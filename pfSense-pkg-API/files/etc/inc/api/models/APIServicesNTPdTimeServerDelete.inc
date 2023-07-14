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


class APIServicesNTPdTimeServerDelete extends APIModel {
    public $timeservers;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Deleted NTP timeserver via API";
        $this->validated_data = $this->get_config("ntpd");
        $this->timeservers = explode(" ", $this->get_config("system/timeservers"));
    }

    public function action() {
        # Save our updated configuration. Note this updates configuration in two different places.
        $this->set_config("ntpd", $this->validated_data);
        $this->set_config("system/timeservers", implode(" ", $this->timeservers));
        $this->write_config();

        # Reconfigure NTP to apply the changes. Since values are stored in two places, simply return the initial data.
        system_ntp_configure();
        $this->validated_data["timeservers"] = $this->timeservers;
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_timeserver();
    }

    private function __validate_timeserver() {
        # Before deleting the timeserver, ensure it exists
        if (in_array($this->initial_data["timeserver"], $this->timeservers)) {
            # Remove the timeserver from our system timeservers. Reindex the array afterwards.
            unset($this->timeservers[array_search($this->initial_data["timeserver"], $this->timeservers)]);
            $this->timeservers = array_values($this->timeservers);

            # Remove the timeserver from the ntpd ispool if found
            $ispool_arr = explode(" ", $this->validated_data["ispool"]);
            if (in_array($this->initial_data["timeserver"], $ispool_arr)) {
                unset($ispool_arr[array_search($this->initial_data["timeserver"], $ispool_arr)]);
                $this->validated_data["ispool"] = implode(" ", $ispool_arr);
            }

            # Remove the timeserver from the ntpd noselect if found
            $noselect_arr = explode(" ", $this->validated_data["noselect"]);
            if (in_array($this->initial_data["timeserver"], $noselect_arr)) {
                unset($noselect_arr[array_search($this->initial_data["timeserver"], $noselect_arr)]);
                $this->validated_data["noselect"] = implode(" ", $noselect_arr);
            }

            # Remove the timeserver from the ntpd prefer if found
            $prefer_arr = explode(" ", $this->validated_data["prefer"]);
            if (in_array($this->initial_data["timeserver"], $prefer_arr)) {
                unset($prefer_arr[array_search($this->initial_data["timeserver"], $prefer_arr)]);
                $this->validated_data["prefer"] = implode(" ", $prefer_arr);
            }

            # If the last timeserver was removed, assert the default to prevent killing the system time
            if (empty($this->timeservers)) {
                $this->timeservers[] = "pool.ntp.org";
            }
        }
        # Otherwise, return error if the request name server does not exist
        else {
            $this->errors[] = APIResponse\get(2053);
        }
    }

}
