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


class APIRoutingStaticRouteUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified static route via API";
    }

    public function action() {
        # Write route changes to config
        $this->set_config("staticroutes/route/{$this->id}", $this->validated_data);
        $this->write_config();

        # Mark the routing subsystem as changed
        mark_subsystem_dirty("staticroutes");

        # Serialize the pending changes, and apply immediately if requested
        $this->__serialize_route();
        $this->apply();

        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_id() {
        # Require clients to pass in an 'id' payload value
        if (isset($this->initial_data["id"])) {
            if ($this->get_config("staticroutes/route/{$this->initial_data["id"]}", false)) {
                $this->id = $this->initial_data["id"];
                $this->validated_data = $this->get_config("staticroutes/route/{$this->id}", []);
            } else {
                $this->errors[] = APIResponse\get(6006);
            }
        } else {
            $this->errors[] = APIResponse\get(6005);
        }
    }

    private function __validate_network() {
        # Optionally allow clients to update the 'network' value
        if (isset($this->initial_data['network'])) {
            $subnet = intval(explode("/", $this->initial_data["network"])[1]);
            $ip = explode("/", $this->initial_data["network"])[0];
            # Check if address is IPv4, IPv6 or alias. Otherwise return an error
            if (is_ipaddrv4($ip)) {
                # Ensure the subnet bit mask is within range for IPv4 addresses
                if (is_numeric($subnet) and ($subnet >= 1 and $subnet <= 32)) {
                    $this->validated_data["network"] = $this->initial_data["network"];
                } else {
                    $this->errors[] = APIResponse\get(6002);
                }

            } elseif (is_ipaddrv6($ip)) {
                # Ensure the subnet bit mask is within range for IPv6 addresses
                if (is_numeric($subnet) and ($subnet >= 1 and $subnet <= 128)) {
                    $this->validated_data["network"] = $this->initial_data["network"];
                } else {
                    $this->errors[] = APIResponse\get(6002);
                }
            } elseif (is_alias($ip)) {
                $this->validated_data["network"] = $this->initial_data["network"];
            } else {
                $this->errors[] = APIResponse\get(6001);
            }
        }
    }

    private function __validate_gateway() {
        # Optionally allow clients to update the 'gateway' value
        if (isset($this->initial_data["gateway"])) {
            # Ensure this is a valid gateway
            if (APITools\is_gateway($this->initial_data["gateway"])) {
                # Set the updated gateway
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            } else {
                $this->errors[] = APIResponse\get(6004);
            }
        }
    }

    private function __validate_descr() {
        # Optionally allow clients to update the 'descr' value
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = strval($this->initial_data["descr"]);
        } elseif(empty($this->validated_data["descr"])) {
            $this->validated_data["descr"] = "";
        }
    }

    private function __validate_disabled() {
        # Optionally allow clients to update the 'disabled' value
        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = "";
        } elseif($this->initial_data["disabled"] === false) {
            unset($this->validated_data["disabled"]);
        }
    }

    public function validate_payload() {
        $this->__validate_id();
        $this->__validate_network();
        $this->__validate_gateway();
        $this->__validate_descr();
        $this->__validate_disabled();
    }

    # Gets all networks associated with this static route's 'network' value.
    public function get_networks() {
        global $aliastable;
        # Keep track of networks that can be applied to a static route
        $networks = [];

        # Check if the current network value is an alias
        if (is_alias($this->validated_data["network"])) {
            foreach (explode(" ", $aliastable[$this->validated_data["network"]]) as $alias_network) {
                # Set CIDR if this is an IPv4 address
                if (is_ipaddrv4($alias_network)) {
                    $alias_network .= "/32";
                }
                # Set CIDR if this is an IPv6 address
                if (is_ipaddrv6($alias_network)) {
                    $alias_network .= "/128";
                }
                # Don't include this newtork if it is not a subnet
                if (!is_subnet($alias_network)) {
                    continue;
                }
                # Add this network to the return array
                $networks[] = $alias_network;
            }
        }
        # Otherwise, we already know it is a subnet, simply add it to the networks list
        else {
            $networks[] = $this->validated_data["network"];
        }

        return $networks;
    }

    private function __serialize_route() {
        global $g;

        # Include pending route changes that haven't been applied yet, or simply initialize our pending change var
        if (file_exists("{$g['tmp_path']}/.system_routes.apply")) {
            $to_apply_list = unserialize(file_get_contents("{$g['tmp_path']}/.system_routes.apply"));
        }
        else {
            $to_apply_list = array();
        }

        # Check for the static route that is currently operating
        if (file_exists("{$g['tmp_path']}/staticroute_{$this->id}")) {
            $old_network = unserialize(file_get_contents("{$g['tmp_path']}/staticroute_{$this->id}"));
        }
        # Check for the static route gateway that is currently operating
        if (file_exists("{$g['tmp_path']}/staticroute_{$this->id}_gw")) {
            $old_gateway = unserialize(file_get_contents("{$g['tmp_path']}/staticroute_{$this->id}_gw"));
        }

        # If was set previously, and has changed, ensure we delete the old route and recreate it.
        if (!empty($old_gateway) && ($this->validated_data["gateway"] != $old_gateway)) {
            $delete_networks = $old_network;
            $delete_gateway = lookup_gateway_ip_by_name($old_gateway);
        }
        # Otherwise, check for routes that are no longer relevant to this entry
        else {
            $delete_networks = array_diff($old_network, $this->get_networks());
            $delete_gateway = lookup_gateway_ip_by_name($this->validated_data["gateway"]);
        }

        # Assign routes to be deleted when changes are applied
        if (count($delete_networks)) {
            foreach ($delete_networks as $del_net) {
                # Set the IP address family correctly when this is an IPv6 route
                if (is_ipaddrv6($del_net)) {
                    $ip_family = "-inet6";
                }
                # Fetch the route to be deleted
                $route = route_get($del_net, '', true);

                # Skip this route if we failed to fetch the route in the previous call
                if (!count($route)) {
                    continue;
                }

                # Mark this route for deletion during the next change
                $to_apply_list[] = "/sbin/route delete {$ip_family} {$del_net} {$delete_gateway}";
            }
        }

        file_put_contents("{$g['tmp_path']}/.system_routes.apply", serialize($to_apply_list));
    }

    public function apply() {
        # Apply these changes immediately if requested
        if ($this->initial_data["apply"] === true) {
            APIRoutingApplyCreate::apply();
        }
    }
}
