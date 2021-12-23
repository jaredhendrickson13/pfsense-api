<?php
//   Copyright 2022 Jared Hendrickson
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


class APIFirewallStatesDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-showstates"];
    }

    public function action() {
        # Reset all states if reset was requested
        if ($this->validated_data["reset"]) {
            pfSense_kill_states("0.0.0.0/0");
            pfSense_kill_states("0::/0");
        }
        # Otherwise, only kill states as requested
        else {
            pfSense_kill_states(
                $this->validated_data["source"],
                $this->validated_data["destination"],
                $this->validated_data["protocol"],
                $this->validated_data["interface"]
            );
            return APIResponse\get(0);
        }
    }

    public function validate_payload() {
        $this->__validate_reset();
        $this->__validate_source();
        $this->__validate_destination();
        $this->__validate_protocol();
        $this->__validate_interface();
    }

    private function __validate_reset() {
        # Check for our optional reset field
        if ($this->initial_data["reset"] === true) {
            $this->validated_data["reset"] = true;
        }
    }

    # This method must run after __validate_reset()
    private function __validate_source() {
        # Check for our optional source field
        if (isset($this->initial_data["source"])) {
            # Ensure source is valid IP address or CIDR
            if (is_ipaddr($this->initial_data["source"]) or is_subnet($this->initial_data["source"])) {
                $this->validated_data["source"] = $this->initial_data["source"];
            } else {
                $this->errors[] = APIResponse\get(4232);
            }
        }
        # Require this field if 'reset' was not passed in
        elseif (!isset($this->validated_data["reset"])) {
            $this->errors[] = APIResponse\get(4231);
        }
    }

    private function __validate_destination() {
        # Check for our optional destination field
        if (isset($this->initial_data["destination"])) {
            # Ensure destination is valid IP address or CIDR
            if (is_ipaddr($this->initial_data["destination"]) or is_subnet($this->initial_data["destination"])) {
                $this->validated_data["destination"] = $this->initial_data["destination"];
            } else {
                $this->errors[] = APIResponse\get(4233);
            }
        }
    }

    private function __validate_protocol() {
        # Local variables
        $protocol_opts = ["tcp", "udp", "icmp", "icmpv6"];

        # Check for our optional protocol field
        if (isset($this->initial_data["protocol"])) {
            # Ensure protocol is valid option
            if (in_array(strtolower($this->initial_data["protocol"]), $protocol_opts)) {
                $this->validated_data["protocol"] = $this->initial_data["protocol"];
            } else {
                $this->errors[] = APIResponse\get(4234);
            }
        }
    }

    private function __validate_interface() {
        # Check for our optional interface field
        if (isset($this->initial_data["interface"])) {
            # Ensure interface exists
            if (APITools\get_pfsense_if_id($this->initial_data["interface"])) {
                $this->validated_data["interface"] = $this->initial_data["interface"];
            } else {
                $this->errors[] = APIResponse\get(4235);
            }
        }
    }

}
