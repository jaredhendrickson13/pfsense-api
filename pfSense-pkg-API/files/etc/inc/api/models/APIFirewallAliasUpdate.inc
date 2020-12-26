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

class APIFirewallAliasUpdate extends APIModel {
    private $type_changed;
    private $original_name;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
        $this->change_note = "Modified firewall alias address via API";
        $this->type_changed = false;
    }

    public function action() {
        $this->config["aliases"]["alias"][$this->id] = $this->validated_data;
        $this->__rename_alias_references__();
        $this->write_config();
        send_event("filter reload");
        return APIResponse\get(0, $this->config["aliases"]["alias"][$this->id]);
    }

    private function __validate_id() {
        # Require clients to pass in an ID field containing the alias name or index value
        if (isset($this->initial_data["id"])) {
            # Loop through each alias and check for a match
            foreach ($this->config["aliases"]["alias"] as $id=>$alias) {
                # First check if the ID matches the index value or the alias name
                if ($this->initial_data["id"] === $id or $this->initial_data["id"] === $alias["name"]) {
                    $this->id = $id;
                    $this->validated_data = $alias;
                    $this->original_name = $alias["name"];
                    break;
                }
            }
            # If we did not find an ID in the loop, return a not found error
            if (is_null($this->id)) {
                $this->errors[] = APIResponse\get(4055);
            }
        } else {
            $this->errors[] = APIResponse\get(4050);
        }
    }

    private function __validate_name() {
        # Optionally allow clients to update the alias name
        if (isset($this->initial_data['name'])) {
            # Ensure an alias with this name does not already exist. Allow if the stored name matches the requested.
            if (!is_alias($this->initial_data['name']) or $this->validated_data["name"] === $this->initial_data['name']) {
                # Ensure the requested name is valid and not reserved by the system
                if (is_validaliasname($this->initial_data["name"])) {
                    $this->validated_data["name"] = $this->initial_data['name'];
                } else {
                    $this->errors[] = APIResponse\get(4053);
                }
            } else {
                $this->errors[] = APIResponse\get(4056);
            }
        }
    }

    private function __validate_type() {
        # Optionally allow clients to update the alias type
        if (isset($this->initial_data['type'])) {
            # Require alias to not be in use to change the type
            if (!APITools\alias_in_use($this->original_name)) {
                # Require this type to be supported
                if (in_array($this->initial_data['type'], ["host", "network", "port"])) {
                    $this->type_changed = true;
                    $this->validated_data["type"] = $this->initial_data["type"];
                } else {
                    $this->errors[] = APIResponse\get(4057);
                }
            } elseif ($this->initial_data["type"] !== $this->validated_data["type"]) {
                $this->errors[] = APIResponse\get(4107);
            }
        }
    }

    private function __validate_address() {
        if (isset($this->initial_data['address']) or $this->type_changed) {
            # Revalidate the stored values if the type was changed but no new address values were requested
            if ($this->type_changed and !isset($this->initial_data['address'])) {
                $this->initial_data["address"] = explode(" ", $this->validated_data["address"]);
            }
            # Convert value to array if it is not already
            if (!is_array($this->initial_data['address'])) {
                $this->initial_data['address'] = [$this->initial_data['address']];
            }
            # Revert stored address to empty array
            $this->validated_data["address"] = [];
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
        }
    }

    private function __validate_detail() {
        # Revalidate the existing details if no new details were requested
        if (!isset($this->initial_data["detail"])) {
            $this->initial_data["detail"] = explode("||", $this->validated_data["detail"]);
        }
        # Convert single values to an array
        if (!is_array($this->initial_data["detail"])) {
            $this->initial_data["detail"] = [$this->initial_data["detail"]];
        }
        # Revert stored detail to empty array
        $this->validated_data["detail"] = [];
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
        $this->__validate_id();
        $this->__validate_name();
        $this->__validate_type();
        $this->__validate_descr();
        $this->__validate_address();
        $this->__validate_detail();
    }

    private function __rename_alias_references__() {
        # Only update alias references if our name was changed
        if ($this->original_name !== $this->validated_data["name"]) {
            update_alias_name($this->validated_data["name"], $this->original_name);
        }
    }
}