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

class APIRoutingGatewayDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-gateways-editgateway"];
        $this->change_note = "Deleted gateway via API";
    }

    public function action() {
        $this->validated_data = $this->config["gateways"]["gateway_item"][$this->id];
        $this->delete_gateway_item();
        $this->write_config();
        system_routing_configure();
        system_resolvconf_generate();
        filter_configure();
        setup_gateways_monitor();
        send_event("service reload dyndnsall");

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

        if (!isset($a_gateways[$this->id])) {
            return;
        }

        # When deleting the default routing gateway, remove the default route
        if (!empty($a_gateways[$this->id]) && is_ipaddr($a_gateways[$this->id]['gateway'])) {
            if (!isset($a_gateways[$this->id]['disabled'])) {
                if (isset($a_gateways[$this->id]['isdefaultgw'])) {
                    $inet = (!is_ipaddrv4($a_gateways[$this->id]['gateway']) ? '-inet6' : '-inet');
                    file_put_contents("/dev/console", "\n[" . getmypid() . "] DEL_GW, route= delete {$inet} default");
                    mwexec("/sbin/route delete {$inet} default");
                }
            }
        }

        # Remove static routes that depend on this gateway
        if (!empty($a_gateways[$this->id]) && is_ipaddr($a_gateways[$this->id]['gateway'])) {
            if (isset($a_gateways[$this->id]["nonlocalgateway"])) {
                $realif = get_real_interface($a_gateways[$this->id]['interface']);
                $inet = (!is_ipaddrv4($a_gateways[$this->id]['gateway']) ? "-inet6" : "-inet");
                file_put_contents("/dev/console", "\n[".getmypid()."] DEL_GW, route= $inet " . escapeshellarg($a_gateways[$this->id]['gateway']) . " -iface " . escapeshellarg($realif));
                $cmd = "/sbin/route delete $inet " . escapeshellarg($a_gateways[$this->id]['gateway']) . " -iface " . escapeshellarg($realif);
                mwexec($cmd);
            }
        }

        # Remove static routes that were created for the monitor IP
        if (!empty($a_gateways[$this->id]['monitor']) && $a_gateways[$this->id]['monitor'] != "dynamic") {
            if (is_ipaddr($a_gateways[$this->id]['monitor']) && $a_gateways[$this->id]['gateway'] != $a_gateways[$this->id]['monitor']) {
                if (is_ipaddrv4($a_gateways[$this->id]['monitor'])) {
                    mwexec("/sbin/route delete " . escapeshellarg($a_gateways[$this->id]['monitor']));
                } else {
                    mwexec("/sbin/route delete -inet6 " . escapeshellarg($a_gateways[$this->id]['monitor']));
                }
            }
        }

        # Remove this gateway from any existing interface
        if ($this->config['interfaces'][$a_gateways[$this->id]['friendlyiface']]['gateway'] == $a_gateways[$this->id]['name']) {
            unset($this->config['interfaces'][$a_gateways[$this->id]['friendlyiface']]['gateway']);
        }
        unset($this->config['gateways']['gateway_item'][$a_gateways[$this->id]['attribute']]);
    }
}