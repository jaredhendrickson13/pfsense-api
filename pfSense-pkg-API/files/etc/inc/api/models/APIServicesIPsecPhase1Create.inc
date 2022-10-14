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

class APIServicesIPsecPhase1Create extends APIModel {
    public $def_rekey_time = false;
    public $def_reauth_time = false;
    public $def_rand_time = false;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-vpn-ipsec-editphase1"];
        $this->change_note = "Added IPsec phase 1 via API";
        $this->init_config("ipsec", "phase1");
        $this->init_config("ipsec", "phase2");
    }

    public function action() {
        # Write this entry to configuration
        $this->config["ipsec"]["phase1"][] = $this->validated_data;
        $this->write_config();
        mark_subsystem_dirty('ipsec');

        # Apply these changes if requested
        if ($this->initial_data["apply"] === true) {
            APIServicesIPsecApplyCreate::apply();
        }

        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Set the IKE ID for this new entry
        $this->validated_data["ikeid"] = ipsec_ikeid_next();

        # Validate individual fields. Do not change the order at which these are validated!
        $this->__validate_descr();
        $this->__validate_disabled();
        $this->__validate_iketype();
        $this->__validate_protocol();
        $this->__validate_interface();
        $this->__validate_remote_gateway();
        $this->__validate_authentication_method();
        $this->__validate_myid_type();
        $this->__validate_myid_data();
        $this->__validate_peerid_type();
        $this->__validate_peerid_data();
        $this->__validate_pre_shared_key();
        $this->__validate_certref();
        $this->__validate_caref();
        $this->__validate_mode();
        $this->__validate_rekey_time();
        $this->__validate_reauth_time();
        $this->__validate_rand_time();
        $this->__validate_lifetime();
        $this->__validate_startaction();
        $this->__validate_closeaction();
        $this->__validate_nat_traversal();
        $this->__validate_mobike();
        $this->__validate_splitconn();
        $this->__validate_prfselect_enable();
        $this->__validate_gw_duplicates();
        $this->__validate_ikeport();
        $this->__validate_nattport();
        $this->__validate_dpd_enable();
        $this->__validate_dpd_delay();
        $this->__validate_dpd_maxfail();
        $this->__validate_encryption();
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

    private function __validate_iketype() {
        # Check for the required 'iketype' field
        if (isset($this->initial_data["iketype"])) {
            # Ensure IKE type is known
            if (in_array($this->initial_data["iketype"], ["ikev1", "ikev2", "auto"])) {
                $this->validated_data["iketype"] = $this->initial_data["iketype"];
            }
            else {
                $this->errors[] = APIResponse\get(2160);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2159);
        }
    }

    private function __validate_protocol() {
        # Check for the required 'protocol' field
        if (isset($this->initial_data["protocol"])) {
            # Ensure protocol is known
            if (in_array($this->initial_data["protocol"], ["inet", "inet6", "both"])) {
                $this->validated_data["protocol"] = $this->initial_data["protocol"];
            }
            else {
                $this->errors[] = APIResponse\get(2162);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2161);
        }
    }

    private function __validate_interface() {
        # Check for the required 'interface' field
        if (isset($this->initial_data["interface"])) {
            # Check if this is an existing interface
            if (APITools\get_pfsense_if_id($this->initial_data["interface"])) {
                $this->validated_data["interface"] = APITools\get_pfsense_if_id($this->initial_data["interface"]);
            }
            # Check if this is an existing virtual IP
            elseif (APITools\is_vip($this->initial_data["interface"])) {
                $this->validated_data["interface"] = APITools\is_vip($this->initial_data["interface"]);
            }
            else {
                $this->errors[] = APIResponse\get(2164);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2163);
        }
    }

    private function __validate_remote_gateway() {
        # Check for the required 'remote-gateway' field
        if (isset($this->initial_data["remote-gateway"])) {
            # Ensure remote gateway is a valid IP address or hostname
            if (!is_ipaddr($this->initial_data["remote-gateway"]) and !is_fqdn($this->initial_data["remote-gateway"])) {
                $this->errors[] = APIResponse\get(2166);
            }
            # For IPv6 remote gateways, ensure the protocol is not 'inet'.
            elseif (is_ipaddrv6($this->initial_data["remote-gateway"]) and $this->validated_data["protocol"] === "inet") {
                $this->errors[] = APIResponse\get(2167);
            }
            # For IPv4 remote gateways, ensure the protocol is not 'inet6'.
            elseif (is_ipaddrv4($this->initial_data["remote-gateway"]) and $this->validated_data["protocol"] === "inet6") {
                $this->errors[] = APIResponse\get(2168);
            }
            # For domain name remote gateways, ensure the protocol is 'both'
            elseif (is_fqdn($this->initial_data["remote-gateway"]) and $this->validated_data["protocol"] !== "both") {
                $this->errors[] = APIResponse\get(2169);
            }
            # Ensure remote gateway is not already in use
            elseif ($this->is_ipsec_remote_gateway_in_use($this->initial_data["remote-gateway"])) {
                $this->errors[] = APIResponse\get(2170);
            }
            # Otherwise, the input is valid
            else {
                $this->validated_data["remote-gateway"] = $this->initial_data["remote-gateway"];
            }
        }
        else {
            $this->errors[] = APIResponse\get(2165);
        }
    }

    private function __validate_authentication_method() {
        # Check for the required 'authentication_method' field
        if ($this->initial_data["authentication_method"]) {
            # Ensure value is a valid option
            if (in_array($this->initial_data["authentication_method"], ["pre_shared_key", "cert"])) {
                $this->validated_data["authentication_method"] = $this->initial_data["authentication_method"];
            }
            else {
                $this->errors[] = APIResponse\get(2172);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2171);
        }
    }

    private function __validate_myid_type() {
        global $my_identifier_list;

        # Check for the required 'myid_type' field
        if (isset($this->initial_data["myid_type"])) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["myid_type"], $my_identifier_list)) {
                $this->validated_data["myid_type"] = $this->initial_data["myid_type"];
            }
            else {
                $this->errors[] = APIResponse\get(2174);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2173);
        }
    }

    private function __validate_myid_data() {
        # Create a shortname variable for myid_type for readability
        $myid_type = $this->validated_data["myid_type"];

        # Check for the conditional 'myid_data' field. Validate this field further if myid_type is not myaddress
        if (isset($this->initial_data["myid_data"]) and $myid_type !== "myaddress") {
            # Create a shortname variable for my myid_data for readability
            $myid_data = $this->initial_data["myid_data"];

            # Return an error if myid_type is address, but the data is not an IP address.
            if ($myid_type === "address" and !is_ipaddr($myid_data)) {
                $this->errors[] = APIResponse\get(2176);
            }
            # Return an error if myid_type is fqdn, but the data is not an FQDN.
            elseif ($myid_type === "fqdn" and !is_fqdn($myid_data)) {
                $this->errors[] = APIResponse\get(2177);
            }
            # Return an error if myid_type is user_fqdn, but the data is not an email address.
            elseif ($myid_type === "user_fqdn" and !filter_var($myid_data, FILTER_VALIDATE_EMAIL)) {
                $this->errors[] = APIResponse\get(2178);
            }
            # Return an error if myid_type is dyn_dns, but the data is not an FQDN.
            elseif ($myid_type === "dyn_dns" and !is_fqdn($myid_data)) {
                $this->errors[] = APIResponse\get(2179);
            }
            # Otherwise, this input is valid.
            else {
                $this->validated_data["myid_data"] = $myid_data;
            }
        }
        # Force this value to an empty string if myid_type is myaddress
        elseif ($myid_type === "myaddress") {
            $this->validated_data["myid_data"] = "";
        }
        # Otherwise, this field is required so return an error
        else {
            $this->errors[] = APIResponse\get(2175);
        }
    }

    private function __validate_peerid_type() {
        global $peer_identifier_list;

        # Check for the required 'peerid_type' field
        if ($this->initial_data["peerid_type"]) {
            # Ensure value is a valid option
            if (array_key_exists($this->initial_data["peerid_type"], $peer_identifier_list)) {
                $this->validated_data["peerid_type"] = $this->initial_data["peerid_type"];
            }
            else {
                $this->errors[] = APIResponse\get(2181);
            }
        }
        else {
            $this->errors[] = APIResponse\get(2180);
        }
    }

    private function __validate_peerid_data() {
        # Create a shortname variable for peerid_type for readability
        $peerid_type = $this->validated_data["peerid_type"];

        # Check for the conditional 'peerid_data' field. Validate this field further if myid_type is not myaddress
        if (isset($this->initial_data["peerid_data"]) and $peerid_type !== "myaddress") {
            # Create a shortname variable for my peerid_data for readability
            $peerid_data = $this->initial_data["peerid_data"];

            # Return an error if myid_type is address, but the data is not an IP address.
            if ($peerid_type === "address" and !is_ipaddr($peerid_data)) {
                $this->errors[] = APIResponse\get(2183);
            }
            # Return an error if myid_type is fqdn, but the data is not an FQDN.
            elseif ($peerid_type === "fqdn" and !is_fqdn($peerid_data)) {
                $this->errors[] = APIResponse\get(2184);
            }
            # Return an error if myid_type is user_fqdn, but the data is not an email address.
            elseif ($peerid_type === "user_fqdn" and !filter_var($peerid_data, FILTER_VALIDATE_EMAIL)) {
                $this->errors[] = APIResponse\get(2185);
            }
            # Return an error if myid_type is dyn_dns, but the data is not an FQDN.
            elseif ($peerid_type === "dyn_dns" and !is_fqdn($peerid_data)) {
                $this->errors[] = APIResponse\get(2186);
            }
            # Otherwise, this input is valid.
            else {
                $this->validated_data["peerid_data"] = $peerid_data;
            }
        }
        # Force this value to an empty string if myid_type is peeraddress
        elseif ($peerid_type === "peeraddress") {
            $this->validated_data["peerid_data"] = "";
        }
        # Otherwise, this field is required so return an error
        else {
            $this->errors[] = APIResponse\get(2182);
        }
    }

    private function __validate_pre_shared_key() {
        # This field is only available if authentication method is set to pre-shared key
        if ($this->validated_data["authentication_method"] === "pre_shared_key") {
            # Check for the required 'pre-shared-key' field
            if (isset($this->initial_data["pre-shared-key"])) {
                $this->validated_data["pre-shared-key"] = (string)$this->initial_data["pre-shared-key"];
            }
            else {
                $this->errors[] = APIResponse\get(2187);
            }
        }
    }

    private function __validate_certref() {
        # This field is only available if the authentication method is set to cert
        if ($this->validated_data["authentication_method"] === "cert") {
            # Check for the required 'certref' field
            if (isset($this->initial_data["certref"])) {
                # Ensure the value is an existing certificate's refid
                if (APITools\is_certificate($this->initial_data["certref"])) {
                    $this->validated_data["certref"] = $this->initial_data["certref"];
                }
                else {
                    $this->errors[] = APIResponse\get(2189);
                }
            }
            else {
                $this->errors[] = APIResponse\get(2188);
            }
        }
    }

    private function __validate_caref() {
        # This field is only available if the authentication method is set to cert
        if ($this->validated_data["authentication_method"] === "cert") {
            # Check for the required 'caref' field
            if (isset($this->initial_data["caref"])) {
                # Ensure the value is an existing CA's refid
                if (APITools\is_ca($this->initial_data["caref"])) {
                    $this->validated_data["caref"] = $this->initial_data["caref"];
                }
                else {
                    $this->errors[] = APIResponse\get(2191);
                }
            }
            else {
                $this->errors[] = APIResponse\get(2190);
            }
        }
    }

    private function __validate_mode() {
        # This field is only available if the iketype is set to 'ikev1' or 'auto'
        if ($this->validated_data["iketype"] === "ikev1" or $this->validated_data["iketype"] === "auto") {
            # Check for the optional 'mode' field
            if (isset($this->initial_data["mode"])) {
                # Ensure the mode is valid option
                if (in_array($this->initial_data["mode"], ["main", "aggressive"])) {
                    $this->validated_data["mode"] = $this->initial_data["mode"];
                }
                else {
                    $this->errors[] = APIResponse\get(2192);
                }
            }
            # Defaults to 'main' mode
            else {
                $this->validated_data["mode"] = "main";
            }
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

    private function __validate_reauth_time() {
        # Check for the optional 'reauth_time' field
        if (isset($this->initial_data["reauth_time"])) {
            # Ensure value is a numeric greater than or equal to 0
            if (is_numeric($this->initial_data["reauth_time"]) and (int)$this->initial_data["reauth_time"] >= 0) {
                $this->validated_data["reauth_time"] = (int)$this->initial_data["reauth_time"];
            }
            else {
                $this->errors[] = APIResponse\get(2194);
            }
        }
        # Sets a flag that will populate the default for this field. Default is 90% of the 'lifetime' value.
        else {
            $this->def_reauth_time = true;
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
        # Use shorthand variables for rekey_time, reauth_time for readability
        $rekey_time = $this->validated_data["rekey_time"];
        $reauth_time = $this->validated_data["reauth_time"];

        # Set default value if not specified. Even the default value needs to be validated for this field.
        if (!isset($this->initial_data["lifetime"])) {
            $this->initial_data["lifetime"] = 28800;
        }

        # Ensure value is a numeric greater than or equal to 0
        if (!is_numeric($this->initial_data["lifetime"]) or (int)$this->initial_data["lifetime"] < 0) {
            $this->errors[] = APIResponse\get(2196);
        }
        # If default 'rekey_time' was not assumed, ensure re-key time is less than lifetime.
        elseif(!$this->def_rekey_time and $rekey_time >= (int)$this->initial_data["lifetime"]) {
            $this->errors[] = APIResponse\get(2197);
        }
        # If default 'reauth_time' was not assumed, ensure re-key time is less than lifetime.
        elseif(!$this->def_reauth_time and $reauth_time >= (int)$this->initial_data["lifetime"]) {
            $this->errors[] = APIResponse\get(2197);
        }
        # Otherwise, input is valid.
        else {
            $this->validated_data["lifetime"] = (int)$this->initial_data["lifetime"];
        }

        # Now we know the lifetime value, populate defaults on the rand_time, rekey_time and reauth_time if requested
        if ($this->def_rekey_time) {
            $this->validated_data["rekey_time"] = intval($this->validated_data["lifetime"] * 0.9);
        }
        if ($this->def_reauth_time) {
            $this->validated_data["reauth_time"] = intval($this->validated_data["lifetime"] * 0.9);
        }
        if ($this->def_rand_time) {
            $this->validated_data["rand_time"] = intval($this->validated_data["lifetime"] * 0.1);
        }
    }

    private function __validate_startaction() {
        global $ipsec_startactions;
        $remotegw = $this->validated_data["remote-gateway"];

        # This field doesn't exist on pfSense 2.5.0-2.5.1, still allow these versions to set this field for tests.
        $ipsec_startactions = ($ipsec_startactions) ?: [""=>0, "none"=>0, "start"=>0, "trap"=>0];

        # Set default value if not specified. Even the default value needs to be validated for this field.
        if (!isset($this->initial_data["startaction"])) {
            $this->initial_data["startaction"] = "";
        }

        # Ensure this value is a known IPsec start action
        if (!array_key_exists($this->initial_data["startaction"], $ipsec_startactions)) {
            $this->errors[] = APIResponse\get(2198);
        }
        # Ensure the remote gateway is not 'any' unless the start action is none
        elseif (in_array($remotegw, ["0.0.0.0", "::"]) and $this->initial_data["startaction"] != "none") {
            $this->errors[] = APIResponse\get(2199);
        }
        # Otherwise, input is valid
        else {
            $this->validated_data["startaction"] = $this->initial_data["startaction"];
        }
    }

    private function __validate_closeaction() {
        global $ipsec_closeactions;

        # Check for the optional 'closeaction' field.
        if (isset($this->initial_data["closeaction"])) {
            # Check if this value is a valid option
            if (array_key_exists($this->initial_data["closeaction"], $ipsec_closeactions)) {
                $this->validated_data["closeaction"] = $this->initial_data["closeaction"];
            }
            else {
                $this->errors[] = APIResponse\get(2200);
            }
        }
        else {
            $this->validated_data["closeaction"] = "";
        }
    }

    private function __validate_nat_traversal() {
        # Check for the optional 'nat_traversal' option
        if (isset($this->initial_data["nat_traversal"])) {
            # Ensure value is valid option
            if (in_array($this->initial_data["nat_traversal"], ["on", "force"])) {
                $this->validated_data["nat_traversal"] = $this->initial_data["nat_traversal"];
            }
            else {
                $this->errors[] = APIResponse\get(2201);
            }
        }
        else {
            # Default to 'on'
            $this->validated_data["nat_traversal"] = "on";
        }
    }

    private function __validate_gw_duplicates() {
        # Check for the optional 'gw_duplicates' field
        if ($this->initial_data["gw_duplicates"] === true) {
            $this->validated_data["gw_duplicates"] = true;
        }
    }

    private function __validate_mobike() {
        # Check for the optional 'mobike' field
        if ($this->initial_data["mobike"] === true) {
            $this->validated_data["mobike"] = "on";
        }
        else {
            $this->validated_data["mobike"] = "off";
        }
    }

    private function __validate_splitconn() {
        # Check for the optional 'splitconn' field
        if ($this->initial_data["splitconn"] === true) {
            $this->validated_data["splitconn"] = true;
        }
    }

    private function __validate_prfselect_enable() {
        # Check for the optional 'prfselect_enable' field
        if ($this->initial_data["prfselect_enable"] === true) {
            $this->validated_data["prfselect_enable"] = "yes";
        }
    }

    private function __validate_ikeport() {
        # Check for the optional 'ikeport' field
        if (isset($this->initial_data["ikeport"])) {
            # Ensure value is numeric and a valid TCP/UDP port
            if (is_numeric($this->initial_data["ikeport"]) and is_port((int)$this->initial_data["ikeport"])) {
                $this->validated_data["ikeport"] = (int)$this->initial_data["ikeport"];
            }
            else {
                $this->errors[] = APIResponse\get(2202);
            }
        }
        else {
            # Default to 500
            $this->validated_data["ikeport"] = 500;
        }
    }

    private function __validate_nattport() {
        # Check for the optional 'nattport' field
        if (isset($this->initial_data["nattport"])) {
            # Ensure value is numeric and a valid TCP/UDP port
            if (!is_numeric($this->initial_data["nattport"]) or !is_port((int)$this->initial_data["nattport"])) {
                $this->errors[] = APIResponse\get(2203);
            }
            # Ensure value is not the same as the IKE port
            elseif ($this->validated_data["ikeport"] === (int)$this->initial_data["nattport"]) {
                $this->errors[] = APIResponse\get(2204);
            }
            else {
                $this->validated_data["nattport"] = (int)$this->initial_data["nattport"];
            }
        }
        else {
            # Default to 500
            $this->validated_data["nattport"] = 4500;
        }
    }

    private function __validate_dpd_enable() {
        # Check for the optional 'dpd_enable' field
        if ($this->initial_data["dpd_enable"] === true) {
            $this->validated_data["dpd_enable"] = true;
        }
    }

    private function __validate_dpd_delay() {
        # This field is only available when `dpd_enable` is set
        if ($this->validated_data["dpd_enable"]) {
            # Check for the optional 'dpd_delay' field
            if (isset($this->initial_data["dpd_delay"])) {
                # Ensure value is a numeric value
                if (is_numeric($this->initial_data["dpd_delay"]) and (int)$this->initial_data["dpd_delay"] >= 0) {
                    $this->validated_data["dpd_delay"] = $this->initial_data["dpd_delay"];
                }
                else {
                    $this->errors[] = APIResponse\get(2205);
                }
            }
            else {
                # Default value
                $this->initial_data["dpd_delay"] = 10;
            }
        }
    }

    private function __validate_dpd_maxfail() {
        # This field is only available when `dpd_enable` is set
        if ($this->validated_data["dpd_enable"]) {
            # Check for the optional 'dpd_maxfail' field
            if (isset($this->initial_data["dpd_maxfail"])) {
                # Ensure value is a numeric value
                if (is_numeric($this->initial_data["dpd_maxfail"]) and (int)$this->initial_data["dpd_maxfail"] >= 0) {
                    $this->validated_data["dpd_maxfail"] = $this->initial_data["dpd_maxfail"];
                }
                else {
                    $this->errors[] = APIResponse\get(2206);
                }
            }
            else {
                # Default value
                $this->initial_data["dpd_maxfail"] = 5;
            }
        }
    }

    public function __validate_encryption() {
        # Use the value nested in the 'item' field if it is present
        # This is needed because pfSense stores encryption items within a redundant 'item' array.
        if (isset($this->initial_data["encryption"]["item"])) {
            $this->initial_data["encryption"] = $this->initial_data["encryption"]["item"];
        }

        # Check for the required 'encryption' field
        if (isset($this->initial_data["encryption"])) {
            # Ensure value is an array and contains at least 1 item
            if (is_array($this->initial_data["encryption"]) and count($this->initial_data["encryption"]) >= 1) {
                # Initialize the validated data array that will hold valid configuration
                $this->validated_data["encryption"] = ["item"=>array()];

                # Loop through each encryption item and ensure it is valid
                foreach ($this->initial_data["encryption"] as $enc_conf) {
                    # Call the APIServicesIPsecPhase1EncryptionCreate model to validate entries
                    $enc_create = new APIServicesIPsecPhase1EncryptionCreate();
                    $enc_create->initial_data = $enc_conf;
                    $enc_create->validate_payload(true, $this->validated_data["prfselect_enable"]);
                    $this->validated_data["encryption"]["item"][] = $enc_create->validated_data;

                    # Add any encountered errors to this model's error array
                    $this->errors = array_merge($this->errors,  $enc_create->errors);
                }
            }
            else {
                $this->errors[] = APIResponse\get(2218);
            }
        } else {
            $this->errors[] = APIResponse\get(2217);
        }
    }

    public function is_ipsec_remote_gateway_in_use($value) {
        # Loop through each IPsec phase 1 and see if the value is in use
        foreach ($this->config['ipsec']['phase1'] as $p1) {
            # Check if this phase 1 entries remote gateway matches the value
            if ($p1["remote-gateway"] === $value) {
                # Allow this value if it is all IPv4
                if ($value === "0.0.0.0") {
                    continue;
                }
                # Allow this value if it is all IPv6
                elseif ($value === "::") {
                    continue;
                }
                # Allow this value if the matched entry is disabled
                elseif (isset($p1["disabled"])) {
                    continue;
                }
                # Allow this entry if both phase 1 entries allow duplicate gateways
                elseif (isset($this->validated_data["gw_duplicates"]) and isset($p1["gw_duplicates"]) and is_ipaddr($value)) {
                    continue;
                }
                # Otherwise, this remote gateway is already in use
                else {
                    return true;
                }
            }
        }

        return false;
    }
}
