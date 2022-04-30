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


class APISystemPackageRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-packagemanager-installed"];
    }

    public function action() {
        return APIResponse\get(0, $this->get_installed_pkgs($this->initial_data["all"]));
    }

    public static function get_installed_pkgs($all=false) {
        # Local variables
        $pkgs = [];
        exec("pkg info", $pkg_out, $pkg_rc);

        # Loop through each of the output lines and format it
        foreach ($pkg_out as $pkg_line) {
            # Only parse this package if it is a pfSense package OR all packages were requested
            if ($all or APITools\str_starts_with("pfSense-pkg", $pkg_line)) {
                # Remove excess whitespace and split the pkg line into an array
                $pkg_line = preg_replace('!\s+!', ' ', $pkg_line);
                $pkg_line = explode(" ", $pkg_line, 2);

                # Extract the pkg description from the line
                $descr = array_pop($pkg_line);

                # Break apart the pkg name and version
                $pkg_line = explode("-", $pkg_line[0]);
                $version = array_pop($pkg_line);
                $name = implode("-", $pkg_line);

                # Add our formatted package to our array
                $pkgs[] = ["name" => $name, "version" => $version, "descr" => $descr];
            }
        }

        return $pkgs;
    }
}
