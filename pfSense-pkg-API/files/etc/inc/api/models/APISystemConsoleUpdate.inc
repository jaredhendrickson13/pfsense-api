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

class APISystemConsoleUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-admin"];
        $this->change_note = "Modified system console settings";
        $this->init_config("system");
    }

    public function action() {
        # Write our console settings
        $this->config["system"] = $this->validated_data;
        $this->write_config();
        console_configure();

        $data = ["disableconsolemenu" => isset($this->validated_data["disableconsolemenu"])];
        return APIResponse\get(0, $data);
    }

    public function validate_payload() {
        $this->validated_data = $this->config["system"];
        $this->__validate_disableconsolemenu();
    }

    private function __validate_disableconsolemenu() {
        if (isset($this->initial_data['disableconsolemenu'])) {
            # Check if the console menu should be password protected
            if ($this->initial_data['disableconsolemenu'] === true) {
                $this->validated_data['disableconsolemenu'] = "";
            }
            elseif ($this->initial_data['disableconsolemenu'] === false) {
                unset($this->validated_data['disableconsolemenu']);
            }
        }
    }
}
