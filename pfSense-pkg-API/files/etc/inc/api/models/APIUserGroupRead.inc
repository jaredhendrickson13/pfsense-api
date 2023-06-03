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

class APIUserGroupRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-groupmanager"];
    }

    public function action() {
        return APIResponse\get(0, $this->to_representation($this->get_config("system/group", []), true));
    }

    public static function to_representation($groups, $multiple=false) {
        # Convert a list of groups to representation
        if ($multiple) {
            $representation = [];
            # Loop through each group and convert various values to their expected types
            foreach($groups as $group) {
                $representation[] =  self::__type_map($group);
            }
        }
        # Convert a single group to representation
        else {
            $representation = self::__type_map($groups);
        }

        return $representation;
    }

    private static function __type_map($group) {
        # Ensure GIDs are integers
        $group["gid"] = intval($group["gid"]);

        # Ensure each member item is an integer
        if ($group["member"]) {
            foreach ($group["member"] as $member_id=>$member) {
                $group["member"][$member_id] = intval($member);
            }
        } else {
            $group["member"] = [];
        }

        # Ensure each priv item is a string
        if ($group["priv"]) {
            foreach ($group["priv"] as $priv_id=>$priv) {
                $group["priv"][$priv_id] = strval($priv);
            }
        } else {
            $group["priv"] = [];
        }

        return $group;
    }
}
