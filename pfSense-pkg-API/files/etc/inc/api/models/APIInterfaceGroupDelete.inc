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


class APIInterfaceGroupDelete extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
    }

    public function action() {
        $this->del_config("ifgroups/ifgroupentry/{$this->id}");
        $this->write_config();
        pfSense_interface_destroy($this->validated_data["ifname"]);
        return APIResponse\get(0, $this->validated_data);
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
    }
}
