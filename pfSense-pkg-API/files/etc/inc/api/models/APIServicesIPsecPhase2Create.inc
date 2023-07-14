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

class APIServicesIPsecPhase2Create extends APIModel {
    public $def_rekey_time = false;
    public $def_rand_time = false;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Added IPsec phase 2 via API";
    }

    public function action() {
        # Write this entry to configuration
        $this->id = $this->get_next_id("ipsec/phase2");
        $this->set_config("ipsec/phase2/{$this->id}", $this->validated_data);
        $this->write_config();
        mark_subsystem_dirty('ipsec');

        # Apply these changes if requested
        if ($this->initial_data["apply"] === true) {
            APIServicesIPsecApplyCreate::apply();
        }

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Set IDs for this phase 2
        $this->validated_data["reqid"] = ipsec_new_reqid();
        $this->validated_data["uniqid"] = uniqid();

        # Validate client specified fields
        $this->__validate_ikeid();
        $this->__validate_descr();
        $this->__validate_disabled();
        $this->__validate_mode();
        $this->__validate_localid();
        $this->__validate_natlocalid();
        $this->__validate_remoteid();
        $this->__validate_protocol();
        $this->__validate_encryption_algorithm_option();
        $this->__validate_hash_algorithm_option();
        $this->__validate_pfsgroup();
        $this->__validate_rekey_time();
        $this->__validate_rand_time();
        $this->__validate_lifetime();
        $this->__validate_pinghost();
        $this->__validate_keepalive();
    }

    private function __validate_ikeid() {
        # Validate the required 'ikeid' field
        if (isset($this->initial_data["ikeid"])) {
            # Return an error if this IKE ID was not found in any existing IPsec phase 1 entry
            if (APITools\get_phase1_config_id_from_ikeid($this->initial_data["ikeid"]) !== false) {
                $this->validated_data["ikeid"] = (int)$this->initial_data["ikeid"];
            } else {
                $this->errors[] = APIResponse\get(2208);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2207);
        }
    }

    private function __validate_descr() {
        # Check for the optional 'descr' field
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = (string)$this->initial_data["descr"];
        }
        # Set empty string as default
        else {
            $this->validated_data["descr"] = "";
        }
    }

    private function __validate_disabled() {
        # Check for the optional 'disabled' field
        if ($this->initial_data["disabled"] === true) {
            $this->validated_data["disabled"] = true;
        }
    }

    private function __validate_mode() {
        global $p2_modes;

        # Check for the required 'mode' field
        if (isset($this->initial_data["mode"])) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["mode"], $p2_modes)) {
                $this->validated_data["mode"] = $this->initial_data["mode"];
            }
            else {
                $this->errors[] = APIResponse\get(2220);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2219);
        }
    }

    private function __validate_localid() {
        # Require this field if 'mode' is not set to 'transport'
        if ($this->validated_data["mode"] !== "transport") {
            # Check for the required 'disabled' field
            if (isset($this->initial_data["localid"])) {
                # Validate subfields for the applicable type
                $this->validated_data["localid"] = [];
                $this->__validate_localid_type();
            } else {
                $this->errors[] = APIResponse\get(2223);
            }
        }
    }

    private function __validate_localid_type() {
        # Validate the required the 'type' localid subfield
        if (isset($this->initial_data["localid"]["type"])) {
            # Find interfaces that match this value
            $type_if = APITools\get_pfsense_if_id($this->initial_data["localid"]["type"]);

            # Validate corresponding fields if type is 'address'
            if ($this->initial_data["localid"]["type"] === "address") {
                $this->validated_data["localid"]["type"] = "address";
                $this->__validate_localid_address();
            }
            # Validate corresponding fields if type is 'network'
            elseif ($this->initial_data["localid"]["type"] === "network") {
                $this->validated_data["localid"]["type"] = "network";
                $this->__validate_localid_address();
                $this->__validate_localid_netbits();
            }
            # Allow an interface to be specified unless 'mode' is set to 'vti'
            elseif ($type_if and $this->validated_data["mode"] !== "vti") {
                $this->validated_data["localid"]["type"] = $type_if;
            }
            # Otherwise, this value is not supported
            else {
                $this->errors[] = APIResponse\get(2225);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2224);
        }
    }

    # This method is called by __validate_localid_type depending on specific conditions.
    private function __validate_localid_address() {
        # Validate the required 'address' localid sub-field
        if (isset($this->initial_data["localid"]["address"])) {
            $mode = $this->validated_data["mode"];
            # Ensure value is IPv4/IPv6 if mode is set to 'vti'
            if (!is_ipaddr($this->initial_data["localid"]["address"]) and $mode === "vti") {
                $this->errors[] = APIResponse\get(2227);
            }
            # Ensure value is IPv4 if mode is set to 'tunnel'
            elseif (!is_ipaddrv4($this->initial_data["localid"]["address"]) and $mode === "tunnel") {
                $this->errors[] = APIResponse\get(2228);
            }
            # Ensure value is IPv6 if mode is set to 'tunnel6'
            elseif (!is_ipaddrv6($this->initial_data["localid"]["address"]) and $mode === "tunnel6") {
                $this->errors[] = APIResponse\get(2229);
            }
            # Otherwise, the value is valid
            else {
                $this->validated_data["localid"]["address"] = $this->initial_data["localid"]["address"];
            }
        }
        else {
            $this->errors[] = APIResponse\get(2226);
        }
    }

    # This method is called by __validate_localid_type depending on specific conditions.
    private function __validate_localid_netbits() {
        # Validate the required 'netbits' localid sub-field
        if (isset($this->initial_data["localid"]["netbits"])) {
            # Use shorthand variables for readability
            $address = $this->initial_data["localid"]["address"];
            $netbits = (int)$this->initial_data["localid"]["netbits"];

            # Require this value to be between 0 and 32 when localid address is IPv4
            if (is_ipaddrv4($address) and (0 > $netbits or 32 < $netbits)) {
                $this->errors[] = APIResponse\get(2231);
            }
            # Require this value to be between 0 and 128 when localid address is IPv6
            elseif (is_ipaddrv6($address) and (0 > $netbits or 128 < $netbits)) {
                $this->errors[] = APIResponse\get(2232);
            }
            # Otherwise, value is valid.
            else {
                $this->validated_data["localid"]["netbits"] = $netbits;
            }
        }
        else {
            $this->errors[] = APIResponse\get(2230);
        }
    }

    private function __validate_natlocalid() {
        # Validate this field if 'mode' is not set to 'transport' or 'vti'
        if (!in_array($this->validated_data["mode"], ["transport", "vti"])) {
            # Check for the required 'disabled' field
            if (isset($this->initial_data["natlocalid"])) {
                # Validate subfields for the applicable type
                $this->validated_data["natlocalid"] = [];
                $this->__validate_natlocalid_type();
            }
        }
    }

    private function __validate_natlocalid_type() {
        # Validate the required the 'type' natlocalid subfield
        if (isset($this->initial_data["natlocalid"]["type"])) {
            # Find interfaces that match this value
            $localid_type = $this->validated_data["localid"]["type"];
            $type_if = APITools\get_pfsense_if_id($this->initial_data["natlocalid"]["type"]);

            # Don't allow value to be 'network' while localid type is 'address'
            if ($localid_type === "address" and $this->initial_data["natlocalid"]["type"] === "network") {
                $this->errors[] = APIResponse\get(2235);
            }
            # Validate corresponding fields if type is 'address'
            elseif ($this->initial_data["natlocalid"]["type"] === "address") {
                $this->validated_data["natlocalid"]["type"] = "address";
                $this->__validate_natlocalid_address();
            }
            # Validate corresponding fields if type is 'network'
            elseif ($this->initial_data["natlocalid"]["type"] === "network") {
                $this->validated_data["natlocalid"]["type"] = "network";
                $this->__validate_natlocalid_address();
                $this->__validate_natlocalid_netbits();
            }
            # Allow an interface to be specified
            elseif ($type_if) {
                $this->validated_data["natlocalid"]["type"] = $type_if;
            }
            # Otherwise, this value is not supported
            else {
                $this->errors[] = APIResponse\get(2234);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2233);
        }
    }

    # This method is called by __validate_natlocalid_type depending on specific conditions.
    private function __validate_natlocalid_address() {
        # Validate the required 'address' natlocalid sub-field
        if (isset($this->initial_data["natlocalid"]["address"])) {
            $mode = $this->validated_data["mode"];
            # Ensure value is IPv4 if mode is set to 'tunnel'
            if (!is_ipaddrv4($this->initial_data["natlocalid"]["address"]) and $mode === "tunnel") {
                $this->errors[] = APIResponse\get(2237);
            }
            # Ensure value is IPv6 if mode is set to 'tunnel6'
            elseif (!is_ipaddrv6($this->initial_data["natlocalid"]["address"]) and $mode === "tunnel6") {
                $this->errors[] = APIResponse\get(2238);
            }
            # Otherwise, the value is valid
            else {
                $this->validated_data["natlocalid"]["address"] = $this->initial_data["natlocalid"]["address"];
            }
        }
        else {
            $this->errors[] = APIResponse\get(2236);
        }
    }

    # This method is called by __validate_natlocalid_type depending on specific conditions.
    private function __validate_natlocalid_netbits() {
        # Validate the required 'netbits' natlocalid sub-field
        if (isset($this->initial_data["natlocalid"]["netbits"])) {
            # Use shorthand variables for readability
            $netbits = (int)$this->initial_data["natlocalid"]["netbits"];

            # Require this value to match the localid netbits subfield
            if ($netbits === (int)$this->validated_data["localid"]["netbits"]) {
                $this->validated_data["natlocalid"]["netbits"] = $netbits;
            }
            else {
                $this->errors[] = APIResponse\get(2240);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2239);
        }
    }
    
    private function __validate_remoteid() {
        # Validate this field if 'mode' is not set to 'transport' or 'vti'
        if ($this->validated_data["mode"] !=="transport") {
            # Check for the required 'disabled' field
            if (isset($this->initial_data["remoteid"])) {
                # Validate subfields for the applicable type
                $this->validated_data["remoteid"] = [];
                $this->__validate_remoteid_type();
            }
            else {
                $this->errors[] = APIResponse\get(2256);
            }
        }
    }

    private function __validate_remoteid_type() {
        # Validate the required the 'type' remoteid subfield
        if (isset($this->initial_data["remoteid"]["type"])) {
            # Validate corresponding fields if type is 'address'
            if ($this->initial_data["remoteid"]["type"] === "address") {
                $this->validated_data["remoteid"]["type"] = "address";
                $this->__validate_remoteid_address();
            }
            # Validate corresponding fields if type is 'network'
            elseif ($this->initial_data["remoteid"]["type"] === "network") {
                $this->validated_data["remoteid"]["type"] = "network";
                $this->__validate_remoteid_address();
                $this->__validate_remoteid_netbits();
            }
            # Otherwise, this value is not supported
            else {
                $this->errors[] = APIResponse\get(2242);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2241);
        }
    }

    # This method is called by __validate_remoteid_type depending on specific conditions.
    private function __validate_remoteid_address() {
        # Validate the required 'address' remoteid sub-field
        if (isset($this->initial_data["remoteid"]["address"])) {
            $mode = $this->validated_data["mode"];
            # Ensure value is IPv4 if mode is set to 'tunnel'
            if (!is_ipaddrv4($this->initial_data["remoteid"]["address"]) and $mode === "tunnel") {
                $this->errors[] = APIResponse\get(2244);
            }
            # Ensure value is IPv6 if mode is set to 'tunnel6'
            elseif (!is_ipaddrv6($this->initial_data["remoteid"]["address"]) and $mode === "tunnel6") {
                $this->errors[] = APIResponse\get(2245);
            }
            # Otherwise, the value is valid
            else {
                $this->validated_data["remoteid"]["address"] = $this->initial_data["remoteid"]["address"];
            }
        }
        else {
            $this->errors[] = APIResponse\get(2243);
        }
    }

    # This method is called by __validate_remoteid_type depending on specific conditions.
    private function __validate_remoteid_netbits() {
        # Validate the required 'netbits' remoteid sub-field
        if (isset($this->initial_data["remoteid"]["netbits"])) {
            # Use shorthand variables for readability
            $address = $this->initial_data["remoteid"]["address"];
            $netbits = (int)$this->initial_data["remoteid"]["netbits"];

            # Require this value to be between 0 and 32 when remoteid address is IPv4
            if (is_ipaddrv4($address) and (0 > $netbits or 32 < $netbits)) {
                $this->errors[] = APIResponse\get(2247);
            }
            # Require this value to be between 0 and 128 when remoteid address is IPv6
            elseif (is_ipaddrv6($address) and (0 > $netbits or 128 < $netbits)) {
                $this->errors[] = APIResponse\get(2248);
            }
            else {
                $this->validated_data["remoteid"]["netbits"] = $netbits;
            }
        }
        else {
            $this->errors[] = APIResponse\get(2246);
        }
    }

    private function __validate_protocol() {
        global $p2_protos;

        # Check for the required 'protocol' field
        if (isset($this->initial_data["protocol"])) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["protocol"], $p2_protos)) {
                $this->validated_data["protocol"] = $this->initial_data["protocol"];
            }
            else {
                $this->errors[] = APIResponse\get(2222);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2221);
        }
    }

    public function __validate_encryption_algorithm_option() {
        # Only validate this field if 'protocol' is 'esp'
        if ($this->validated_data["protocol"] === "esp") {
            # Check for the required 'encryption-algorithm-option' field
            if (isset($this->initial_data["encryption-algorithm-option"])) {
                # Ensure value is an array and contains at least 1 item
                if (is_array($this->initial_data["encryption-algorithm-option"]) and count($this->initial_data["encryption-algorithm-option"]) >= 1) {
                    # Initialize the validated data array that will hold valid configuration
                    $this->validated_data["encryption-algorithm-option"] = [];

                    # Loop through each encryption-algorithm-option item and ensure it is valid
                    foreach ($this->initial_data["encryption-algorithm-option"] as $enc_conf) {
                        # Call the APIServicesIPsecPhase1EncryptionCreate model to validate entries
                        $enc_create = new APIServicesIPsecPhase1EncryptionCreate();
                        $enc_create->initial_data = ["encryption-algorithm"=>$enc_conf];
                        $enc_create->validate_encryption_algorithm(true);
                        $this->validated_data["encryption-algorithm-option"][] = $enc_conf;

                        # Add any encountered errors to this model's error array
                        $this->errors = array_merge($this->errors,  $enc_create->errors);
                    }
                }
                else {
                    $this->errors[] = APIResponse\get(2250);
                }
            } else {
                $this->errors[] = APIResponse\get(2249);
            }
        }
    }

    private function __validate_hash_algorithm_option() {
        global $p2_halgos;

        # Check for the required 'hash-algorithm-option' field
        if (isset($this->initial_data["hash-algorithm-option"])) {
            # Use a shorthand variable for readability
            $hash_algorithm_options = $this->initial_data["hash-algorithm-option"];

            # Initialize the array to store valid algos in
            $this->validated_data["hash-algorithm-option"] = [];

            # Require value to be array with at least 1 item
            if (is_array($hash_algorithm_options) and count($hash_algorithm_options) >= 1) {
                # Loop through each specified algorithm and ensure it is supported
                foreach ($hash_algorithm_options as $hash_algorithm_option) {
                    # Ensure value is a valid option
                    if (array_key_exists($hash_algorithm_option, $p2_halgos)) {
                        $this->validated_data["hash-algorithm-option"][] = $hash_algorithm_option;
                    }
                    else {
                        $this->errors[] = APIResponse\get(2253);
                    }
                }
            }
            else {
                $this->errors[] = APIResponse\get(2252);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2251);
        }
    }

    private function __validate_pfsgroup() {
        global $p2_pfskeygroups;

        # Check for the required 'pfsgroup' field
        if (isset($this->initial_data["pfsgroup"])) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["pfsgroup"], $p2_pfskeygroups)) {
                $this->validated_data["pfsgroup"] = $this->initial_data["pfsgroup"];
            }
            else {
                $this->errors[] = APIResponse\get(2255);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2254);
        }
    }

    private function __validate_rekey_time() {
        # Check for the optional 'rekey_time' field
        if (isset($this->initial_data["rekey_time"])) {
            # Ensure value is a numeric greater than or equal to 0
            if (is_numeric($this->initial_data["rekey_time"]) and (int)$this->initial_data["rekey_time"] >= 0) {
                $this->validated_data["rekey_time"] = (int)$this->initial_data["rekey_time"];
            }
            else {
                $this->errors[] = APIResponse\get(2193);
            }
        }
        # Sets a flag that will populate the default for this field. Default is 90% of the 'lifetime' value.
        else {
            $this->def_rekey_time = true;
        }
    }

    private function __validate_rand_time() {
        # Check for the optional 'rand_time' field
        if (isset($this->initial_data["rand_time"])) {
            # Ensure value is a numeric greater than or equal to 0
            if (is_numeric($this->initial_data["rand_time"]) and (int)$this->initial_data["rand_time"] >= 0) {
                $this->validated_data["rand_time"] = (int)$this->initial_data["rand_time"];
            }
            else {
                $this->errors[] = APIResponse\get(2195);
            }
        }
        # Sets a flag that will populate the default for this field. Default is 90% of the 'lifetime' value.
        else {
            $this->def_rand_time = true;
        }
    }

    private function __validate_lifetime() {
        # Use shorthand variables for rekey_time, rand_time for readability
        $rekey_time = $this->validated_data["rekey_time"];

        # Set default value if not specified. Even the default value needs to be validated for this field.
        if (!isset($this->initial_data["lifetime"])) {
            $this->initial_data["lifetime"] = 3600;
        }

        # Ensure value is a numeric greater than or equal to 0
        if (!is_numeric($this->initial_data["lifetime"]) or (int)$this->initial_data["lifetime"] < 0) {
            $this->errors[] = APIResponse\get(2196);
        }
        # If default 'rekey_time' was not assumed, ensure re-key time is less than lifetime.
        elseif(!$this->def_rekey_time and $rekey_time >= (int)$this->initial_data["lifetime"]) {
            $this->errors[] = APIResponse\get(2197);
        }
        # Otherwise, input is valid.
        else {
            $this->validated_data["lifetime"] = (int)$this->initial_data["lifetime"];
        }

        # Now we know the lifetime value, populate defaults on the rand_time, rekey_time and rand_time if requested
        if ($this->def_rekey_time) {
            $this->validated_data["rekey_time"] = intval($this->validated_data["lifetime"] * 0.9);
        }
        if ($this->def_rand_time) {
            $this->validated_data["rand_time"] = intval($this->validated_data["lifetime"] * 0.1);
        }
    }

    private function __validate_pinghost() {
        # Check for the optional 'pinghost' field
        if (isset($this->initial_data["pinghost"])) {
            $this->validated_data["pinghost"] = (string)$this->initial_data["pinghost"];
        }
        else {
            $this->validated_data["pinghost"] = "";
        }
    }

    private function __validate_keepalive() {
        # Validate the optional 'keepalive' field
        if ($this->initial_data["keepalive"] === true) {
            $this->validated_data["keepalive"] = "enabled";
        }
        else {
            $this->validated_data["keepalive"] = "disabled";
        }
    }
}
