<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallVirtualIPUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->change_note = "Modified virtual IP via API";
    }

    public function action() {
        APITools\bring_down_virtual_ip($this->config["virtualip"]["vip"][$this->id], $this->id, false);
        $this->config["virtualip"]["vip"][$this->id] = $this->validated_data;
        $this->write_config();
        APITools\apply_virtual_ip($this->validated_data);
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {
        # Validate our required 'id' payload value
        if (isset($this->initial_data['id'])) {
            # Check that our rule ID exists
            if (array_key_exists($this->initial_data["id"], $this->config["virtualip"]["vip"])) {
                $this->id = $this->initial_data['id'];
                $this->validated_data = $this->config["virtualip"]["vip"][$this->id];
            } else {
                $this->errors[] = APIResponse\get(4018);
            }
        } else {
            $this->errors[] = APIResponse\get(4017);
        }

        # Validate our optional 'mode' payload value
        if (isset($this->initial_data['mode'])) {
            $mode_options = array("ipalias", "carp", "proxyarp", "other");
            if (in_array($this->initial_data['mode'], $mode_options)) {
                $previous_mode = $this->validated_data["mode"];
                $this->validated_data["mode"] = $this->initial_data['mode'];
            } else {
                $this->errors[] = APIResponse\get(4023);
            }
        }

        # Validate our optional 'interface' payload value
        if (isset($this->initial_data['interface'])) {
            # Convert this interface into the pfSense interface ID or set error if not found
            $this->initial_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            if (is_string($this->initial_data["interface"])) {
                $this->validated_data["interface"] = $this->initial_data["interface"];
            } else {
                $this->errors[] = APIResponse\get(4024);
            }
        }

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
                }
            } else {
                $this->errors[] = APIResponse\get(4025);
            }
        }

        # Validate our optional 'descr' payload value
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = strval($this->initial_data['descr']);
        }

        # Validate our optional 'noexpand' payload value
        if ($this->initial_data['noexpand'] === true and in_array($this->initial_data["mode"], ["proxyarp", "other"])) {
            $this->validated_data['noexpand'] = "";
        }

        # Validate CARP conditional payload values
        if ($this->validated_data["mode"] === "carp") {
            # Check for our optional 'vhid' payload value. Assume default if none was specified.
            if (isset($this->initial_data['vhid'])) {
                if (vhid_exists($this->initial_data['vhid'])) {
                    $this->errors[] = APIResponse\get(4027);
                } elseif (1 > $this->initial_data['vhid'] or $this->initial_data['vhid'] > 255) {
                    $this->errors[] = APIResponse\get(4028);
                } else {
                    $this->validated_data["vhid"] = $this->initial_data["vhid"];
                }
            } elseif ($previous_mode !== "carp") {
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
            } elseif ($previous_mode !== "carp") {
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
            } elseif ($previous_mode !== "carp") {
                $this->validated_data["advbase"] = 1;
            }

            # Check for our require 'password' payload value. Set error if none was specified.
            if (isset($this->initial_data['password'])) {
                $this->validated_data["password"] = strval($this->initial_data['password']);
            } elseif ($previous_mode !== "carp") {
                $this->errors[] = APIResponse\get(4022);
            }
        }

        # Set virtual IP type to network. This is easier to handle than allow single IPs too.
        $this->validated_data["type"] = "network";
    }
}