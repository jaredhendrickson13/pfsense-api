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


class APIServicesNTPdTimeServerCreate extends APIModel {
    public $timeservers;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-ntpd"];
        $this->change_note = "Added NTP timeserver via API";
        $this->validated_data = $this->config["ntpd"];
        $this->timeservers = explode(" ", $this->config["system"]["timeservers"]);
        $this->init_config("ntpd");
        $this->init_config("system");
    }

    public function action() {
        # Save our updated configuration. Note this updates configuration in two different places.
        $this->config["ntpd"] = $this->validated_data;
        $this->config["system"]["timeservers"] = implode(" ", array_filter($this->timeservers));
        $this->write_config();

        # Reconfigure NTP to apply the changes then return the response
        system_ntp_configure();
        $this->validated_data["timeservers"] = $this->timeservers;
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        $this->__validate_timeserver_limit();
        $this->__validate_timeserver();
        $this->__validate_ispool();
        $this->__validate_noselect();
        $this->__validate_prefer();
    }

    private function __validate_timeserver_limit() {
        # Before adding a new timeserver, ensure we are not already at our maximum limit
        if (count($this->timeservers) >= 10) {
            $this->errors[] = APIResponse\get(2052);
        }
    }

    private function __validate_timeserver() {
        # Check for our required 'timeserver' payload value
        # Timeservers must be tracked separately as they are stored under system > timeservers rather than the
        # NTP service settings. Do not use the validated_data property to track valid timeserver input.
        if (isset($this->initial_data["timeserver"])) {
            # Ensure the value is a valid IP address or hostname
            if (is_ipaddr($this->initial_data["timeserver"]) or is_hostname($this->initial_data["timeserver"])) {
                $this->timeservers[] = $this->initial_data['timeserver'];
            } else {
                $this->errors[] = APIResponse\get(2051);
            }
        } else {
            $this->errors[] = APIResponse\get(2050);
        }
    }

    private function __validate_ispool() {
        # Check for our optional 'ispool' payload value
        if ($this->initial_data['ispool'] === true) {
            $this->validated_data["ispool"] = $this->validated_data["ispool"].$this->initial_data["timeserver"]." ";
        }
    }

    private function __validate_noselect() {
        # Check for our optional 'noselect' payload value
        if ($this->initial_data['noselect'] === true) {
            $this->validated_data["noselect"] = $this->validated_data["noselect"].$this->initial_data["timeserver"]." ";
        }
    }

    private function __validate_prefer() {
        # Check for our optional 'prefer' payload value
        if ($this->initial_data['prefer'] === true) {
            $this->validated_data["prefer"] = $this->validated_data["prefer"].$this->initial_data["timeserver"]." ";
        }
    }


}
