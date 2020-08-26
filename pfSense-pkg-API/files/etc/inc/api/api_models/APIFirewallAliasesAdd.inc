<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallAliasesAdd extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
        $this->change_note = "Added firewall alias via API";
    }

    public function action() {
        // Add our alias
        $this->config["aliases"] = !is_array($this->config["aliases"]) ? array("alias" => []) : $this->config["aliases"];
        $this->config["aliases"]["alias"][] = $this->validated_data;    // Write our configuration change
        $this->write_config();    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {

        $allowed_alias_types = array("host", "network", "port");    // Array of allowed alias types
        if (isset($this->initial_data['name'])) {
            $name = $this->initial_data['name'];
            $name = APITools\sanitize_str($name);
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
        if (isset($this->initial_data['type'])) {
            $type = $this->initial_data['type'];
            $type = trim($type);
        } else {
            $this->errors[] = APIResponse\get(4061);
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
        if (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
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
        } elseif (!is_string($type)) {
            $this->errors[] = APIResponse\get(4062);
        } elseif (!in_array($type, $allowed_alias_types)) {
            $this->errors[] = APIResponse\get(4057);
        } elseif (isset($descr) and !is_string($descr)) {
            $this->errors[] = APIResponse\get(4063);
        } elseif (!is_array($address)) {
            $this->errors[] = APIResponse\get(4054);
        } elseif (isset($detail) and !is_array($detail)) {
            $this->errors[] = APIResponse\get(4063);
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
                        $address[$a_count] = sanitize_str($ae);
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
        }
        // Check our existing aliases
        if (is_array($this->config["aliases"])) {
            $c_count = 0;
            // Loop through each alias and see if alias already exists
            foreach ($this->config["aliases"]["alias"] as $ce) {
                if ($ce["name"] === $name) {
                    $this->errors[] = APIResponse\get(4056);

                }
            }
        }
        $this->validated_data["name"] = $name;    // Save our alias name
        $this->validated_data["type"] = $type;    // Save our type
        $this->validated_data["descr"] = $descr;    // Save our description
        $this->validated_data["address"] = implode(" ", $address);    // Join array in to space seperated string
        $this->validated_data["detail"] = implode("||", $detail);    // Join array in to || seperated string
    }
}