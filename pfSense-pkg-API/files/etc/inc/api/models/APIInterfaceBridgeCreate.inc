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


class APIInterfaceBridgeCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-bridge-edit"];
        $this->change_note = "Added bridge interface via API";
    }

    public function action() {
        $this->id = $this->get_next_id("bridges/bridged");
        $this->set_config("bridges/bridged/{$this->id}", $this->validated_data);
        $this->write_config();
        interface_bridge_configure($this->validated_data);
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_members() {
        # Default validated members
        $this->validated_data["members"] = [];

        # Require clients to specify at least 1 bridge member
        if (isset($this->initial_data['members'])) {
            # If non-array was passed in, convert it an array
            if (!is_array($this->initial_data['members'])) {
                $this->initial_data['members'] = [$this->initial_data['members']];
            }

            # Require at least 1 member to be contained within the array
            if (count($this->initial_data['members']) >= 1) {
                # Loop through each member and validate it
                foreach ($this->initial_data['members'] as $member) {
                    # Convert the member to the pfSense ID (wan, lan, optx, etc)
                    $member = APITools\get_pfsense_if_id($member);

                    # Ensure the interface exists
                    if (!$member) {
                        $this->errors[] = APIResponse\get(3067);
                    }
                    # Ensure member is not already a member of another bridge
                    elseif ($this->__is_member_in_bridge($member)) {
                        $this->errors[] = APIResponse\get(3068);
                    }
                    # Ensure wireless members are in host AP mode
                    elseif ($this->__is_member_wireless($member) && !$this->__is_wireless_member_hostap($member)) {
                        $this->errors[] = APIResponse\get(3069);
                    }
                    # Ensure members do not have captive portal configured
                    elseif ($this->__is_member_captive_portal($member)) {
                        $this->errors[] = APIResponse\get(3070);
                    }
                    # If no previous conditions were met, the member is valid.
                    else {
                        $this->validated_data["members"][] = $member;
                    }
                }

                # Convert members back into a comma separated string as expected by pfSense
                $this->validated_data["members"] = implode(",", $this->validated_data["members"]);

            } else {
                $this->errors[] = APIResponse\get(3066);
            }
        } else {
            $this->errors[] = APIResponse\get(3066);
        }
    }

    private function __validate_descr() {
        # Optionally allow client to specify a bridge description. Default if not set.
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        } else {
            $this->validated_data["descr"] = "";
        }
    }

    public function validate_payload() {
        $this->__validate_members();
        $this->__validate_descr();

        # Static/non-interactive fields
        # TODO: add additional field validators for the advanced settings
        $this->validated_data["maxaddr"] = "";
        $this->validated_data["timeout"] = "";
        $this->validated_data["maxage"] = "";
        $this->validated_data["fwdelay"] = "";
        $this->validated_data["hellotime"] = "";
        $this->validated_data["priority"] = "";
        $this->validated_data["proto"] = "rstp";
        $this->validated_data["holdcnt"] = "";
        $this->validated_data["ifpriority"] = "";
        $this->validated_data["ifpathcost"] = "";
        $this->validated_data["bridgeif"] = $this->__get_next_bridge_if();
    }

    private function __is_member_in_bridge($member) {
        # Local variables
        $member = APITools\get_pfsense_if_id($member);

        # Check if our request bridge member already exists in another bridge
        foreach ($this->get_config("bridges/bridged", []) as $bridge) {
            $bridge_members = explode(",", $bridge["members"]);
            if (in_array($member, $bridge_members)) {
                return true;
            }
        }
        return false;
    }

    private function __is_member_wireless($member) {
        # Check if a wireless configuration exists for this interface
        if (is_array($this->get_config("interfaces/{$member}/wireless"))) {
            return true;
        } else {
            return false;
        }
    }

    private function __is_wireless_member_hostap($member) {
        # Only proceed if this member is a wireless interfaee
            # Check if this wireless interface is not in host AP mode
            if ($this->get_config("interfaces/{$member}/wireless/mode") == "hostap") {
                return true;
            } else {
                return false;
            }
    }

    private function __is_member_captive_portal($member) {
        # Check if this member interface has a captive portal configured
        foreach ($this->get_config('captiveportal', []) as $cp) {
            if (in_array($member, explode(',', $cp['interface']))) {
                return true;
            }
        }
        return false;
    }

    private function __get_next_bridge_if() {
        # Local variables
        $counter = 0;
        $in_use = true;

        # Loop continuously until unique bridge if is found
        while($in_use) {
            # Loop variables
            $next_bridge = "bridge".strval($counter);
            $in_use = false;

            # Loop through all existing bridges and see if this bridgeif is in use
            foreach ($this->get_config("bridges/bridged", []) as $bridge) {
                # Check if this value matches our potential next bridge
                if ($bridge["bridgeif"] === $next_bridge) {
                    $in_use = true;
                }
            }

            # Increase the counter
            $counter++;
        }

        return $next_bridge;
    }
}
