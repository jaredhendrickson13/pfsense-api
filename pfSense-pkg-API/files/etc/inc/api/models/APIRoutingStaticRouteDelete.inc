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


class APIRoutingStaticRouteDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Deleted static route via API";
    }

    public function action() {
        # Remove this static route
        delete_static_route($this->id, true);
        $del_data = $this->del_config("staticroutes/route/{$this->id}");
        $this->write_config();

        # Mark the routing subsystem as changed
        mark_subsystem_dirty("staticroutes");
        $this->apply();

        return APIResponse\get(0, $del_data);
    }

    public function validate_payload() {
        # Check that this static route ID exists
        if (isset($this->initial_data["id"])) {
            if ($this->get_config("staticroutes/route/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
            } else {
                $this->errors[] = APIResponse\get(6006);
            }
        } else {
            $this->errors[] = APIResponse\get(6005);
        }
    }

    public function apply() {
        # Apply these changes immediately if requested
        if ($this->initial_data["apply"] === true) {
            APIRoutingApplyCreate::apply();
        }
    }
}
