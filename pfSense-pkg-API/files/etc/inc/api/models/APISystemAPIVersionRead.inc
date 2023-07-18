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
        $pkg_info = explode(PHP_EOL, shell_exec("pkg-static info pfSense-pkg-API"));

        # Loop through each line and check the version
        foreach ($pkg_info as $pkg_line) {
            if (str_starts_with($pkg_line, "Version")) {
                # Locate the version and format it to a standard semantic version format (x.x.x)
                $version = str_replace(" ", "", $pkg_line);
                $version = explode(":", $version)[1];
                $version = (strlen($version) === 3) ? $version.".0" : $version;
                $version = str_replace("_", ".", $version);
                return "v{$version}";
            }
        }
    }

    public static function get_latest_api_version() {
        # Fetch our latest version and format it semantically (x.x.x)
        return array_key_first(self::get_all_available_versions());
    }

    public static function get_latest_api_release_date() {
        # Loop through each release and locate the latest available releases creation date
        foreach (self::get_github_releases()["releases"] as $release) {
            # Check if this releases is latest available for our platform
            if ($release["tag_name"] === self::get_latest_api_version()) {
                return $release["created_at"];
            }
        }
    }

    public static function is_update_available() {
        # Check if the current version is less than the latest version
        $curr_ver_num = intval(str_replace(".", "", self::get_api_version()));
        $latest_ver_num = intval(str_replace(".", "", self::get_latest_api_version()));
        return $curr_ver_num < $latest_ver_num;
    }

    public static function get_github_releases() {
        # Variables
        $releases_file = "/usr/local/share/pfSense-pkg-API/releases.json";
        $releases = json_decode(file_get_contents($releases_file), true);
        $fetch_releases_cmd = "curl -s https://api.github.com/repos/jaredhendrickson13/pfsense-api/releases -m 10";

        # Check if our previous releases data is older than 120 seconds
        if ($releases["last_updated"] + 120 < time()) {
            # Pull the releases data from Github and rewrite the releases file
            $api_resp = shell_exec($fetch_releases_cmd);
            $api_resp = json_decode($api_resp, true);
            $releases = ["last_updated" => time(), "releases" => $api_resp];
            file_put_contents($releases_file, json_encode($releases));
        }
        return $releases;
    }

    public static function get_all_available_versions() {
        # Variables
        $pf_version = APITools\get_pfsense_version()["base"];
        $pkg_config = APITools\get_api_config()[1];
        $versions = [];
        $count = 0;
        $releases = self::get_github_releases()["releases"];

        # Loop through each version and populate our version data
        foreach ($releases as $release) {
            # Check if this release is a pre-release, if so, only include it if allow_pre_release is enabled
            if ($release["prerelease"] === false or isset($pkg_config["allow_pre_release"])) {
                # Loop through the assets of each release and check if our version of pfSense is supported
                foreach ($release["assets"] as $asset) {
                    if (str_starts_with($asset["name"], "pfSense-" . $pf_version)) {
                        # The first item of our list is the latest release, mark it as such.
                        if ($count === 0) {
                            $versions[$release["tag_name"]] = $release["name"] . " - Latest";
                            $count++;
                        } else {
                            $versions[$release["tag_name"]] = $release["name"];
                        }
                    }
                }
            }
        }
        return $versions;
    }
}
