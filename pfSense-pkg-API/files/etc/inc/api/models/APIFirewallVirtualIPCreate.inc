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

class APIFirewallVirtualIPCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->change_note = "Added virtual IP via API";
    }

    public function action() {
        # Ensure our config has virtual IP arrays initialized
        if (!is_array($this->config["virtualip"]["vip"])) {
            $this->config["virtualip"] = array("vip" => []);
        }

        $this->config["virtualip"]["vip"][] = $this->validated_data;
        $this->write_config();
        APITools\apply_virtual_ip($this->validated_data);
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {

        # Validate our required 'mode' payload value
        if (isset($this->initial_data['mode'])) {
            $mode_options = array("ipalias", "carp", "proxyarp", "other");
            if (in_array($this->initial_data['mode'], $mode_options)) {
                $this->validated_data["mode"] = $this->initial_data['mode'];
            } else {
                $this->errors[] = APIResponse\get(4023);
            }
        } else {
            $this->errors[] = APIResponse\get(4019);
        }

        # Validate our required 'interface' payload value
        if (isset($this->initial_data['interface'])) {
            # Convert this interface into the pfSense interface ID or set error if not found
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            if (is_string($this->initial_data["interface"])) {
                $this->validated_data["interface"] = $this->initial_data["interface"];
            } else {
                $this->errors[] = APIResponse\get(4024);
            }
        } else {
            $this->errors[] = APIResponse\get(4020);
        }

        # Validate our required 'subnet' payload value
        if (isset($this->initial_data['subnet'])) {
            // If a single IPv4 or IPv6, append the subnet mask for one address
            if (is_ipaddrv4($this->initial_data['subnet'])) {
                $this->initial_data['subnet'] = $this->initial_data['subnet']."/32";
            } elseif (is_ipaddrv6($this->initial_data['subnet'])) {
                $this->initial_data['subnet'] = $this->initial_data['subnet']."/128";
            }
            # Check that we now have a valid subnet CIDR
            if (is_subnet($this->initial_data['subnet'])) {
                $subnet_split = explode("/", $this->initial_data['subnet']);
                # Check that our subnet is not used elsewhere
                if (APITools\is_ip_in_use($subnet_split[0])) {
                    $this->errors[] = APIResponse\get(4026);
                } else {
                    $this->validated_data["subnet"] = $subnet_split[0];
                    $this->validated_data["subnet_bits"] = $subnet_split[1];
                }
            } else {
                $this->errors[] = APIResponse\get(4025);
            }

        } else {
            $this->errors[] = APIResponse\get(4021);
        }

        # Validate our required 'descr' payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = strval($this->initial_data['descr']);
        } else {
            $this->validated_data["descr"] = "";
        }

        # Validate our required 'noexpand' payload value
        if ($this->initial_data['noexpand'] === true and in_array($this->initial_data["mode"], ["proxyarp", "other"])) {
            $this->validated_data['noexpand'] = "";
        }

        # Validate CARP conditional payload values
        if ($this->validated_data["mode"] === "carp") {
            # Check for our optional 'vhid' payload value. Assume default if none was specified.
            if (isset($this->initial_data['vhid'])) {
                if ($this->__vhid_exists($this->initial_data["interface"], $this->initial_data['vhid'])) {
                    $this->errors[] = APIResponse\get(4027);
                } elseif (1 > $this->initial_data['vhid'] or $this->initial_data['vhid'] > 255) {
                    $this->errors[] = APIResponse\get(4028);
                } else {
                    $this->validated_data["vhid"] = $this->initial_data["vhid"];
                }
            } else {
                # If a VHID was not specified, use the next available VHID
                $this->validated_data["vhid"] = APITools\next_vhid();
            }

            # Check for our optional 'advskew' payload value. Assume default if none was specified.
            if (isset($this->initial_data['advskew'])) {
                # Ensure 'advskew' value is within range
                if (0 > $this->initial_data['advskew'] or $this->initial_data['advskew'] > 254) {
                    $this->errors[] = APIResponse\get(4030);
                } else {
                    $this->validated_data["advskew"] = intval($this->initial_data['advskew']);
                }
            } else {
                $this->validated_data["advskew"] = 0;
            }

            # Check for our optional 'advbase' payload value. Assume default if none was specified.
            if (isset($this->initial_data['advbase'])) {
                # Ensure 'advbase' value is within range
                if (1 > $this->initial_data['advbase'] or $this->initial_data['advbase'] > 254) {
                    $this->errors[] = APIResponse\get(4029);
                } else {
                    $this->validated_data["advbase"] = intval($this->initial_data['advbase']);
                }
            } else {
                $this->validated_data["advbase"] = 1;
            }

            # Check for our require 'password' payload value. Set error if none was specified.
            if (isset($this->initial_data['password'])) {
                $this->validated_data["password"] = strval($this->initial_data['password']);
            } else {
                $this->errors[] = APIResponse\get(4022);
            }
        }

        # Set virtual IP type to network/single based on subnet and subnet_bits
        if (is_ipaddrv4($this->validated_data["subnet"]) && $this->validated_data["subnet_bits"] == 32) {
          $this->validated_data["type"] = "single";
        } elseif (is_ipaddrv6($this->validated_data["subnet"]) && $this->validated_data["subnet_bits"] == 128) {
          $this->validated_data["type"] = "single";
        } else {
          $this->validated_data["type"] = "network";
        }
    }

    private function __vhid_exists($interface, $vhid) {
        # Loop through each virtual IP and ensure it is not using the requested vhid
        foreach ($this->config["virtualip"]["vip"] as $vip) {
            if ($interface === $vip["interface"] && intval($vhid) === intval($vip["vhid"])) {
                return true;
            }
        }
        return false;
    }

}
