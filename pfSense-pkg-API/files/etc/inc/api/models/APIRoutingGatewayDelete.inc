<?php
//   Copyright 2022 Jared Hendrickson
//
//   Accreditation for delete_gateway_item():
//   part of pfSense (https://www.pfsense.org)
//   Copyright (c) 2004-2013 BSD Perimeter
//   Copyright (c) 2013-2016 Electric Sheep Fencing
//   Copyright (c) 2014-2022 Rubicon Communications, LLC (Netgate)
//   Copyright (c) 2010 Seth Mos <seth.mos@dds.nl>
//   All rights reserved.
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

class APIRoutingGatewayDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-gateways-editgateway"];
        $this->change_note = "Deleted gateway via API";
        $this->init_config('gateways', 'gateway_item');
    }

    public function action() {
        # Write these changes to config
        $this->validated_data = $this->config["gateways"]["gateway_item"][$this->id];
        $this->delete_gateway();
        $this->write_config();

        # Mark the routing subsystem as changed
        mark_subsystem_dirty("staticroutes");
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Require clients to pass in a gateway ID to locate the gateway to update
        if (isset($this->initial_data['id'])) {
            # Check if an existing gateway's numeric ID matches this ID
            if (array_key_exists($this->initial_data['id'], $this->config["gateways"]["gateway_item"])) {
                $this->id = $this->initial_data['id'];
                $this->validated_data = $this->config["gateways"]["gateway_item"][$this->id];
            }
            # Check if the ID passed in is a gateway name
            elseif (is_numeric($this->get_id_by_name($this->initial_data["id"]))) {
                $this->id = $this->get_id_by_name($this->initial_data["id"]);
                $this->validated_data = $this->config["gateways"]["gateway_item"][$this->id];
            }
            else {
                $this->errors[] = APIResponse\get(6028);
            }
        } else {
            $this->errors[] = APIResponse\get(6027);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
    }

    # Gets the numeric ID of a gateway by it's name
    public function get_id_by_name($name) {
        # Loop through all existing gateways and check if this name matches
        foreach ($this->config["gateways"]["gateway_item"] as $id=>$gw) {
            # Check if this name matches
            if ($name === $gw["name"]) {
                return $id;
            }
        }
    }

    public function delete_gateway() {
        # Local variables
        $gw_data = return_gateways_array(true, false, true, true);

        # Loop through the gateway days until we find the gateway to delete
        foreach ($gw_data as $gw) {
            # Check if this gateways 'attribute' matches the ID of the gateway we're deleting
            if ($this->id === $gw["attribute"]) {
                # Remove the gateway and monitor IPs
                route_del($gw["gateway"], $gw["ipprotocol"]);
                route_del($gw["monitor"], $gw["ipprotocol"]);

                # Delete the default route if this gateway is the default gateway
                if ($gw["isdefaultgw"]) {
                    route_del("default", $gw["ipprotocol"]);
                }
            }
        }

        # Remove this gateway from config
        unset($this->config["gateways"]["gateway_item"][$this->id]);
    }

    public function apply() {
        # Apply these changes immediately if requested
        if ($this->initial_data["apply"] === true) {
            APIRoutingApplyCreate::apply();
        }
    }
}
