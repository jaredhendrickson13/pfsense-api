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

class APIFirewallAliasEntryCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-alias-edit"];
        $this->change_note = "Added firewall alias address via API";
    }

    public function action() {
        # Update our alias with the new entries
        $this->config["aliases"]["alias"][$this->id] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty("aliases");

        # Only reload the firewall filter if a false value was not passed in
        # TODO: This condition applies the changes by default to stay backwards compatible with v1.3.0
        # TODO: this should be refactored in a future release to not apply by default
        if ($this->initial_data["apply"] !== false) {
            APIFirewallApplyCreate::apply();
        }

        return APIResponse\get(0, $this->config["aliases"]["alias"][$this->id]);
    }

    private function __validate_name() {
        # Require clients to pass in a name value to locate the alias to add addresses to
        if (isset($this->initial_data['name'])) {
            # Ensure our alias name is a string
            if (is_string($this->initial_data['name'])) {
                # Ensure this alias exists
                $this->id = $this->__get_alias_id($this->initial_data["name"]);
                if (isset($this->id)) {
                    $this->validated_data = $this->config["aliases"]["alias"][$this->id];
                } else {
                    $this->errors[] = APIResponse\get(4055);
                }
            } else {
                $this->errors[] = APIResponse\get(4053);
            }
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
    }

    public function __validate_address() {
        # Require clients to pass in addresses to add to the alias
        if (isset($this->initial_data["address"])) {
            # Ensure our input and existing data is in array format
            $this->initial_data["address"] = $this->__value_to_array($this->initial_data["address"]);
            $this->validated_data["address"] = array_filter(explode(" ", $this->validated_data["address"]));

            # Loop through each address and ensure it matches the alias type
            foreach ($this->initial_data["address"] as $address) {
                # If the alias is a host type, validate it accordingly
                if ($this->validated_data["type"] === "host") {
                    # Require each address to be either IPv4 address, IPv6 address, or FQDN
                    if (is_ipaddroralias($address) or is_fqdn($address)) {
                        $this->validated_data["address"][] = $address;
                    } else {
                        $this->errors[] = APIResponse\get(4058);
                    }
                }
                # If the alias is a network type, validate it accordingly
                elseif ($this->validated_data["type"] === "network") {
                    # Require each address to be either an IPv4/IPv6 subnet, or an FQDN
                    if (is_subnet($address) or is_fqdn($address) or is_ipaddroralias($address)) {
                        # If a singular IP, add /32 or /128 subnet mask
                        if (is_ipaddrv4($address)) {
                            $address = $address."/32";
                        } elseif (is_ipaddrv6($address)) {
                            $address = $address."/128";
                        }
                        $this->validated_data["address"][] = $address;
                    } else {
                        $this->errors[] = APIResponse\get(4059);
                    }
                }
                # If the alias is a port type, validate it accordingly
                elseif ($this->validated_data["type"] === "port") {
                    # Replace hyphen with colon as expected for port ranges
                    $address = str_replace("-", ":", $address);

                    # Require each address to be either a TCP/UDP port or range
                    if (is_port_or_range_or_alias(strval($address))) {
                        $this->validated_data["address"][] = $address;
                    } else {
                        $this->errors[] = APIResponse\get(4060);
                    }
                }
            }
            $this->validated_data["address"] = implode(" ", $this->validated_data["address"]);
        }
    }

    public function __validate_detail() {
        $this->initial_data["detail"] = $this->__value_to_array($this->initial_data["detail"]);
        $this->validated_data["detail"] = ($this->validated_data["detail"]) ? explode("||", $this->validated_data["detail"]) : [];
        $this->validated_data["detail"] = $this->__value_to_array($this->validated_data["detail"]);

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
        $this->__validate_address();
        $this->__validate_detail();
    }

    private function __get_alias_id($name) {
        # Loop through each alias until we find one that matches
        foreach ($this->config["aliases"]["alias"] as $id => $alias) {
            # Check if the name matches
            if ($name === $alias["name"]) {
                return intval($id);
            }
        }
    }

    # Adds a non-array variable to an array
    private function __value_to_array($value) {
        # Only convert the value if it is not already array
        if (!is_array($value)) {
            if (is_null($value)) {
                return [];
            } else {
                return [$value];

            }
        } else {
            return $value;
        }
    }
}
