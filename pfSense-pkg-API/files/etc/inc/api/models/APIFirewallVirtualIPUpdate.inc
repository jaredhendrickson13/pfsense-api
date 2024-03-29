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

class APIFirewallVirtualIPUpdate extends APIModel {
    private $__prev_mode;
    
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->change_note = "Modified virtual IP via API";
    }

    public function action() {
        # Serialize the original entry
        $this->__serialize_vip();

        # Update this virtual IP to config and mark the changes as pending
        $this->set_config("virtualip/vip/{$this->id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty('vip');

        # Apply the changes
        applyVIP();

        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        $this->__validate_id();
        $this->__validate_mode();
        $this->__validate_interface();
        $this->__validate_subnet();
        $this->__validate_descr();
        $this->__validate_noexpand();
        $this->__validate_vhid();
        $this->__validate_advbase();
        $this->__validate_advskew();
        $this->__validate_password();
    }

    private function __validate_id() {
        # Validate our required 'id' payload value
        if (isset($this->initial_data['id'])) {
            # Check that our rule ID exists
            if ($this->get_config("virtualip/vip/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data['id'];
                $this->validated_data = $this->get_config("virtualip/vip/{$this->id}");
            } else {
                $this->errors[] = APIResponse\get(4018);
            }
        } else {
            $this->errors[] = APIResponse\get(4017);
        }
    }

    private function __validate_mode() {
        # Validate our optional 'mode' payload value
        if (isset($this->initial_data['mode'])) {
            $mode_options = array("ipalias", "carp", "proxyarp", "other");
            if (in_array($this->initial_data['mode'], $mode_options)) {
                $this->__prev_mode = $this->validated_data["mode"];
                $this->validated_data["mode"] = $this->initial_data['mode'];
            } else {
                $this->errors[] = APIResponse\get(4023);
            }
        }
    }

    private function __validate_interface() {
        # Validate our optional 'interface' payload value
        if (isset($this->initial_data['interface'])) {
            # Convert this interface into the pfSense interface ID or set error if not found
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"], true);
            if (is_string($this->initial_data["interface"])) {
                $this->validated_data["interface"] = $this->initial_data["interface"];
            } else {
                $this->errors[] = APIResponse\get(4024);
            }
        }
    }

    private function __validate_subnet() {
        # Validate our optional 'subnet' payload value
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

                    # Set virtual IP type to network/single based on subnet and subnet_bits
                    if (is_ipaddrv4($this->validated_data["subnet"]) && $this->validated_data["subnet_bits"] == 32) {
                        $this->validated_data["type"] = "single";
                    } elseif (is_ipaddrv6($this->validated_data["subnet"]) && $this->validated_data["subnet_bits"] == 128) {
                        $this->validated_data["type"] = "single";
                    } else {
                        $this->validated_data["type"] = "network";
                    }
                }
            } else {
                $this->errors[] = APIResponse\get(4025);
            }
        }
    }

    private function __validate_descr() {
        # Validate our optional 'descr' payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = strval($this->initial_data['descr']);
        }
    }

    private function __validate_noexpand() {
        # Validate our optional 'noexpand' payload value
        if ($this->initial_data['noexpand'] === true and in_array($this->initial_data["mode"], ["proxyarp", "other"])) {
            $this->validated_data['noexpand'] = "";
        }
    }

    private function __validate_vhid() {
        # Only validate this field if the mode is set to CARP
        if ($this->validated_data["mode"] === "carp") {
            # Check for our optional 'vhid' payload value. Assume default if none was specified.
            if (isset($this->initial_data['vhid'])) {
                if ($this->__vhid_exists($this->initial_data['vhid'])) {
                    $this->errors[] = APIResponse\get(4027);
                } elseif (1 > $this->initial_data['vhid'] or $this->initial_data['vhid'] > 255) {
                    $this->errors[] = APIResponse\get(4028);
                } else {
                    $this->validated_data["vhid"] = $this->initial_data["vhid"];
                }
            } elseif ($this->__prev_mode !== "carp") {
                # If a VHID was not specified, use the next available VHID
                $this->validated_data["vhid"] = APITools\next_vhid();
            }
        }
    }

    private function __validate_advskew() {
        # Only validate this field if the mode is set to CARP
        if ($this->validated_data["mode"] === "carp") {
            # Check for our optional 'advskew' payload value. Assume default if none was specified.
            if (isset($this->initial_data['advskew'])) {
                # Ensure 'advskew' value is within range
                if (0 > $this->initial_data['advskew'] or $this->initial_data['advskew'] > 254) {
                    $this->errors[] = APIResponse\get(4030);
                } else {
                    $this->validated_data["advskew"] = intval($this->initial_data['advskew']);
                }
            } elseif ($this->__prev_mode !== "carp") {
                $this->validated_data["advskew"] = 0;
            }
        }
    }

    private function __validate_advbase() {
        # Only validate this field if the mode is set to CARP
        if ($this->validated_data["mode"] === "carp") {
            # Check for our optional 'advbase' payload value. Assume default if none was specified.
            if (isset($this->initial_data['advbase'])) {
                # Ensure 'advbase' value is within range
                if (1 > $this->initial_data['advbase'] or $this->initial_data['advbase'] > 254) {
                    $this->errors[] = APIResponse\get(4029);
                } else {
                    $this->validated_data["advbase"] = intval($this->initial_data['advbase']);
                }
            } elseif ($this->__prev_mode !== "carp") {
                $this->validated_data["advbase"] = 1;
            }
        }
    }

    private function __validate_password() {
        # Validate CARP conditional payload values
        if ($this->validated_data["mode"] === "carp") {
            # Check for our require 'password' payload value. Set error if none was specified.
            if (isset($this->initial_data['password'])) {
                $this->validated_data["password"] = strval($this->initial_data['password']);
            } elseif ($this->__prev_mode !== "carp") {
                $this->errors[] = APIResponse\get(4022);
            }
        }
    }

    private function __vhid_exists($vhid) {
        # Loop through each virtual IP and ensure it is not using the requested vhid
        foreach ($this->get_config("virtualip/vip", []) as $vip) {
            if (intval($vhid) === intval($vip["vhid"]) and intval($vhid) !== intval($this->validated_data["vhid"])) {
                return true;
            }
        }
        return false;
    }

    private function __serialize_vip() {
        global $g;

        # Retain existing serialized virtual IPs if present
        if (file_exists("{$g['tmp_path']}/.firewall_virtual_ip.apply")) {
            $vips_to_apply = unserialize(file_get_contents("{$g['tmp_path']}/.firewall_virtual_ip.apply"));
            $vips_to_apply[$this->id] = $this->get_config("virtualip/vip/{$this->id}");
        }
        else {
            $vips_to_apply = [$this->id => $this->get_config("virtualip/vip/{$this->id}")];
        }

        # Write the pending changes to file
        file_put_contents("{$g['tmp_path']}/.firewall_virtual_ip.apply", serialize($vips_to_apply));
    }
}
