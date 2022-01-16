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


class APISystemPackageCreate extends APIModel {
    public $pkg_mode;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-packagemanager-installpackage"];
        $this->change_note = "Added package via API";
    }

    public function action() {
        return APIResponse\get($this->install_pkg());
    }

    public function validate_payload(){
        $this->__validate_name();
        $this->__validate_force();
    }

    # Attempts to install the package specified in $this->validated_data["name"]
    public function install_pkg() {
        # Format and execute our pkg command
        $pkg_force = ($this->validated_data["force"]) ? "-f" : "";
        $pkg_cmd = "pkg ".$this->pkg_mode."-y ".$pkg_force." ".$this->validated_data["name"];
        exec($pkg_cmd, $pkg_out, $pkg_rc);

        # Check for known errors
        $api_rc = $this->__check_pkg_install($pkg_out, $pkg_rc);

        # Force the action to be logged in the configuration history if our API response code is success
        if ($api_rc === 0) {
            $this->write_config();
        }

        return $api_rc;
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

    private function __validate_name() {
        # Check for our required name input
        if (isset($this->initial_data["name"])) {
            # Check if this is a remote/third-party package to be installed by URL
            if (filter_var($this->initial_data["name"], FILTER_VALIDATE_URL)) {
                # Set pkg add to install package from URL
                $this->pkg_mode = "add";
                $this->validated_data["name"] = filter_var($this->initial_data["name"], FILTER_VALIDATE_URL);
            }
            # Otherwise, we will assume this package exists in pfSense's package repos
            else {
                # Set pkg install to install the package from pfSense's repos
                $this->pkg_mode = "install";

                # Ensure this package exists in pfSense's repos
                if ($this->is_pkg_in_repo($this->initial_data["name"])) {
                    # Ensure package is not already installed
                    if (is_pkg_installed($this->initial_data["name"])) {
                        $this->validated_data["name"] = $this->initial_data["name"];
                    } else {
                        $this->errors[] = APIResponse\get(1076);
                    }
                } else {
                    $this->errors[] = APIResponse\get(1075);
                }

            }
        } else {
            $this->errors[] = APIResponse\get(1073);
        }
    }

    private function __validate_force() {
        # Check for our option force input
        if ($this->initial_data["force"] === true) {
            $this->validated_data["force"] = true;
        }
    }

    # This function is intended to take the output of our pkg add or install command and check for failures.
    # Returns the corresponding API response ID.
    # TODO: matching text based error messages is prone to breaking as the pkg cli tool changes. As of now, pkg does
    # TODO: not return unique return codes for specific errors. Re-evaluate as time goes and refactor when a better way
    # TODO: is made available.
    private function __check_pkg_install($pkg_out, $pkg_rc) {
        # Loop through each line of the pkg output and check for known error messages
        foreach ($pkg_out as $pkg_line) {
            # Check for 'pkg install' no matching package in repository error
            if (APITools\str_starts_with($pkg_line, "pkg: No packages available to install matching")) {
                return 1075;
            }
            # Check for 'pkg install' most recent version is already installed error
            elseif (APITools\str_starts_with($pkg_line, "The most recent versions of packages are already installed")) {
                return 1076;
            }
            # Check for 'pkg add' no package file found at URL error
            elseif ($pkg_line == "pkg: ".$this->validated_data["name"].": Not Found") {
                return 1075;
            }
            # Check for 'pkg add' URL does not contain package file error
            elseif (APITools\str_starts_with($pkg_line, "pkg: archive_read_open_filename")) {
                return 1076;
            }
            # Check for 'pkg add' DNS resolution error
            elseif ($pkg_line == "pkg: ".$this->validated_data["name"].": No address record") {
                return 13;
            }
            # Check for 'pkg add' URL unreachable error
            elseif ($pkg_line == "pkg: ".$this->validated_data["name"].": Network is unreachable") {
                return 13;
            }
            # When no known error messages were matched, but the pkg command still failed, return unexpected error
            elseif ($pkg_rc !== 0) {
                return 1;
            }
            # Otherwise, our package installation appears to be successful
            else {
                return 0;
            }
        }
    }
}
