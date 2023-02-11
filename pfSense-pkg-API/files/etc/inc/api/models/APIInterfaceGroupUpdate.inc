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


class APIInterfaceGroupUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-groups-edit"];
    }

    public function action() {
        $this->set_config("ifgroups/ifgroupentry/{$this->id}", $this->validated_data);
        $this->write_config();
        return APIResponse\get(0, $this->validated_data);
    }
    
    private function __validate_members() {
        # Require clients to specify at least 1 group member
        if (isset($this->initial_data['members'])) {
            # Default validated members
            $members = [];
            
            # If non-array was passed, convert it into an array
            if (!is_array($this->initial_data['members'])) {
                $this->initial_data['members'] = [$this->initial_data['members']];
            }

            # Loop through each member and validate it
            foreach ($this->initial_data['members'] as $member) {
                # Convert the member to the pfSense ID (wan, lan, optx, etc)
                $member_if_id = APITools\get_pfsense_if_id($member);

                # Ensure the interface exists
                if (!$member_if_id) {
                    $this->errors[] = APIResponse\get(3074, $member);
                }
                # If no previous conditions were met, the member is valid.
                else {
                    $members[] = $member_if_id;
                }
            }

            # Convert members to a space separated string as expected by pfSense
            $this->validated_data["members"] = implode(" ", $members);
        } else {
            $this->errors[] = APIResponse\get(3074, $member);
        }
    }

    private function __validate_descr() {
        # Optionally allow client to specify a group description. Default if not set.
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
    }
    
    private function __validate_id() {
        if (isset($this->initial_data['id'])) {
            if (APITools\is_ifgroup($this->initial_data['id'])) {
                foreach ($this->get_config("ifgroups/ifgroupentry", []) as $id=>$ifgroup) {
                    # Check if this group matches
                    if ($this->initial_data["id"] === $ifgroup["ifname"]) {
                        $this->validated_data = $ifgroup;
                        $this->id = $id;
                    }
                }
                # If no ID was identified, throw a group not found error
                if (!is_numeric($this->id)) {
                    $this->errors[] = APIResponse\get(3080);
                }
            } else {
                $this->errors[] = APIResponse\get(3080);
            }
        } else {
            $this->errors[] = APIResponse\get(3079);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
        $this->__validate_members();
        $this->__validate_descr();
    }
}
