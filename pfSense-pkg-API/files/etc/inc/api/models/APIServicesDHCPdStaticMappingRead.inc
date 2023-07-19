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


class APIServicesDHCPdStaticMappingRead extends APIModel {
    # Create our method constructor
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
    }

    public function action() {
        $mappings = [];
        # Check that we have a mappings array
        if (is_array($this->get_config("dhcpd/{$this->id}/staticmap"))) {
            # Loop through each static map and ensure its ID is assign to our response
            foreach ($this->get_config("dhcpd/{$this->id}/staticmap", []) as $id=>$mapping) {
                $mappings[] = array_merge(["id" => $id], $mapping);
            }
        }
        return APIResponse\get(0, $mappings);
    }

    public function validate_payload() {
        # Check for our required 'interface' payload value
        if ($this->initial_data["interface"]) {
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            # Check that this interface has a DHCPd configuration
            if ($this->get_config("dhcpd/{$this->initial_data["interface"]}", false)) {
                $this->id = $this->initial_data["interface"];
            } else {
                $this->errors[] = APIResponse\get(2018);
            }
        } else {
            $this->errors[] = APIResponse\get(2017);
        }
    }
}
