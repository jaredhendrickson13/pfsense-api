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


class APIServicesNTPdRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-ntpd"];
        $this->init_config("ntpd");
        $this->init_config("system");
    }

    public function action() {
        # Return the current NTP configuration if it exists, otherwise return empty array
        if (isset($this->config["ntpd"])) {
            $data = $this->config["ntpd"];
            $data["timeservers"] = explode(" ", $this->config["system"]["timeservers"]);
            return APIResponse\get(0, $data);
        } else {
            return APIResponse\get(0, []);
        }
    }
}
