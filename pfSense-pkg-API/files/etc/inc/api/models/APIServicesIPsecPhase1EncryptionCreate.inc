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

class APIServicesIPsecPhase1EncryptionCreate extends APIModel {
    # Create our method constructor
    public function construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-ipsec-editphase1"];
        $this->change_note = "Added IPsec phase 1 encryption via API";
    }

    public function action() {
        # Add this encryption item to the parent P1 entry
        $next_id = $this->get_next_id("ipsec/phase1/{$this->id}/encryption/item");
        $this->set_config("ipsec/phase1/{$this->id}/encryption/item/{$next_id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty('ipsec');

        # Apply these changes if requested
        if ($this->initial_data["apply"] === true) {
            APIServicesIPsecApplyCreate::apply();
        }

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload($nested=false, $prf=false) {
        # Only validate the IKE ID if this is not a nested API call
        if (!$nested) {
            $this->__validate_ikeid();
        }

        # Validate fields
        $this->validate_encryption_algorithm();
        $this->validate_hash_algorithm();
        $this->validate_dhgroup();

        # Only validate PRF fields if the parent phase 1 entry has it enabled or it's explicitly requested
        if ($prf or $this->is_parent_prf_enabled()) {
            $this->validate_prf_algorithm();
        }
        # Default prf-algorithm to the specified hash-algorithm
        else {
            $this->validated_data["prf-algorithm"] = $this->validated_data["hash-algorithm"];
        }

    }

    private function __validate_ikeid() {
        # Validate the required 'ikeid' field
        if (isset($this->initial_data["ikeid"])) {
            # Attempt to get the config ID from this IKE ID
            $this->id = APITools\get_phase1_config_id_from_ikeid($this->initial_data["ikeid"]);

            # Return an error if this IKE ID was not found in any existing IPsec phase1 entry
            if ($this->id === false) {
                $this->errors[] = APIResponse\get(2208);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2207);
        }
    }

    public function validate_encryption_algorithm($include_auto=false) {
        # Check for the required 'encryption-algorithm' field
        if (isset($this->initial_data["encryption-algorithm"])) {
            # Ensure value is valid option
            if (in_array($this->initial_data["encryption-algorithm"], $this->get_encryption_algos($include_auto))) {
                $this->validated_data["encryption-algorithm"] = $this->initial_data["encryption-algorithm"];
            }
            else {
                $this->errors[] = APIResponse\get(2210);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2209);
        }
    }

    public function validate_hash_algorithm() {
        global $p1_halgos;

        # Check for the required 'hash-algorithm' field
        if (isset($this->initial_data["hash-algorithm"])) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["hash-algorithm"], $p1_halgos)) {
                $this->validated_data["hash-algorithm"] = $this->initial_data["hash-algorithm"];
            }
            else {
                $this->errors[] = APIResponse\get(2212);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2211);
        }
    }

    public function validate_dhgroup() {
        global $p1_dhgroups;

        # Check for the required 'dhgroup' field
        if (isset($this->initial_data["dhgroup"])) {
            # Ensure value is a valid option
            if (array_key_exists((int)$this->initial_data["dhgroup"], $p1_dhgroups)) {
                $this->validated_data["dhgroup"] = (int)$this->initial_data["dhgroup"];
            }
            else {
                $this->errors[] = APIResponse\get(2214);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2213);
        }
    }

    public function validate_prf_algorithm() {
        global $p1_halgos;

        # Check for the required 'prf-algorithm' field
        if (isset($this->initial_data["prf-algorithm"])) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["prf-algorithm"], $p1_halgos)) {
                $this->validated_data["prf-algorithm"] = $this->initial_data["prf-algorithm"];
            }
            else {
                $this->errors[] = APIResponse\get(2216);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2215);
        }
    }

    public function is_parent_prf_enabled() {
        return $this->is_config_enabled("ipsec/phase1/{$this->id}", "prfselect_enable");
    }

    # Determines every possible encryption algorithm combination
    public function get_encryption_algos($include_auto=false) {
        global $p1_ealgos;
        $algo_options = [];

        # Loop through all encryption algorithms and determine all available options
        foreach ($p1_ealgos as $p1_ealgo_name=>$p1_ealgo) {
            # Determine available key lengths for this algo when selections are available
            if (array_key_exists("keysel", $p1_ealgo)) {
                # Determine the lowest and highest key lengths available, as well as intermittent steps.
                $key_len = $p1_ealgo["keysel"]["lo"];
                $key_len_max = $p1_ealgo["keysel"]["hi"];
                $key_len_step = $p1_ealgo["keysel"]["step"];

                # Include auto option if enabled (used for phase 2 entries)
                if ($include_auto) {
                    $algo_options[] = ["name"=>$p1_ealgo_name, "keylen"=>"auto"];
                }

                # Loop until key length steps are exhausted
                while ($key_len <= $key_len_max) {
                    $algo_options[] = ["name"=>$p1_ealgo_name, "keylen"=>$key_len];
                    $algo_options[] = ["name"=>$p1_ealgo_name, "keylen"=>(string)$key_len];
                    $key_len = $key_len + $key_len_step;
                }
            }
            # Otherwise, only set the name and/or empty keylen value
            else {
                $algo_options[] = ["name"=>$p1_ealgo_name];
                $algo_options[] = ["name"=>$p1_ealgo_name, "keylen"=>""];
            }
        }

        return $algo_options;
    }
}
