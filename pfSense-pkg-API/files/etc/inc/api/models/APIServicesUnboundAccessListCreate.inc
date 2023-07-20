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


class APIServicesUnboundAccessListCreate extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
        $this->change_note = "Added DNS Resolver access list via API";
    }

    public function action() {
        $this->id = $this->get_next_id("unbound/acls");
        $this->set_config("unbound/acls/{$this->id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty("unbound");
        # If user requests immediately application
        if ($this->initial_data['apply'] === true) {
            APITools\unbound_reload_config();
        }
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        $this->__validate_aclname();
        $this->__validate_aclaction();
        $this->__validate_description();
        $this->__validate_row();
    }

    private function __validate_aclname() {
        # Check for our optional 'aclname' payload value
        if (isset($this->initial_data['aclname'])) {
            $this->validated_data["aclname"] = strval($this->initial_data['aclname']);
        } else {
            $this->validated_data["aclname"] = "";
        }
    }

    private function __validate_aclaction() {
        # Local variables
        $acl_actions = ["allow", "deny", "refuse", "allow snoop", "deny nonlocal", "refuse nonlocal"];

        # Check for our required 'aclaction' payload value
        if (isset($this->initial_data['aclaction'])) {
            # Check that our aclaction is of a known type
            if (in_array($this->initial_data["aclaction"], $acl_actions)) {
                $this->validated_data["aclaction"] = $this->initial_data['aclaction'];
            } else {
                $this->errors[] = APIResponse\get(2067);
            }
        } else {
            $this->errors[] = APIResponse\get(2066);
        }
    }

    private function __validate_description() {
        # Check for our optional 'description' payload value
        if (isset($this->initial_data['description'])) {
            $this->validated_data["description"] = $this->initial_data['description'];
        } else {
            $this->validated_data["description"] = "";
        }
    }

    private function __validate_row() {
        # Init our ACL row array
        $this->validated_data["row"] = [];

        # Check for our required 'row' field
        if (isset($this->initial_data["row"])) {
            # Check that our array has a minimum of 1 row entry
            if (count($this->initial_data["row"]) >= 1) {
                # Validate each individual row entry
                foreach ($this->initial_data["row"] as $row) {
                    # Run a nested API call to the APIServicesUnboundAccessListRowCreate model
                    $create_row = new APIServicesUnboundAccessListRowCreate($this->initial_data);
                    $create_row->initial_data = $row;
                    $create_row->__validate_acl_network();
                    $create_row->__validate_mask();
                    $create_row->__validate_description();

                    # Append any identified errors to this models errors
                    $this->errors = $this->errors + $create_row->errors;

                    # Append this row entry if no errors were found
                    if (!$create_row->errors) {
                        $this->validated_data["row"][] = $row;
                    }
                }
            } else {
                $this->errors[] = APIResponse\get(2073);
            }
        } else {
            $this->errors[] = APIResponse\get(2073);
        }
    }
}
