<?php
//   Copyright 2020 Jared Hendrickson
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

class APIFirewallAliasCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
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

    private function __validate_name() {
        # Require clients to specify a valid name for the alias
        if (isset($this->initial_data['name'])) {
            # Ensure an alias with this name does not already exist
            if (!is_alias($this->initial_data['name'])) {
                # Ensure the requested name is valid and not reserved by the system
                if (is_validaliasname($this->initial_data["name"])) {
                    $this->validated_data["name"] = $this->initial_data['name'];
                } else {
                    $this->errors[] = APIResponse\get(4053);
                }
            } else {
                $this->errors[] = APIResponse\get(4056);
            }
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
    }

    private function __validate_type() {
        # Require clients to specify a valid type for this alias
        if (isset($this->initial_data['type'])) {
            # Require this type to be supported
            if (in_array($this->initial_data['type'], ["host", "network", "port"])) {
                $this->validated_data["type"] = $this->initial_data["type"];
            } else {
                $this->errors[] = APIResponse\get(4057);
            }
        } else {
            $this->errors[] = APIResponse\get(4061);
        }
    }

    private function __validate_address() {
        if (isset($this->initial_data['address'])) {
            # Convert value to array if it is not already
            if (!is_array($this->initial_data['address'])) {
                $this->initial_data['address'] = [$this->initial_data['address']];
            }

            # Loop through each address and ensure it is valid for our specified type
            foreach($this->initial_data['address'] as $address) {
                # Validation for host type aliases
                if ($this->validated_data["type"] === "host") {
                    # Require this address to be a valid IPv4, IPv6, or FQDN
                    if (is_ipaddrv4($address) or is_ipaddrv6($address) or is_fqdn($address)) {
                        $this->validated_data["address"][] = $address;
                    } else {
                        $this->errors[] = APIResponse\get(4058);
                    }
                }
                # Validation for network type aliases
                elseif ($this->validated_data["type"] === "network") {
                    # Require this address to be a valid IPv4 subnet, IPv6 subnet, or FQDN
                    if (is_subnetv4($address) or is_subnetv6($address) or is_fqdn($address)) {
                        $this->validated_data["address"][] = $address;
                    } else {
                        $this->errors[] = APIResponse\get(4059);
                    }
                }
                # Validation for port type aliases
                elseif ($this->validated_data["type"] === "port") {
                    # Convert integers to string expected by is_port_or_range() function
                    $address = (is_int($address)) ? strval($address) : $address;

                    # Replace hyphen with colon
                    $address = str_replace("-", ":", $address);

                    # Require this address to be a valid port or port range
                    if (is_port_or_range($address)) {
                        $this->validated_data["address"][] = $address;
                    } else {
                        $this->errors[] = APIResponse\get(4060);
                    }
                }

            }

            # Convert our array to a space separated string as expected by the XML config
            $this->validated_data["address"] = implode(" ", $this->validated_data["address"]);
        } else {
            $this->errors[] = APIResponse\get(4052);
        }
    }

    private function __validate_descr() {
        # Optionally allow clients to specify an alias description
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        } else {
            $this->validated_data["descr"] = "";
        }
    }

    private function __validate_detail() {
        # Convert single values to an array
        if (!is_array($this->initial_data["detail"])) {
            $this->initial_data["detail"] = [$this->initial_data["detail"]];
        }

        # If we have less detail values than address values, add some defaults
        while(true) {
            # Check if we have the correct number of detail values
            if (count($this->initial_data["detail"]) < count($this->initial_data["address"])) {
                $this->initial_data["detail"][] = "Entry added " . date('r');
            } else {
                break;
            }
        }

        # Ensure the number of detail values is less than or equal to the number of address values
        if (count($this->initial_data["detail"]) <= count($this->initial_data["address"])) {
            # Loop through each alias detail and ensure it is a string
            foreach ($this->initial_data["detail"] as $detail) {
                if (is_string($detail)) {
                    $this->validated_data["detail"][] = $detail;
                } else {
                    $this->errors[] = APIResponse\get(4071);
                }
            }

            # Convert our array to pfSense's expected XML string format
            $this->validated_data["detail"] = implode("||", $this->validated_data["detail"]);
        } else {
            $this->errors[] = APIResponse\get(4106);
        }
    }
    
    public function validate_payload() {
        $this->__validate_name();
        $this->__validate_type();
        $this->__validate_descr();
        $this->__validate_address();
        $this->__validate_detail();
    }
}