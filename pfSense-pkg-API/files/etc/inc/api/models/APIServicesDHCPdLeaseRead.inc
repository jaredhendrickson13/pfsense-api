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


class APIServicesDHCPdLeaseRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-dhcpleases"];
    }

    public function action() {
        return APIResponse\get(0, $this->__to_representation(system_get_dhcpleases()["lease"]));
    }

    # Formats the leases array to be more REST like
    private function __to_representation($leases) {
        # Loop through each array item and change the online/offline keywords to True/False accordingly
        # Also change the 'act' key to 'state'
        foreach ($leases as $id=>$lease) {
            # Change the 'online' keyword to true
            if ($lease["online"] === "online") {
                $leases[$id]["online"] = true;
            }
            # Otherwise change the 'offline' keyword to false
            else {
                $leases[$id]["online"] = false;
            }

            # Change the 'act' key to 'state'
            $leases[$id]["state"] = $leases[$id]["act"];
            unset($leases[$id]["act"]);
        }

        return $leases;
    }
}
