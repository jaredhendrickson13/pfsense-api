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


class APIDiagnosticsCommandPromptCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
    }

    public function action() {
        exec($this->validated_data["shell_cmd"]." 2>&1", $cmd_output);
        return APIResponse\get(0, ["cmd_output"=>implode(PHP_EOL, $cmd_output)]);
    }

    public function validate_payload() {
        $this->__validate_shell_cmd();
    }

    private function __validate_shell_cmd() {
        # Check if we received a shell command to execute
        if ($this->initial_data["shell_cmd"]) {
            $this->validated_data["shell_cmd"] = strval($this->initial_data["shell_cmd"]);
        } else {
            $this->errors[] = APIResponse\get(7000);
        }
    }
}
