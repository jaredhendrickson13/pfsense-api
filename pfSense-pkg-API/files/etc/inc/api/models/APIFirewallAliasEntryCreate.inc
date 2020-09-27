<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallAliasEntryCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
        $this->change_note = "Added firewall alias address via API";
    }

    public function action() {
        // Add our alias
        $this->config["aliases"]["alias"][$this->initial_data["id"]]["address"] = implode(" ", $this->validated_data["address"]);
        $this->config["aliases"]["alias"][$this->initial_data["id"]]["detail"] = implode("||", $this->validated_data["detail"]);
        $this->write_config();    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $this->config["aliases"]["alias"][$this->initial_data["id"]]);
    }
    
    public function validate_payload() {

        if (isset($this->initial_data['name'])) {
            $name = $this->initial_data['name'];
            $name = APITools\sanitize_str($name);
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
        if (isset($this->initial_data['address'])) {
            $address = $this->initial_data['address'];
            // Convert string to array
            if (!is_array($address)) {
                $address = array($address);
            }
        } else {
            $this->errors[] = APIResponse\get(4052);

        }
        if (isset($this->initial_data['detail'])) {
            $detail = $this->initial_data['detail'];
            // Convert string to array
            if (!is_array($detail)) {
                $detail = array($detail);
            }
        }
        // Check that our input is valid
        if (!is_string($name)) {
            $this->errors[] = APIResponse\get(4053);
        } elseif (!is_array($address)) {
            $this->errors[] = APIResponse\get(4054);
        } elseif (isset($detail) and !is_array($detail)) {
            $this->errors[] = APIResponse\get(4064);
        }
        // Loop through our existing firewall entries and check for our requested alias
        $c_count = 0;
        foreach ($this->config["aliases"]["alias"] as $ce) {
            if ($name === $ce["name"]) {
                $this->initial_data["id"] = $c_count;
                $type = $ce["type"];
                $curr_addr = explode(" ", $ce["address"]);
                $curr_detail = explode("||", $ce["detail"]);
                break;
            }
            $c_count++;    // Increase our counter
        }
        // If we could not find an alias, return error
        if (!isset($type)) {
            $this->errors[] = APIResponse\get(4055);
        }
        if (!isset($type_err)) {
            // Loop through our arrays and ensure the values are valid
            $a_count = 0;   // Define a loop counter
            foreach ($address as $ae) {
                // Conditions for alias type 'port'
                if ($type === "port") {
                    // Check that our value is numeric
                    if (is_numeric($ae)) {
                        if (1 <= intval($ae) and intval($ae) <= 65535) {
                            $address[$a_count] = strval($ae);
                        } else {
                            $this->errors[] = APIResponse\get(4065);

                        }
                    } else {
                        $this->errors[] = APIResponse\get(4066);

                    }
                }
                // Conditionals for alias type 'network'
                if ($type === "network") {
                    // Check that values are strings
                    if (is_string($ae)) {
                        // Check that string is a network CIDR
                        if (strpos($ae, "/")) {
                            $net_ip = explode("/", $ae)[0];    // Save our network IP
                            $bit_mask = explode("/", $ae)[1];    // Save our subnet bit mask
                            // Check if our IP is IPv4
                            if (is_ipaddrv4($net_ip)) {
                                $max_bits = 32;    // Assign our maximum IPv4 bitmask
                            } elseif (is_ipaddrv6($net_ip)) {
                                $max_bits = 128;    // Assign our maximum IPv4 bitmask
                            } else {
                                $this->errors[] = APIResponse\get(4067);
                            }
                            // Check if our bitmask is numeric and in range
                            if (is_numeric($bit_mask)) {
                                if (1 <= intval($bit_mask) and intval($bit_mask) <= $max_bits) {
                                    continue;
                                } else {
                                    $this->errors[] = APIResponse\get(4068);
                                }
                            } else {
                                $this->errors[] = APIResponse\get(4069);
                            }
                        } else {
                            $this->errors[] = APIResponse\get(4069);
                        }
                    } else {
                        $this->errors[] = APIResponse\get(4070);
                    }
                }
                // Conditions for alias type 'host'
                if ($type === "host") {
                    // Check that values are strings
                    if (is_string($ae)) {
                        $address[$a_count] = APITools\sanitize_str($ae);
                    } else {
                        $this->errors[] = APIResponse\get(4070);
                    }
                }
                // Increase our counter
                $a_count++;
            }
            // Check each of our alias details
            foreach ($detail as $de) {
                if (!is_string($de)) {
                    $this->errors[] = APIResponse\get(4071);
                }
            }
            $this->validated_data["address"] = array_merge($curr_addr, $address);
            $this->validated_data["detail"] = array_merge($curr_detail, $detail);
        }
    }
}