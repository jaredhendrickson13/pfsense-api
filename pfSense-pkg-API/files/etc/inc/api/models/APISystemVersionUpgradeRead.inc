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


class APISystemVersionUpgradeRead extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
    }

    public function action() {
        $ver = get_system_pkg_version(false, $this->validated_data["use_cache"]);
        return APIResponse\get(0, ($ver) ?: []);
    }

    public function validate_payload() {
        $this->__validate_use_cache();
    }

    private function __validate_use_cache() {
        # Check for the optional 'use_cache' field
        if ($this->initial_data["use_cache"] === false) {
            $this->validated_data["use_cache"] = false;
        } else {
            $this->validated_data["use_cache"] = true;
        }
    }
}
