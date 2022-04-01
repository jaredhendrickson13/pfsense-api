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


class APISystemPackageDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-packagemanager-installed"];
        $this->change_note = "Deleted package via API";
    }

    public function action() {
        $this->delete_pkg();
        return APIResponse\get(0);
    }

    public function validate_payload(){
        $this->__validate_name();
    }

    # Deletes the package specified in $this->validated_data["name"]
    public function delete_pkg() {
        # Remove the requested package and clean up dependencies afterwards
        pkg_call("delete -y " . $this->validated_data["name"]);
        pkg_call("autoremove -y");

        # Force the action to be logged in the configuration history
        $this->write_config();
    }

    private function __validate_name() {
        # Check for our required name input
        if (isset($this->initial_data["name"])) {
            # Only allow pfSense packages to be deleted, not system packages
            if (APITools\str_starts_with("pfSense-pkg", $this->initial_data["name"])) {
                # Require the package to be presently installed
                if (is_pkg_installed($this->initial_data["name"])) {
                    $this->validated_data["name"] = $this->initial_data["name"];
                } else {
                    $this->errors[] = APIResponse\get(1075);
                }
            } else {
                $this->errors[] = APIResponse\get(1074);
            }
        } else {
            $this->errors[] = APIResponse\get(1073);
        }
    }
}
