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


class APISystemPackageCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-packagemanager-installpackage"];
        $this->change_note = "Added package via API";
    }

    public function action() {
        # Force the action to be logged in the configuration history if our API response code is success
        if (pkg_install($this->validated_data["name"])) {
            return APIResponse\get(0);
        } else {
            return APIResponse\get(1077);
        }
    }

    public function validate_payload(){
        $this->__validate_name();
    }

    private function __validate_name() {
        # Check for our required name input
        if (isset($this->initial_data["name"])) {
            # Ensure this package exists in pfSense's repos
            if ($this->is_pkg_in_repo($this->initial_data["name"])) {
                # Ensure package is not already installed
                if (!is_pkg_installed($this->initial_data["name"])) {
                    $this->validated_data["name"] = $this->initial_data["name"];
                } else {
                    $this->errors[] = APIResponse\get(1076);
                }
            } else {
                $this->errors[] = APIResponse\get(1075);
            }
        } else {
            $this->errors[] = APIResponse\get(1073);
        }
    }

    # Checks pfSense's upstream package repo for a package by name. Requires upstream internet connection.
    public function is_pkg_in_repo($name) {
        # Variables
        $pkgs = get_pkg_info();

        # Set a bad gateway error if we did not find any packages from the upstream repository
        if ($pkgs === []) {
            $this->errors[] = APIResponse\get(13);
        }

        # Loop through each package in the repo and check for a matching name
        foreach($pkgs as $pkg) {
            if ($name === $pkg["name"])
                return true;
        }

        return false;
    }
}
