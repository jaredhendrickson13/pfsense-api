<?php
//   Copyright 2021 Jared Hendrickson
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


class APISystemAPIVersionRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
    }

    public function action() {
        # Add API version context
        $api_version["current_version"] = self::get_api_version();
        $api_version["latest_version"] = self::get_latest_api_version();
        $api_version["update_available"] = self::is_update_available();

        return APIResponse\get(0, $api_version);
    }

    public static function get_api_version() {
        # Pull the raw pkg info for the API package into an array for each line
        $pkg_info = explode(PHP_EOL, shell_exec("pkg info pfSense-pkg-API"));

        # Loop through each line and check the version
        foreach ($pkg_info as $pkg_line) {
            if (APITools\str_starts_with("Version", $pkg_line)) {
                # Locate the version and format it to a standard semantic version format (x.x.x)
                $version = str_replace(" ", "", $pkg_line);
                $version = explode(":", $version)[1];
                $version = str_replace("_", ".", $version);
                return $version;
            }
        }
    }

    public static function get_latest_api_version() {
        # Pull the current API version from Github's API
        $api_resp = shell_exec("curl -s https://api.github.com/repos/jaredhendrickson13/pfsense-api/releases/latest");

        # Format the version to a standard semantic format (x.x.x)
        $version = json_decode($api_resp, true)["tag_name"];
        $version = str_replace("v", "", $version);
        return $version;
    }

    public static function is_update_available() {
        # Check if the current version is less than the latest version
        $curr_ver_num = intval(str_replace(".", "", self::get_api_version()));
        $latest_ver_num = intval(str_replace(".", "", self::get_latest_api_version()));
        return $curr_ver_num < $latest_ver_num;
    }
}
