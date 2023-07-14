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


class APISystemRebootCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
    }

    public function action() {
        # Use ob_start()/ob_end_clean() to prevent system_reboot() from printing output to the JSON response
        $this->__clean_system_halt();
        return APIResponse\get(0, []);
    }

    private function __clean_system_halt() {
        # Don't actually reboot the system if we are just debugging. Required for E2E testing.
        if ($this->initial_data["debug"] !== true) {
            # Use ob_start()/ob_end_clean() to prevent system_reboot() from printing output to the JSON response
            ob_start();
            system_reboot();
            ob_end_clean();
        }
    }
}
