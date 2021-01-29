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

class APIFirewallNATOneToOneCreate extends APIModel {
    private $nat_reflection;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Added 1:1 NAT mapping via API";
        $this->privileges = ["page-all", "page-firewall-nat-1-1-edit"];
        $this->nat_reflection = ["enable", "disable"];
        $this->id = count($this->config["nat"]["onetoone"]);
    }

    public function action() {
        # Add our new configuration
        $this->__init_array();
        $this->config["nat"]["onetoone"][$this->id] = $this->validated_data;
        APITools\sort_nat_rules($this->initial_data["top"], $this->id, "onetoone");
        $this->write_config();
        $this->apply();
        return APIResponse\get(0, $this->validated_data);
    }

    private function __validate_interface() {
        # Require clients to pass in an interface
        if (isset($this->initial_data['interface'])) {
            $interface = APITools\get_pfsense_if_id($this->initial_data['interface']);
            # Require interface to exist
            if ($interface) {
                $this->validated_data["interface"] = $interface;
            } else {
                $this->errors[] = APIResponse\get(4076);
            }
        } else {
            $this->errors[] = APIResponse\get(4075);
        }
    }

    private function __validate_external() {
        # Require the client to pass in an external IP value
        if (isset($this->initial_data["external"])) {
            # Require the external address to be either IPv4 or IPv6 address
            if (is_ipaddrv4($this->initial_data["external"]) or is_ipaddrv6($this->initial_data["external"])) {
                $this->validated_data["external"] = $this->initial_data["external"];
            } else {
                $this->errors[] = APIResponse\get(4082);
            }
        } else {
            $this->errors[] = APIResponse\get(4081);
        }
    }

    private function __validate_src() {
        # Require to specify a source address
        if (isset($this->initial_data['src'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['src'], "source");
            if ($rule_check["valid"]) {
                $this->validated_data["source"] = $rule_check["data"]["source"];
            } else {
                $this->errors[] = APIResponse\get(4079);
            }
        } else {
            $this->errors[] = APIResponse\get(4077);
        }
    }

    private function __validate_dst() {
        # Require to specify a destination address
        if (isset($this->initial_data['dst'])) {
            $rule_check = APITools\is_valid_rule_addr($this->initial_data['dst'], "destination");
            if ($rule_check["valid"]) {
                $this->validated_data["destination"] = $rule_check["data"]["destination"];
            } else {
                $this->errors[] = APIResponse\get(4080);
            }
        } else {
            $this->errors[] = APIResponse\get(4078);
        }
    }

    private function __validate_disabled() {
        # Optionally allow client to disable port forward upon entry
        if ($this->initial_data['disabled'] === true) {
            $this->validated_data["disabled"] = "";
        }
    }

    private function __validate_nobinat() {
        # Optionally allow client to disable biNAT for this entry
        if ($this->initial_data['nobinat'] === true) {
            $this->validated_data["nobinat"] = "";
        }
    }

    private function __validate_descr() {
        # Optionally allow client to specify a description for this entry
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = $this->initial_data["descr"];
        } else {
            $this->validated_data["descr"] = "";
        }
    }

    private function __validate_natreflection() {
        # Optionally allow client to specify the NAT reflection mode
        if (isset($this->initial_data['natreflection'])) {
            # Ensure client specified a support NAT reflection mode
            if (in_array($this->initial_data['natreflection'], $this->nat_reflection)) {
                $this->validated_data["natreflection"] = $this->initial_data['natreflection'];
            } else {
                $this->errors[] = APIResponse\get(4008);
            }
        }
    }

    private function __init_array() {
        # Ensure our NAT configuration is initialized as an array
        if (!is_array($this->config["nat"])) {
            $this->config["nat"] = [];
        }

        # Ensure our one-to-one NAT configuration is initialized as an array
        if (!is_array($this->config["nat"]["onetoone"])) {
            $this->config["nat"]["onetoone"] = [];
        }

    }

    public function validate_payload() {
        # Run each field validator method
        $this->__validate_interface();
        $this->__validate_external();
        $this->__validate_src();
        $this->__validate_dst();
        $this->__validate_disabled();
        $this->__validate_nobinat();
        $this->__validate_descr();
        $this->__validate_natreflection();
    }

    public function apply() {
        # Mark the NAT subsystem as changed, clear if applied
        mark_subsystem_dirty('natconf');

        # Allow clients to apply this rule immediately if they passed in an apply value
        if ($this->initial_data["apply"] === true) {
            filter_configure();
            clear_subsystem_dirty('natconf');
            clear_subsystem_dirty('filter');
        }
    }
}