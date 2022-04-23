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

class APIUserGroupRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-groupmanager"];
    }

    public function action() {
        return APIResponse\get(0, $this->__to_representation($this->config["system"]["group"]));
    }

    private function __to_representation($groups) {
        # Loop through each group and convert various values to their expected types
        foreach($groups as $id=>$group) {
            # Ensure GIDs are integers
            $groups[$id]["gid"] = intval($groups[$id]["gid"]);

            # Ensure each member item is an integer
            if ($groups[$id]["member"]) {
                foreach ($groups[$id]["member"] as $member_id=>$member) {
                    $groups[$id]["member"][$member_id] = intval($member);
                }
            } else {
                $groups[$id]["member"] = [];
            }

            # Ensure each priv item is a string
            if ($groups[$id]["priv"]) {
                foreach ($groups[$id]["priv"] as $priv_id=>$priv) {
                    $groups[$id]["priv"][$priv_id] = strval($priv);
                }
            } else {
                $groups[$id]["priv"] = [];
            }
        }

        return $groups;
    }
}
