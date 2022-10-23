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
        $this->delete_gateway_item();
        $this->write_config();

        # Mark the routing subsystem as changed
        mark_subsystem_dirty("staticroutes");
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Require clients to pass in a gateway ID to locate the gateway to update
        if (isset($this->initial_data['id'])) {
            # Check that our ID exists
            if (array_key_exists($this->initial_data['id'], $this->config["gateways"]["gateway_item"])) {
                $this->id = $this->initial_data['id'];
                $this->validated_data = $this->config["gateways"]["gateway_item"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(6028);
            }
        } else {
            $this->errors[] = APIResponse\get(6027);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
    }

    private function delete_gateway_item() {
        $a_gateways = return_gateways_array(true, false, true, true);

        # Loop to obtain the backend gateways array ID
        # TODO: There is likely a better way to do this. Revisit and refactor as needed.
        foreach ($a_gateways as $be_gw_id=>$be_gw_item) {
            # Check if the backend gateways attribute ID matches our client's requested object ID
            if ($be_gw_item["attribute"] === $this->id) {
                $backend_gw = $be_gw_item;
                break;
            }
        }

        if (!isset($backend_gw)) {
            return;
        }

        # When deleting the default routing gateway, remove the default route
        if (!empty($backend_gw) && is_ipaddr($backend_gw['gateway'])) {
            if (!isset($backend_gw['disabled'])) {
                if (isset($backend_gw['isdefaultgw'])) {
                    $inet = (!is_ipaddrv4($backend_gw['gateway']) ? '-inet6' : '-inet');
                    file_put_contents("/dev/console", "\n[" . getmypid() . "] DEL_GW, route= delete {$inet} default");
                    mwexec("/sbin/route delete {$inet} default");
                }
            }
        }

        # Remove static routes that depend on this gateway
        if (!empty($backend_gw) && is_ipaddr($backend_gw['gateway'])) {
            if (isset($backend_gw["nonlocalgateway"])) {
                $realif = get_real_interface($backend_gw['interface']);
                $inet = (!is_ipaddrv4($backend_gw['gateway']) ? "-inet6" : "-inet");
                file_put_contents("/dev/console", "\n[".getmypid()."] DEL_GW, route= $inet " . escapeshellarg($backend_gw['gateway']) . " -iface " . escapeshellarg($realif));
                $cmd = "/sbin/route delete $inet " . escapeshellarg($backend_gw['gateway']) . " -iface " . escapeshellarg($realif);
                mwexec($cmd);
            }
        }

        # Remove static routes that were created for the monitor IP
        if (!empty($backend_gw['monitor']) && $backend_gw['monitor'] != "dynamic") {
            if (is_ipaddr($backend_gw['monitor']) && $backend_gw['gateway'] != $backend_gw['monitor']) {
                if (is_ipaddrv4($backend_gw['monitor'])) {
                    mwexec("/sbin/route delete " . escapeshellarg($backend_gw['monitor']));
                } else {
                    mwexec("/sbin/route delete -inet6 " . escapeshellarg($backend_gw['monitor']));
                }
            }
        }

        # Remove this gateway from any existing interface
        if ($this->config['interfaces'][$backend_gw['friendlyiface']]['gateway'] == $backend_gw['name']) {
            unset($this->config['interfaces'][$backend_gw['friendlyiface']]['gateway']);
        }
        unset($this->config['gateways']['gateway_item'][$backend_gw['attribute']]);
    }

    public function apply() {
        # Apply these changes immediately if requested
        if ($this->initial_data["apply"] === true) {
            APIRoutingApplyCreate::apply();
        }
    }
}
